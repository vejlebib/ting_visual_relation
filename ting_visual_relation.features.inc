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

/**
 * Implements hook_default_page_manager_handlers_alter().
 */
function ting_visual_relation_default_page_manager_handlers_alter(&$data) {
  if (isset($data['search_ting_search_panel_context'])) {
    $data['search_ting_search_panel_context']->conf['display']->content['new-5929c0c7-25f7-4d61-8829-81a740e6f77e']->position = 1; /* WAS: 0 */
    $data['search_ting_search_panel_context']->conf['display']->content['new-c8426105-10ad-4014-8f20-41e3d37c15e7'] = (object) array(
          'pid' => 'new-c8426105-10ad-4014-8f20-41e3d37c15e7',
          'panel' => 'left_sidebar',
          'type' => 'ting_visual_relation_search',
          'subtype' => 'ting_visual_relation_search',
          'shown' => TRUE,
          'access' => array(),
          'configuration' => array(
            'context' => 'argument_string_1',
            'override_title' => 1,
            'override_title_text' => '',
          ),
          'cache' => array(),
          'style' => array(
            'settings' => NULL,
          ),
          'css' => array(),
          'extras' => array(),
          'position' => 0,
          'locks' => array(),
          'uuid' => 'c8426105-10ad-4014-8f20-41e3d37c15e7',
        ); /* WAS: '' */
    $data['search_ting_search_panel_context']->conf['display']->content['new-f5097d22-b99c-49be-a0df-6b45a7d0d83c']->position = 2; /* WAS: 1 */
    $data['search_ting_search_panel_context']->conf['display']->panels['left_sidebar'][1] = 'new-5929c0c7-25f7-4d61-8829-81a740e6f77e'; /* WAS: 'new-f5097d22-b99c-49be-a0df-6b45a7d0d83c' */
    $data['search_ting_search_panel_context']->conf['display']->panels['left_sidebar'][0] = 'new-c8426105-10ad-4014-8f20-41e3d37c15e7'; /* WAS: 'new-5929c0c7-25f7-4d61-8829-81a740e6f77e' */
    $data['search_ting_search_panel_context']->conf['display']->panels['left_sidebar'][2] = 'new-f5097d22-b99c-49be-a0df-6b45a7d0d83c'; /* WAS: '' */
  }
}
