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
  // Make sure the attributes and classes array is always set.
  if (!isset($vars['attributes_array'])) {
    $vars['attributes_array'] = array();
  }
  if (!isset($vars['classes_array'])) {
    $vars['classes_array'] = array();
  }
  $vars['classes_array'][] = 'relvis-request';
  $vars['classes_array'][] = 'ting-visual-relation-activator';
  $vars['classes_array'][] = 'ting-visual-relation-activator' . '-' . $type;
  $vars['attributes_array']['data-relvis-type'] = $type;
  switch ($type) {
    case 'external':
    case 'circular':
      $datawell_pid = rawurlencode($vars['object']->id);
      $vars['attributes_array']['data-relvis-id'] = $datawell_pid;
      break;
    case 'struct':
      $vars['attributes_array']['data-relvis-id'] = 'search:' . $vars['search'];
  }
  $vars['theme_hook_suggestions'] += array(
    'ting_visual_relation_activator__' . $type,
  );
}

/**
 * Implements hook_process_ting_visual_relation_activator().
 */
function template_process_ting_visual_relation_activator(&$vars) {
  $vars['attributes'] = drupal_attributes($vars['attributes_array']);
  $vars['classes'] = implode(' ', $vars['classes_array']);
}

/**
 * Implements theme_hook().
 *
 * Default implementation for ting_visual_relation_activator.
 */
function theme_ting_visual_relation_activator($vars) {
  $button = '<button class="' . $vars['classes'] . '" ' . $vars['attributes'] . '>' . $vars['text'] . '</button>';
  return '<div class="ting-visual-relation-activator-wrapper">' . $button . '</div>';
 }
