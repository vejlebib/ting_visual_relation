<?php

/**
 * @file
 *
 * Defines forms and page callbacks for the Ting visual relation module.
 */

// Load additianal includes for its helper functions
module_load_include('inc', 'ting_covers', 'ting_covers.pages');
module_load_include('client.inc', 'ting');

// The agency used for global posts.
define('TING_VISUAL_RELATION_GLOBAL_AGENCY', 870970);

/**
 * Get data about the ting objects.
 *
 * Ajax callback for: ting-visual-relation/get-ting-object
 */
function ting_visual_relation_get_ting_object($ding_entity_id = NULL) {
  // Initialise our response array.
  $response = array();

  // Only return something if the supplied $ding_entity_id results in a valid
  // object from the datawell. Doing it this way prevents invalid rows in the
  // ting_object table and there's practically no performance overhead since
  // ting_get_object caches the result.
  if (isset($ding_entity_id) && ting_get_object($ding_entity_id)) {
    $ding_entity = ding_entity_load($ding_entity_id);
    // Populate response-array with simple properties.
    $properties = _ting_visual_relation_ting_object_properties();
    foreach ($properties as $getter => $property) {
      if ($values = $ding_entity->{$getter}) {
        $values = is_array($values) ? $values : array($values);
        foreach ($values as $value) {
          $response[] = _ting_visual_relation_property_value_object($property, $value);
        }
      }
    }
    // Populate response-array with relations.
  	foreach ($ding_entity->relations as $relation) {
      $value = $relation->object->ding_entity_id;
      $data = _ting_visual_relation_property_value_object('relation', $value);
      $data->type = $relation->type;
  		$response[] = $data;
  	}
    // Populate response-array with cover url if possible.
  	if ($cover_url = ting_visual_relation_get_cover_url($ding_entity)) {
  		$response[] = _ting_visual_relation_property_value_object('cover', $cover_url);
  	}
  }

  ting_visual_relation_json_output($response);
}

/**
 * Helper-function to hold the simple ting_object properties we return.
 *
 * @return $properties
 *   An array with property getter-method as key and property name as value.
 */
function _ting_visual_relation_ting_object_properties() {
  return array(
    'id' => 'id',
    'title' => 'title',
    'creators' => 'creator',
    'subjects' => 'subject',
    'type' => 'type',
  );
}

/**
 * Helper-function to prepare a property-value data-object.
 *
 * @param $property
 *   The property.
 * @param $value
 *   The value of the property.
 * @return $data
 *   A data-object with propery and value set.
 */
function _ting_visual_relation_property_value_object($property, $value) {
  $data = new stdClass;
  $data->property = $property;
  $data->value = $value;
  return $data;
}

/**
 * Get data about the ting collections.
 *
 * Ajax callback for: ting-visual-relation/get-ting-collection
 */
function ting_visual_relation_get_ting_collection($ding_entity_id = NULL) {
  $response = array();
  if (isset($ding_entity_id) && ting_get_object($ding_entity_id, TRUE)) {
    $ding_entity_collection = ding_entity_collection_load($ding_entity_id);
    foreach ($ding_entity_collection->entities as $ding_entity) {
      $response[] = $ding_entity->id;
    }
  }
  ting_visual_relation_json_output($response);
}

/**
 * Get recommendations based on a ding_entity_id.
 *
 * Ajax callback for: ting-visual-relation/get-recommendations
 *
 * @param $count
 *   The number of recommendations to return. Defaults to 3.
 */
function ting_visual_relation_get_recommendations($ding_entity_id = NULL, $count = 3) {
  $response = array();
  // TODO: Implement caching of recommendations and check that first.
  if (isset($ding_entity_id)) {
    // For now we will only support object recommendations.
    $object = ting_get_object($ding_entity_id);
    if (!empty($object->record['dc:identifier']['dkdcplus:ISBN'])) {
      $identifier = $object->record['dc:identifier']['dkdcplus:ISBN'];
      $recommendations = ting_get_object_recommendations($identifier, $count);
      // Convert to data well 3 PID's
      foreach ($recommendations as $recommendation) {
        $local_id = $recommendation->localId;
        $owner_id = $recommendation->ownerId;
        // If it's a global post:
        if ($owner_id == TING_VISUAL_RELATION_GLOBAL_AGENCY) {
          $recommendation_id = ding_provider_build_entity_id($local_id);
        }
        // Else pass along the owner to build the entity id function.
        else {
          $recommendation_id = ding_provider_build_entity_id($local_id, $owner_id);
        }
        $response[] = $recommendation_id;
      }
    }
  }
  ting_visual_relation_json_output($response);
}

/**
 * Wrapper function for json output that handles jsonp request if a callback
 * is supplied. Otherwise it falls back to normal json output.
 */
function ting_visual_relation_json_output($response) {
  $callback = $_REQUEST['callback'];
  // If it's a jsonp request with a valid javascript function
  if (isset($callback) && _ting_visual_relation_validate_func_name($callback)) {
    $response = drupal_json_encode($response);
    // $response = _ting_visual_relation_fix_unicode_for_json($response);
    header('Content-type: text/javascript');
    echo $callback . '(' . $response . ');';
  }
  // If normal json request or invalid callback: Return normal json as fallback
  else {
    drupal_json_output($response);
  }
}


/**
 * Helper function to get URL of the cover of a data-well object.
 *
 * A simplification of the ting_covers_objects() function from
 * ting_covers_pages.inc in the ting_covers module. Here we only return a URL
 * for the cover and we only operate on one cover each call.
 *
 * TODO: Add option for admins to choose image_style for returned covers?
 *
 * @param $ding_entity
 *   A ding_entity wrapper-object for the data-well object the cover is
 *   being requested on.
 * @return $cover_url
 *   A URL for the local copy of the cover.
 *   FALSE if no cover was found.
 */
function ting_visual_relation_get_cover_url($ding_entity) {
  $cover_url = FALSE;
	$localId = $ding_entity->localId;
  // Don't do anything if the cover is a known negative.
  if (!cache_get('ting_covers:' . $localId)) {
    $path = ting_covers_object_path($localId);
    // If the cover is allready saved locally, just use that.
    if (file_exists($path)) {
      $cover_url = file_create_url($path);
    }
    // Otherwise try to download the cover.
    else {
    	try {
    		$service = new AdditionalInformationService(
    			variable_get('addi_wsdl_url'),
    			variable_get('addi_username'),
    			variable_get('addi_group'),
    			variable_get('addi_password')
    		);
    		$addi_info = $service->getByLocalIdentifier((object) array(
    			'localIdentifier' => $localId,
    			'libraryCode' => $ding_entity->ownerId,
    		));
        if (isset($addi_info[$localId]) && $addi_info = $addi_info[$localId]) {
      		$source_url = FALSE;
      		if ($addi_info->detailUrl) {
      			$source_url = $addi_info->detailUrl;
      		}
      		else if ($addi_info->thumbnailUrl) {
      			$source_url = $addi_info->thumbnailUrl;
      		}
      		if ($source_url) {
      			_ting_covers_pages_fetch_image($path, $source_url);
      			$cover_url = file_create_url($path);
      		}
          // Cache this for future reference to avoid unnecessary requests.
          else {
        		cache_set(
        		  'ting_covers:' . $local_id,
        			1, // Just need some value
        			'cache',
        			$_SERVER['REQUEST_TIME'] + TING_COVERS_DEFAULT_CACHE_LIFETIME
        		);
          }
        }
    	}
      catch (Exception $e) {
        watchdog(
        	'ting_visual_relation',
        	'Unable to retrieve covers from ADDI to the relation browser: %message',
        	array('%message' => $e->getMessage()),
        	WATCHDOG_ERROR
        );
      }
    }
  }
  return $cover_url;
}

/**
 * Helper function to validate a javascript function name. It's considered
 * valid if name only consist of alphanumeric characters and underscore.
 */
function _ting_visual_relation_validate_func_name($function_name) {
	return ctype_alnum(str_replace('_', '', $function_name));
}

/**
 * Helper function to replace escaped unicode characters with actual unicode
 * charaters.
 *
 * See: http://stackoverflow.com/questions/14523846/convert-unicode-from-json-string-with-php
 */
function _ting_visual_relation_fix_unicode_for_json($str) {
    $str = preg_replace("/\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})/e", 'chr(hexdec("$1")).chr(hexdec("$2")).chr(hexdec("$3")).chr(hexdec("$4"))', $str);
    $str = preg_replace("/\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})/e", 'chr(hexdec("$1")).chr(hexdec("$2")).chr(hexdec("$3"))', $str);
    $str = preg_replace("/\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})/e", 'chr(hexdec("$1")).chr(hexdec("$2"))', $str);
    $str = preg_replace("/\\\\u00([0-9a-f]{2})/e", 'chr(hexdec("$1"))', $str);
    return $str;
}
