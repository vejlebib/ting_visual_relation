<?php
/**
 * @file
 * ting_visual_relation.features.inc
 */

/**
 * Implements hook_field_group_info_alter().
 */
function ting_visual_relation_field_group_info_alter(&$data) {
  if (isset($data['group_ting_object_right_column|ting_object|ting_object|default'])) {
    $data['group_ting_object_right_column|ting_object|ting_object|default']->data['children'][7] = 'field_ting_visual_relations'; /* WAS: '' */
  }
}
