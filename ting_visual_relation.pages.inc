<?php

/**
 * @file
 *
 * Defines forms and page callbacks for the Ting visual relation module.
 */

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
		$data->value = $relation->object->ding_entity_id;
		$response[] = $data;
	}

	// TODO: Find a proper way to valide the javascript callback function
	$callback = $_REQUEST['callback'];
	// If it's a jsonp request
	if (isset($callback) && $callback != '') {
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
 * Inspired by ting_covers_objects() from ting_covers_pages.inc in the
 * ting_covers module.
 * 
 * @param $ding_entity
 *   A ding_entity wrapper-object for the data-well object the cover is 
 *   being requested on.
 * @return $cover_url
 *   A URL for the local copy of the cover.
 */
function _ting_visual_relation_get_cover($ding_entity) {

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
