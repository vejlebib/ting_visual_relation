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
	if (isset($ding_entity)) {
		// TODO: use ting_object_load() instead of ding_entity_load()?
		// It seems to be easier to get data with ding_entity, so we will use 
		// ding_entity_load() for now. We might need to use ting_object_load() 
		// instead if we run into performance issues.
		// module_load_include('client.inc', 'ting');
		// $ting_object = ting_get_object($id);
		$response = array();
		$data = new stdClass;
		$data->property = 'title';
		$data->value = $ding_entity->title;
		$response[] = $data;
		$callback = $_REQUEST['callback'];
		// If it's a jsonp request with a valid javascript callback function
		// For an explanation of the use of ctype_alumn see:
		// http://stackoverflow.com/questions/20958218/is-it-necessary-to-validate-or-escape-the-jsonp-callback-string	
		if (isset($callback) && ctype_alnum($callback)) {
			$response = drupal_json_encode($response);
			$response = _fix_unicode_for_json($response);
			//$response = drupal_json_encode($response);
			header('Content-type: text/javascript');
			echo $callback . '(' . $response . ');';
		}
		// If normal json request or invalid callback: Return normal json as fallback
		else {
			drupal_json_output($response);
		}
	}
}

/**
 * Replaces escaped unicode characters with actual unicode charaters.
 *
 * See: http://stackoverflow.com/questions/14523846/convert-unicode-from-json-string-with-php
 */
function _fix_unicode_for_json($str) {
    $str = preg_replace("/\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})/e", 'chr(hexdec("$1")).chr(hexdec("$2")).chr(hexdec("$3")).chr(hexdec("$4"))', $str);
    $str = preg_replace("/\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})/e", 'chr(hexdec("$1")).chr(hexdec("$2")).chr(hexdec("$3"))', $str);
    $str = preg_replace("/\\\\u00([0-9a-f]{2})\\\\u00([0-9a-f]{2})/e", 'chr(hexdec("$1")).chr(hexdec("$2"))', $str);
    $str = preg_replace("/\\\\u00([0-9a-f]{2})/e", 'chr(hexdec("$1"))', $str);
    return $str;
}
