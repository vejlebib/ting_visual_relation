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

  // Exported field_base: 'ting_visual_relations'
  $field_bases['ting_visual_relations'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'ting_visual_relations',
    'foreign keys' => array(),
    'indexes' => array(),
    'locked' => 0,
    'module' => 'ting_visual_relation',
    'settings' => array(),
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
