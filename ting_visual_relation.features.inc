<?php
/**
 * @file
 * ting_visual_relation_feature.features.inc
 */

/**
 * Implements hook_field_group_info_alter().
 */
function ting_visual_relation_field_group_info_alter(&$data) {
  if (isset($data['group_ting_object_right_column|ting_object|ting_object|default'])) {
    $data['group_ting_object_right_column|ting_object|ting_object|default']->data['children'][7] = 'ting_visual_relations'; /* WAS: '' */
  }
}

/**
 * Implements hook_field_default_field_instances_alter().
 */
function ting_visual_relation_field_default_field_instances_alter(&$data) {
  if (isset($data['ting_object-ting_object-ting_abstract'])) {
    $data['ting_object-ting_object-ting_abstract']['display']['default']['weight'] = 6; /* WAS: 5 */
  }
  if (isset($data['ting_object-ting_object-ting_series'])) {
    $data['ting_object-ting_object-ting_series']['display']['default']['weight'] = 7; /* WAS: 6 */
  }
  if (isset($data['ting_object-ting_object-ting_subjects'])) {
    $data['ting_object-ting_object-ting_subjects']['display']['default']['weight'] = 8; /* WAS: 7 */
  }
  if (isset($data['ting_object-ting_object-ding_entity_buttons'])) {
    $data['ting_object-ting_object-ding_entity_buttons']['display']['default']['weight'] = 9; /* WAS: 8 */
  }  
}
