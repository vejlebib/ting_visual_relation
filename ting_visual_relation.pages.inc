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
function ting_visual_relation_get_ting_object($id) {
	$response = array(1, 2, 3);
	$callback = $_REQUEST['callback'];
	// If it's a jsonp callback request with a prober Javascript function
	if (isset($callback) && $callback != '') {
		$result = preg_match('/^[$A-Z_][0-9A-Z_$]*$/', $callback);
		file_put_contents("/home/drupalpro/debug/debug.txt", print_r($result, TRUE), FILE_APPEND);
		$response = drupal_json_encode($response);
		header('Content-type: text/javascript');
		echo $callback . '(' . $response . ');';
	}
	// Else return normal json
	else {
		drupal_json_output($response);
	}
	// ting_get_object or ding_entity_load ?
}
