<?php

/**
 * @file
 *
 * Defines forms and page callbacks for the Ting visual relation module.
 */

// Load ting_covers.pages.inc for its helper functions
module_load_include('inc', 'ting_covers', 'ting_covers.pages');

/**
 * Get data about the ting objects.
 *
 * Ajax callback for: ting-visual-relation/get-ting-object
 */
function ting_visual_relation_get_ting_object($ding_entity) {
	// TODO: use ting_get_object() instead of ding_entity_load()?
	// It seems to be easier to get data with ding_entity, so we will use
	// ding_entity_load() for now. We might need to use ting_get_object()
	// instead if we run into performance issues.
	// module_load_include('client.inc', 'ting');
	// $ting_object = ting_get_object($id);

  $response = array();

  // Populate response-array with simple properties.
  $properties = _ting_visual_relation_ting_object_properties();
  foreach ($properties as $getter => $name) {
    $property = $ding_entity->{$getter};
    $property = is_array($property) ? $property : array($property);
    foreach ($property as $value) {
      $response[] = _ting_visual_relation_property_value_object($name, $value);
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

	$callback = $_REQUEST['callback'];
	// If it's a jsonp request with a valid javascript function
	if (isset($callback) && _ting_visual_relation_validate_func_name($callback)) {
    ting_visual_relation_jsonp_output($response, $callback);
	}
	// If normal json request or invalid callback: Return normal json as fallback
	else {
		drupal_json_output($response);
	}
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
  );
}

/**
 * Helper-function to prepare a property-value data-object.
 *
 * @param $name
 *   The name of the property.
 * @param $value
 *   The value of the property.
 * @return $data
 *   A data-object with propery and value set.
 */
function _ting_visual_relation_property_value_object($name, $value) {
  $data = new stdClass;
  $data->property = $name;
  $data->value = $value;
  return $data;
}

/**
 * Get data about the ting collections.
 *
 * Ajax callback for: ting-visual-relation/get-ting-collection
 */
function ting_visual_relation_get_ting_collection($ding_entity_id = NULL) {

}

/**
 * Return data in JSONP format.
 */
function ting_visual_relation_jsonp_output($response, $callback) {
  $response = drupal_json_encode($response);
  // $response = _ting_visual_relation_fix_unicode_for_json($response);
  header('Content-type: text/javascript');
  echo $callback . '(' . $response . ');';
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
