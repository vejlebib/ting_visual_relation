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
 * Visual relation app settings form builder.
 */
function ting_visual_relation_app_settings_form($form, &$form_state) {
  // Tell Drupal not to flatten our form array on submit.
  $form['slides_table']['#tree'] = TRUE;
  $slides = db_select('ting_visual_relation_slides', 'tvrs')
    ->fields('tvrs')
    ->execute();
  foreach ($slides as $slide) {
    $form['slides_table'][$slide->slide_id] = array(
      // Relation browser type for the slide
      'type' => array(
        '#markup' => $slide->type,
      ),
      // Datawell PID if external or stuctural
      'datawell_pid' => array(
        '#markup' => empty($slide->datawell_pid) ? 'N/A' : $slide->datawell_pid,
      ),
      // Search query if structural
      'search_query' => array(
        '#markup' => empty($slide->search_query) ? 'N/A' : $slide->search_query,
      ),
      // The position in the table and in the slideshow
      'position' => array(
        '#type' => 'weight',
        '#title' => t('Position'),
        '#default_value' => $slide->position,
        '#delta' => 10,
        '#title_display' => 'invisible',
      ),
    );
  }
  $form['slides_table_actions'] = array('#type' => 'actions');
  // Add submit handler
  $form['slides_table_actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save changes'),
    '#submit' => array('ting_visual_relation_app_settings_form_table_submit'),
  );
  $form['slideshow_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Slideshow settings'),
    '#tree' => TRUE,
    '#weight' => 40,
  );
  return $form;
}

/**
 * Theme callback for the ting_visual_relation_app_settings_form form, that
 * adds tabledrag functionality.
 */
function theme_ting_visual_relation_app_settings_form(&$variables) {
  // Get the form passed along from the form-builder.
  // See: ting_visual_relation_app_settings_form above.
  $form = $variables['form'];

  // Setup the rows:
  $rows = array();
  foreach (element_children($form['slides_table']) as $slide_id) {
    // Add a custom class to the position column
    $form['slides_table'][$slide_id]['position']['#attributes'] = array(
      'class' => array('ting-visual-relation-slides-table-position'),
    );
    // Add each element to the rows-array
    $rows[] = array(
      'data' => array(
        // Type column
        drupal_render($form['slides_table'][$slide_id]['type']),
        // Datawell-PID column
        drupal_render($form['slides_table'][$slide_id]['datawell_pid']),
        // Search query column
        drupal_render($form['slides_table'][$slide_id]['search_query']),
        // Position column
        drupal_render($form['slides_table'][$slide_id]['position']),
      ),
      // Add the 'draggable' class to the tr-element
      'class' => array('draggable'),
    );
  }
  // Define the table-header values
  $header = array(t('Type'), t('Datawell-PID'), t('Search query'), t('Position'));

  // Id for out table (needed later)
  $table_id = 'ting-visual-relation-slides-table';

  // Theme the table output
  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => $table_id)
  ));

  // Render submit buttonn for slides table
  $output .= drupal_render($form['slides_table_actions']);

  // Render remaining form-elements
  $output .= drupal_render_children($form);

  // Add the tabledrag.js magic
  drupal_add_tabledrag($table_id, 'order', 'sibling', 'ting-visual-relation-slides-table-position');

  return $output;
}

/**
 * Submit hanlder for ting_visual_relation_app_settings_form.
 */
function ting_visual_relation_app_settings_form_table_submit($form, &$form_state) {

}

/**
 * Form for adding a slide to the Visual relation app.
 */
function ting_visual_relation_add_slide_form($form, &$form_state) {
  $form['add_slide'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add slide'),
  );
  // Relation browser type
  $form['add_slide']['type'] = array(
    '#type' => 'select',
    '#title' => t('Relation browser type'),
    '#description' => t('Select the type of relation browser slide to create'),
    '#options' => ting_visual_relation_types(),
    '#default_value' => 'external',
  );
  // Datawell PID - Only visable when type external or circular is selected.
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
  // Search query - Only visible when type structural is selected.
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
      if (empty($form_state['values']['search_query'])) {
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
