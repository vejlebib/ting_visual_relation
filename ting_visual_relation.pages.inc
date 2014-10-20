<?php

/**
 * @file
 *
 * Defines forms and page callbacks for the Ting visual relation module.
 */

// Load ting_covers.pages.inc for its helper functions
module_load_include('inc', 'ting_covers', 'ting_covers.pages');

/**
 * Get data about the ting object with the specified $id.
 *
 * Page callback for: ting-visual-relation/get-ting-object
 */
function ting_visual_relation_get_ting_object($ding_entity) {
	// TODO: use ting_get_object() instead of ding_entity_load()?
	// It seems to be easier to get data with ding_entity, so we will use
	// ding_entity_load() for now. We might need to use ting_get_object()
	// instead if we run into performance issues.
	// module_load_include('client.inc', 'ting');
	// $ting_object = ting_get_object($id);

	// TODO: This code needs to be refactored:
	$response = array();
	$data = new stdClass;
	$data->property = 'id';
	$data->value = $ding_entity->id;
	$response[] = $data;
	$data = new stdClass;
	$data->property = 'title';
	$data->value = $ding_entity->title;
	$response[] = $data;
	foreach ($ding_entity->creators as $creator) {
		$data = new stdClass;
		$data->property = 'creator';
		$data->value = $creator;
		$response[] = $data;
	}
	foreach ($ding_entity->subjects as $subject) {
		$data = new stdClass;
		$data->property = 'subject';
		$data->value = $subject;
		$response[] = $data;
	}
	foreach ($ding_entity->relations as $relation) {
		$data = new stdClass;
		$data->property = 'relation';
		$data->type = $relation->type;
		$data->value = $relation->object->ding_entity_id;
		$response[] = $data;
	}

	if ($cover_url = _ting_visual_relation_get_cover_url($ding_entity)) {
		$data = new stdClass;
		$data->property = 'cover';
		$data->value = $cover_url;
		$response[] = $data;
	}

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
 * Inspired by ting_covers_objects() method from ting_covers_pages.inc in the
 * ting_covers module.
 *
 * TODO: Add option for admins to choose image_style for returned covers?
 *
 * @param $ding_entity
 *   A ding_entity wrapper-object for the data-well object the cover is
 *   being requested on.
 * @return $cover_url
 *   A URL for the local copy of the cover.
 *   Ŕeturns FALSE if no cover was found.
 */
function _ting_visual_relation_get_cover_url($ding_entity) {
	$localId = $ding_entity->localId;
	// Determine if the local id is a known negative
  if (cache_get('ting_covers:' . $localId)) {
	  return FALSE;
  }
	$path = ting_covers_object_path($localId);
	// If the cover is allready saved locally then just use that
	if (file_exists($path)) {
		return file_create_url($path);
	}
	// Otherwise try to download the cover
	try {
		$service = new AdditionalInformationService(
			variable_get('addi_wsdl_url'),
			variable_get('addi_username'),
			variable_get('addi_group'),
			variable_get('addi_password')
		);
		$additinal_information = $service->getByLocalIdentifier((object) array(
			'localIdentifier' => $localId,
			'libraryCode' => $ding_entity->ownerId,
		));
		$additinal_information = $additinal_information[$localId];
		$source_url = FALSE;
		if ($additinal_information->detailUrl) {
			$source_url = $additinal_information->detailUrl;
		}
		else if ($additinal_information->thumbnailUrl) {
			$source_url = $additinal_information->thumbnailUrl;
		}
		if ($source_url) {
			$path = _ting_covers_pages_fetch_image($path, $source_url);
			return file_create_url($path);
		}
		 // No cover image found? Cache this for future reference to avoid
     // unnecessary requests and return FALSE.
		cache_set(
			'ting_covers:' . $local_id,
			1, // Just need some value
			'cache',
			$_SERVER['REQUEST_TIME'] + TING_COVERS_DEFAULT_CACHE_LIFETIME
		);
		return FALSE;
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
