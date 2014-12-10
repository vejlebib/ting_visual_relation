<?php
/**
 * @file
 * ting_visual_relation.features.features_overrides.inc
 */

/**
 * Implements hook_features_override_default_overrides().
 */
function ting_visual_relation_features_override_default_overrides() {
  // This code is only used for UI in features. Exported alters hooks do the magic.
  $overrides = array();

  // Exported overrides for: field_group
  $overrides["field_group.group_ting_object_right_column|ting_object|ting_object|default.data|children|7"] = 'field_ting_visual_relations';

 return $overrides;
}
