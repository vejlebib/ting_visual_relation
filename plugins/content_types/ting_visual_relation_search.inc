<?php

/**
 * @file
 *
 * Defines the Ting visual relation search visualization plugin.
 */

$plugin = array(
  'title' => t('Ting visual relation - Search visualization'),
  'description' => t('Exposes an activator for the relvis search visualization.'),
  'single' => TRUE,
  'content_types' => array('ting_visual_relation_search'),
  'required context' => new ctools_context_required(t('Keyword'), 'string'),
  'category' => t('Ting'),
  'render callback' => 'ting_visual_relation_search_content_type_render',
  'edit form' => 'ting_visual_relation_search_content_type_edit_form',
);

/**
 * Renders the Search visualization plugin.
 */
function ting_visual_relation_search_content_type_render($subtype, $conf, $args, $context) {
  $block = new stdClass();
  $block->title = t('Ting visual relation - Search visualization');
  $content = array();
  if (!empty($context) && isset($context->data)) {
    $d3 = libraries_load('d3');
    if (!empty($d3['installed']) && version_compare($d3['version'], '3.0') > 0) {
      $search = check_plain($context->data);
      $content = array(
        '#theme' => 'ting_visual_relation_activation_area',
        '#activators' => array(),
      );
      $content['#activators'][] = array(
        '#theme' => 'ting_visual_relation_activator',
        '#type' => 'struct',
        '#text' => t('Overview'),
        '#search' => $search,
      );
      ting_visual_relation_prepare_render_array($content);
    }
  }
  $block->content = $content;
  return $block;
}

/**
 * Enable admin settings page.
 */
function ting_visual_relation_search_content_type_edit_form($form, &$form_state) {
  return $form;
}
