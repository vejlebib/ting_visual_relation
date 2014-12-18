<?php

/**
 * @file
 *
 * Preprocess, process and theming functions for ting_visual_relation.
 */

/**
 * Implements hook_preprocess_ting_visual_relation_activator().
 */
function template_preprocess_ting_visual_relation_activator(&$vars) {
  $type = $vars['type'];
  $object = $vars['object'];
  // Might be set if the implementation is changed to a template?
  if (!isset($vars['attributes_array'])) {
    $vars['attributes_array'] = array();
  }
  $vars['attributes_array']['data-relvis-type'] = $type;
  switch ($type) {
    case 'external':
    case 'circular':
      $vars['attributes_array']['data-relvis-id'] = $vars['object']->id;
      break;
    case 'struct':
      $vars['attributes_array']['data-relvis-id'] = 'search:' . $vars['search'];
  }
}
