<?php
/**
 * @file
 * ting_visual_relation_feature.features.features_overrides.inc
 */

/**
 * Implements hook_features_override_default_overrides().
 */
function ting_visual_relation_features_override_default_overrides() {
  // This code is only used for UI in features. Exported alters hooks do the magic.
  $overrides = array();

  // Exported overrides for: field_group
  $overrides["field_group.group_ting_object_right_column|ting_object|ting_object|default.data|children|7"] = 'ting_visual_relations';

    // Exported overrides for: field_instance
  $overrides["field_instance.ting_object-ting_object-ting_abstract.display|default|weight"] = 6;
  $overrides["field_instance.ting_object-ting_object-ting_series.display|default|weight"] = 7;
  $overrides["field_instance.ting_object-ting_object-ting_subjects.display|default|weight"] = 8;
  $overrides["field_instance.ting_object-ting_object-ding_entity_buttons.display|default|weight"] = 9;

 return $overrides;
}
