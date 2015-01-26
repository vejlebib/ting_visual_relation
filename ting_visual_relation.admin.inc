<?php

/**
 * @file
 *
 * Administration interface for the Ting visual relation module.
 */

/**
 * Basic admin settings.
 */
function ting_visual_relation_settings_form($form, &$form_state) {
  return $form;
}

/**
 * Visual relation app settings.
 */
function ting_visual_relation_app_settings_form($form, &$form_state) {
  return $form;
}

/**
 * Form for adding a slide to the Visual relation app.
 */
function ting_visual_relation_add_slide_form($form, &$form_state) {
  $form['add_slide'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add slide'),
  );
  $form['add_slide']['type'] = array(
    '#type' => 'select',
    '#title' => t('Relation browser type'),
    '#description' => t('Select the type of relation browser slide to create'),
    '#options' => ting_visual_relation_types(),
    '#default_value' => 'external',
  );
  $form['add_slide']['datawell_pid'] = array(
    '#type' => 'textfield',
    '#title' => t('Datawell PID'),
    '#description' => t('Enter the datawell PID the browser should be based on'),
    '#states' => array(
      'visible' => array(
        ':input[name="type"]' => array(
          array('value' => 'external'),
          array('value' => 'circular'),
        ),
      ),
    ),
  );
  $form['add_slide']['search_query'] = array(
    '#type' => 'textfield',
    '#title' => t('Search query'),
    '#description' => t('Enter the search query the browser should be based on'),
    '#states' => array(
      'visible' => array(
        ':input[name="type"]' => array('value' => 'structural'),
      ),
    ),
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save slide'),
  );
  return $form;
}

/**
 * Validation handler for ting_visual_relation_add_slide_form.
 */
function ting_visual_relation_add_slide_form_validate($form, &$form_state) {
  $type = $form_state['values']['type'];
  switch ($type) {
    case 'external':
    case 'circular':
      $datawell_pid = $form_state['values']['datawell_pid'];
      // TODO: use a regex to validate the datawell-PID.
      if (empty($datawell_pid)) {
        form_set_error('datawell_pid', t('Please enter a valid datawell-PID'));
      }
      break;
    case 'structural':
      if ($form_state['values']['search_query']) {
        form_set_error('search_query', t('Please enter a search query'));
      }
  }
}

/**
 * Submit handler for ting_visual_relation_add_slide_form.
 */
function ting_visual_relation_add_slide_form_submit($form, &$form_state) {
  $type = $form_state['values']['type'];
  $fields = array('type' => $type);
  switch ($type) {
    case 'external':
    case 'circular':
      $fields['datawell_pid'] = $form_state['values']['datawell_pid'];
      break;
    case 'structural':
      $fields['search_query'] = $form_state['values']['search_query'];
  }
  // Save and go back to table overview.
  db_insert('ting_visual_relation_slides')->fields($fields)->execute();
  drupal_set_message(t('Slide saved'));
  $form_state['redirect'] = 'admin/config/ting/ting-visual-relation/app-settings';
}
