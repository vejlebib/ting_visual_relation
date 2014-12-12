<?php

/**
 * @file
 *
 * Defines page and AJAX callbacks for the Ting visual relation module.
 */

// Load additianal includes for its helper functions
module_load_include('inc', 'ting_covers', 'ting_covers.pages');
module_load_include('client.inc', 'ting');

// The agency used for global posts.
define('TING_VISUAL_RELATION_GLOBAL_AGENCY', 870970);
// The default number of search results to return.
define('TING_VISUAL_RELATION_DEFAULT_SEARCH_SIZE', 100);

/**
 * Outputs data in json format for the ting object with the datawell PID.
 *
 * AJAX callback for /ting-visual-relation/get-ting-object
 */
function ting_visual_relation_get_ting_object($datawell_pid = NULL) {
  $response = ting_visual_relation_get_ting_object_array($datawell_pid);
  ting_visual_relation_json_output($response);
}

/**
 * Outputs data in json format for every ting object in the collection with the
 * specified datawell PID.
 *
 * Ajax callback for: ting-visual-relation/get-ting-collection
 */
function ting_visual_relation_get_ting_collection($datawell_pid = NULL) {
  $response = array();
  if (isset($datawell_pid) && ting_get_object($datawell_pid, TRUE)) {
    $ting_collection = ding_entity_collection_load($datawell_pid);
    foreach ($ting_collection->entities as $ting_entity) {
      $response[] = ting_visual_relation_get_ting_object_array($ting_entity->id);
    }
  }
  ting_visual_relation_json_output($response);
}

/**
 * Returns an array with data for the ting object with the specified PID. The
 * array is populated with property-value objects describing the ting object.
 *
 * @param  $datawell_pid
 *   The datawell PID for the ting object.
 * @return  $return
 *   An array with data for the ting object.
 *   An empty array if the supplied PID didn't result in a valid object.
 */
function ting_visual_relation_get_ting_object_array($datawell_pid) {
  $return = array();
  // Only return something if the supplied $datawell_pid results in a valid
  // object from the datawell. Doing it this way prevents invalid rows in the
  // ting_object table and there's practically no performance overhead since
  // ting_get_object caches the result.
  if (isset($datawell_pid) && ting_get_object($datawell_pid)) {
    // Get a Drupal entity (ting entity) that wraps the datawell object. The
    // ding_entity module has a common API for all DDB CMS specific entities,
    // so we use that to load the entity (ding_entity_load()).
    // We could have used the ting object directly, but the ting entity has
    // methods for easy retrieval of data.
    $ting_entity = ding_entity_load($datawell_pid);

    // Populate response-array with simple properties.
    $properties = _ting_visual_relation_ting_entity_properties();
    foreach ($properties as $getter => $property) {
      if (isset($ting_entity->{$getter})) {
        $values = $ting_entity->{$getter};
        $values = is_array($values) ? $values : array($values);
        foreach ($values as $value) {
          // Filter out FALSE and empty values.
          if (!empty($value)) {
            $return[] = _ting_visual_relation_property_value_object($property, $value);
          }
        }
      }
    }

    // - with relations if any.
    if (!empty($ting_entity->relations)) {
      foreach ($ting_entity->relations as $relation) {
        $value = NULL;
        // Some relations have a URI for online access.
        if (isset($relation->uri)) {
          $value = $relation->uri;
        }
        // Otherwise use PID as value for the relation.
        else if (is_object($relation->object)) {
          $value = $relation->object->ding_entity_id;
        }
        $data = _ting_visual_relation_property_value_object('relation', $value);
        $data->type = $relation->type;
        $return[] = $data;
      }
    }

    // - with cover url if possible.
    if ($cover_url = ting_visual_relation_get_cover_url($ting_entity)) {
      $return[] = _ting_visual_relation_property_value_object('cover', $cover_url);
    }
    // If no cover was found, check if it's possible to return a default cover.
    else if ($cover_url = ting_visual_relation_get_default_cover_url($ting_entity)) {
      $return[] = _ting_visual_relation_property_value_object('default_cover', $cover_url);
    }

    // - with fulltext if available.
    if (!empty($ting_entity->reply->formatsAvailable) && in_array('docbook', $ting_entity->reply->formatsAvailable)) {
      module_load_include('inc', 'ting_fulltext', 'includes/ting_fulltext.pages');
      // Get the fulltext data from the datawell.
      $fulltext = ting_fulltext_parse(ting_fulltext_object_load($ting_entity->id));
      $value = array();
      if (isset($fulltext['title'])) {
        $value[] = _ting_visual_relation_property_value_object('title', $fulltext['title']);
      }
      $creator = isset($fulltext['surname']) ? $fulltext['surname'] . ', ' : '';
      $creator .= isset($fulltext['firstname']) ? $fulltext['firstname'] : '';
      if (!empty($creator)) {
        $value[] = _ting_visual_relation_property_value_object('creator', $creator);
      }
      if (isset($fulltext['subject'])) {
        foreach ($fulltext['subject'] as $subject) {
          $value[] = _ting_visual_relation_property_value_object('subject', $subject);
        }
      }
      if (!empty($fulltext['section'])) {
        foreach ($fulltext['section'] as $section) {
          $section_value = array();
          $section_value[] = _ting_visual_relation_property_value_object('title', $section['title']);
          $section_value[] = _ting_visual_relation_property_value_object('text', $section['para']);
          $value[] = _ting_visual_relation_property_value_object('section', $section_value);
        }
      }
      $return[] = _ting_visual_relation_property_value_object('fulltext', $value);
    }
  }
  return $return;
}

/**
 * Helper-function to hold the Ting entity properties we return.
 * Some properties requires special handling, and are therefor not inc. here.
 *
 * @return $properties
 *   An array with property getter-method as key and property name as value.
 */
function _ting_visual_relation_ting_entity_properties() {
  return array(
    'id' => 'id',
    'localId' => 'localId',
    'ownerId' => 'ownerId',
    'title' => 'title',
    'description' => 'description',
    'abstract' => 'abstract',
    'creators' => 'creator',
    'subjects' => 'subject',
    'type' => 'type',
    'isPartOf' => 'isPartOf',
    'isbn' => 'isbn',
    'serieNumber' => 'serieNumber',
    'serieTitle' => 'serieTitle',
    'serieDescription' => 'serieDescription',
    'language' => 'language',
    'date' => 'date',
    'online_url' => 'onlineUrl',
    'ac_source' => 'acSource',
    'contributers' => 'contributer',
    'extent' => 'extent',
    'classification' => 'classification',
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
 * Get recommendations based on a ding_entity_id.
 *
 * Ajax callback for: ting-visual-relation/get-recommendations
 *
 * @param $count
 *   If possible, this is the number of recommendation that will be returned.
 *   The amount returned may be less than count, but will never exceed it.
 */
function ting_visual_relation_get_recommendations($datawell_pid = NULL, $count = 3) {
  $response = array();
  // TODO: Implement caching of recommendations and check that first.
  if (isset($datawell_pid) && $object = ting_get_object($datawell_pid)) {
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
    // Sometimes the ADHL-service returns an empty response when it shouldn't.
    // in this case we try once more.
    if (empty($recommendations)) {
      $recommendations = ting_get_object_recommendations($identifier, $count);
    }
    // Convert to datawell PID's
    foreach ($recommendations as $recommendation) {
      $local_id = $recommendation->localId;
      $owner_id = $recommendation->ownerId;
      // If it's a global post
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
function ting_visual_relation_get_availability($datawell_pid = NULL) {
  $response = array();
  if (ding_provider_implements('availability', 'items') && isset($datawell_pid)) {
    // Extract the local id from the PID.
    $datawell_pid = explode(':', $datawell_pid);
    // Make sure the local-id of the PID is set.
    if (isset($datawell_pid[1])) {
      $local_id = $datawell_pid[1];
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
 * Returns the search result for the specified engine and query in a well
 * structured json array.
 *
 * Ajax callback for: ting-visual-relation/get-search-result.
 */
function ting_visual_relation_get_search_result($search_engine = NULL, $search_query = '') {
  $response = array();
  if (isset($search_engine)) {
    $search_engine = str_replace(array('-', ' '), '_', $search_engine);
    $search_info = search_get_info();
    // Need a valid search engine to conteniue.
    if (isset($search_info[$search_engine])) {
      $search_info = $search_info[$search_engine];
      $conditions = $search_info['conditions_callback']($search_query);
      // Only search if there is a search query supplied or non-empty conditions.
      // Sort is always set, so we check for more than 1 condition.
      if (count($conditions) > 1 || !empty($search_query)) {
        if (!isset($conditions['size'])) {
          $conditions['size'] = TING_VISUAL_RELATION_DEFAULT_SEARCH_SIZE;
        }
        $result = search_data($search_query, $search_info['module'], $conditions);
        // If the search engine isn't node, there's better ways to get results.
        if ($search_engine != 'node') {
          if ($search_engine == 'ting_search') {
            $result = drupal_static('ting_search_results');
          }
          else if ($search_engine == 'ting_extrasearch') {
            $result = drupal_static('ting_extrasearch_results');
          }
          $response = new stdClass;
          // Setup collections if any.
          if (!empty($result->collections)) {
            $response->collections = array();
            foreach ($result->collections as $collection) {
              $data = new stdClass;
              $data->pid = $collection->id;
              $data->primaryObject = $collection->primary_object->id;
              $data->entities = array();
              foreach ($collection->getEntities() as $entity) {
                $data->entities[] = $entity->ding_entity_id;
              }
              $response->collections[] = $data;
            }
          }
          // Setup facets if any.
          if (!empty($result->facets)) {
            $response->facets = array();
            foreach ($result->facets as $facet_name => $facet) {
              $data = new stdClass;
              $facet_name = explode('.', $facet_name);
              $data->name = isset($facet_name[1]) ? $facet_name[1] : array_shift($facet_name);
              $data->terms = array();
              foreach ($facet->terms as $term_name => $term_count) {
                $term_data = new stdClass;
                $term_data->name = $term_name;
                $term_data->count = $term_count;
                $data->terms[] = $term_data;
              }
              $response->facets[] = $data;
            }
          }
        }
        // TODO: Handle node search.
        else {

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
 * @param $ting_entity
 *   A Drupal entity (ting entity) that wraps the datawell-object the cover
 *   is being requested on.
 * @return $cover_url
 *   A URL for the local copy of the cover.
 *   FALSE if no cover was found.
 */
function ting_visual_relation_get_cover_url($ting_entity) {
  $cover_url = FALSE;
	$localId = $ting_entity->localId;
  // Don't do anything if the cover is a known negative.
  if (!cache_get('ting_covers:' . $localId)) {
    $path = ting_covers_object_path($localId);
    // If the cover is already saved locally, just use that.
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
 * Helper function to get a default cover for a datawell object, if there was
 * no cover found in the addi-service.
 *
 * @param $ting_entity
 *   A Drupal entity (ting entity) that wraps the datawell-object the cover
 *   is being requested on.
 * @return $cover_url
 *   A URL for the local copy of the cover.
 *   FALSE if no cover was found.
 */
function ting_visual_relation_get_default_cover_url($ting_entity) {
  $cover_url = FALSE;
  // Default covers is provided by ting_covers_plus module. As it's not a
  // standard DDB CMS module, we dont wanna depend on it.
  if (module_exists('ting_covers_plus')) {
    module_load_include('inc', 'ting_covers_plus', 'ting_covers_plus.pages');
    $mtype = mb_strtolower($ting_entity->type);
    $source = mb_strtolower($ting_entity->ac_source);
    $path = _ting_covers_plus_get_default_image($mtype, $source);
    if (isset($path)) {
      $cover_url = file_create_url($path);
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
