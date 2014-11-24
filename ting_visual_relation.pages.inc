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
// The default number of search results to return.
define('TING_VISUAL_RELATION_DEFAULT_SEARCH_SIZE', 100);

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
 *   If possible, this is the number of recommendation that will be returned.
 *   The amount returned may be less than count, but will never exceed it.
 */
function ting_visual_relation_get_recommendations($ding_entity_id = NULL, $count = 3) {
  $response = array();
  // TODO: Implement caching of recommendations and check that first.
  if (isset($ding_entity_id) && $object = ting_get_object($ding_entity_id)) {
    $identifier = array();
    // Get the identifier to pass along to the adhl-webservice.
    if (!empty($object->record['dc:identifier']['dkdcplus:ISBN'])) {
      $identifier = $object->record['dc:identifier']['dkdcplus:ISBN'];

    }
    else if (!empty($object->record['dc:identifier']['dkdcplus:ISSN'])) {
      $identifier = $object->record['dc:identifier']['dkdcplus:ISSN'];
    }
    foreach ($identifier as $key => $number) {
      $identifier[$key] = str_replace(array(' ', '-'), '', $number);
    }
    rsort($identifier);
    // The ADHL-service doesn't like 2 identifiers. To prevent this we always
    // pick the first, if the identifier is an array.
    $identifier = is_array($identifier) ? array_shift($identifier) : $identifier;
    $recommendations = ting_get_object_recommendations($identifier, $count);
    // Convert to data well 3 PID's
    foreach ($recommendations as $recommendation) {
      $local_id = $recommendation->localId;
      $owner_id = $recommendation->ownerId;
      // If it's a global post:
      if ($owner_id == TING_VISUAL_RELATION_GLOBAL_AGENCY) {
        $recommendation_id = ding_provider_build_entity_id($local_id);
      }
      // Else pass along the owner to the build entity id function.
      else {
        $recommendation_id = ding_provider_build_entity_id($local_id, $owner_id);
      }
      $response[] = $recommendation_id;
    }
  }
  ting_visual_relation_json_output($response);
}

/**
 * Returns information about the availability of a datawell object.
 *
 * Ajax callback for: ting-visual-relation/get-availability.
 */
function ting_visual_relation_get_availability($ding_entity_id = NULL) {
  $response = array();
  if (ding_provider_implements('availability', 'items')) {
    // Extract the local id from the ding entity id.
    $ding_entity_id = explode(':', $ding_entity_id);
    // Just to be safe
    if (isset($ding_entity_id[1])) {
      $local_id = $ding_entity_id[1];
      // Get the availability information
      $info = ding_provider_invoke('availability', 'items', array($local_id));
      $response = $info[$local_id];
      // Make sure everything is set.
      $response += array(
        'reservable' => FALSE,
        'available' => FALSE,
      );
      // Marks internet resources as available.
      if (isset($response['is_internet']) && $response['is_internet']) {
        $response['available'] = TRUE;
      }
    }
  }
  ting_visual_relation_json_output($response);
}

/**
 *
 */
function ting_visual_relation_get_search_result($search_engine = NULL, $search_query = '') {
  $response = array();
  if (isset($search_engine) ) {
    $search_engine = str_replace('-', '_', $search_engine);
    $search_info = search_get_info();
    $search_info = $search_info[$search_engine];
    $conditions = $search_info['conditions_callback']($search_query);
    // Only search if there is a search query supplied or non-empty conditions.
    // count($conditions > 1) because sort is always set (see ting_search_conditions_callback())
    if (count($conditions) > 1 || !empty($search_query)) {
      if (!isset($conditions['size'])) {
        $conditions['size'] = TING_VISUAL_RELATION_DEFAULT_SEARCH_SIZE;
      }
      $results = search_data($search_query, $search_info['module'], $conditions);
      $data = new stdClass;
      $data->collections = array();
      if ($search_engine == 'ting_search') {
        $results = drupal_static('ting_search_results');
        foreach ($results->collections as $collection) {
          file_put_contents("/home/drupalpro/debug/debug.txt", print_r($, TRUE), FILE_APPEND);
          break;
        }
      }
    }
  }
  ting_visual_relation_json_output($response);
}

/**
 * Returns information about the internal structure a ting object belongs to.
 *
 * TODO: This function needs a lot of refactoring.
 *
 * Ajax callback for: ting-visual-relation/get-structure
 */
function ting_visual_relation_get_structure($ting_object_id = NULL) {
  $response = array();
  if (isset($ting_object_id) && $object = ting_get_object($ting_object_id, FALSE, TRUE))  {
    // TODO: Maybe it's better to use ting object entity??? (ding entity)
    $type = !empty($object->record['dc:type']['dkdcplus:BibDK-Type']) ? $object->record['dc:type']['dkdcplus:BibDK-Type'] : FALSE;
    if ($type && is_array($type)) {
      if (in_array('Avisartikel', $type)) {
        $data = new stdClass;
        // Try to get the host-publication
        $host_object_id = FALSE;
        foreach ($object->relationsData as $relation) {
          if ($relation->relationType == 'dbcbib:isPartOfManifestation') {
            $host_object_id = $relation->relationObject->id;
            break;
          }
        }
        if ($host_object_id) {
          $data->host = $host_object_id;
        }
        // Try to get the children
        $children = array();
        if (!empty($object->record['dcterms:isPartOf'])) {
          $query = '';
          foreach ($object->record['dcterms:isPartOf'] as $type => $is_part_of) {
            if ($type == '') {
              $query = is_array($is_part_of) ? $is_part_of[0] : $is_part_of;
              break;
            }
          }
          $result = ting_do_search($query);
          foreach ($result->collections as $id => $collection) {
            $children[] = $id;
          }
        }
        if (!empty($children)) {
          $data->children = $children;
        }
        $response = $data;
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
  isset($_REQUEST['callback']) ? $callback = $_REQUEST['callback'] : $callback = FALSE;
  // If it's a jsonp request with a valid javascript function
  if ($callback && _ting_visual_relation_validate_func_name($callback)) {
    $response = drupal_json_encode($response);
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
