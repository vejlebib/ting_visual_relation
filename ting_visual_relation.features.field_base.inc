<?php
/**
 * @file
 * ting_visual_relation_feature.features.field_base.inc
 */

/**
 * Implements hook_field_default_field_bases().
 */
function ting_visual_relation_field_default_field_bases() {
  $field_bases = array();

  // Exported field_base: 'field_ting_visual_relations'
  $field_bases['field_ting_visual_relations'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(
      0 => 'ting_object',
      1 => 'ting_collection',
    ),
    'field_name' => 'field_ting_visual_relations',
    'foreign keys' => array(),
    'indexes' => array(),
    'locked' => 0,
    'module' => 'ting_visual_relation',
    'settings' => array(
      'profile2_private' => FALSE,
    ),
    'storage' => array(
      'active' => 1,
      'module' => 'virtual_field',
      'settings' => array(),
      'type' => 'virtual_field',
    ),
    'translatable' => 0,
    'type' => 'ting_visual_relations',
  );

  return $field_bases;
}
