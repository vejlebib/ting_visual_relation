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
    ->orderBy('position', 'ASC')
    ->execute();
  foreach ($slides as $slide) {
    $slide_id = $slide->slide_id;
    $form['slides_table'][$slide_id] = array(
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
      'edit' => array(
        '#type' => 'link',
        '#title' => t('Edit'),
        '#href' => 'admin/config/ting/ting-visual-relation/app-settings/' . $slide_id . '/edit'
      ),
    );
  }
  // Time between each slide.
  $intervals = array(
    30,
    60,
    90,
    120,
    150,
    180,
    210,
    240,
    270,
    300,
  );
  $options = array();
  foreach ($intervals as $interval) {
    $options[$interval] = format_interval($interval, 2);
  }
  $form['interval'] = array(
    '#type' => 'select',
    '#title' => t('Slideshow interval'),
    '#description' => t('Adjust the time a slide is shown in the slideshow before showing the next one.'),
    '#options' => $options,
    '#default_value' => variable_get('ting_visual_relation_slideshow_interval', 120),
  );
  $form['actions'] = array('#type' => 'actions');
  // Add submit handler
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );
  return $form;
}

/**
 * Theme callback for the ting_visual_relation_app_settings_form form, that
 * adds tabledrag functionality.
 */
function theme_ting_visual_relation_app_settings_form(&$variables) {
  // Render table title and description.
  $table_title = array(
    '#type' => 'html_tag',
    '#tag' => 'label',
    '#value' => t('Slideshow overview'),
  );
  $output = drupal_render($table_title);
  $table_description = array(
    '#type' => 'html_tag',
    '#tag' => 'div',
    '#value' => t('Manage settings and position for the different slides in the slideshow.'),
    '#attributes' => array(
      'class' => 'description',
    ),
  );
  $output .= drupal_render($table_description);
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
        // Edit slide link
        drupal_render($form['slides_table'][$slide_id]['edit']),
      ),
      // Add the 'draggable' class to the tr-element
      'class' => array('draggable'),
    );
  }
  // Define the table-header values
  $header = array(t('Type'), t('Datawell-PID'), t('Search query'), t('Position'), t('Edit'));

  // Id for out table (needed later)
  $table_id = 'ting-visual-relation-slides-table';

  // Theme the table output
  $output .= theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => $table_id)
  ));

  // Render remaining form-elements
  $output .= drupal_render_children($form);

  // Add the tabledrag.js magic
  drupal_add_tabledrag($table_id, 'order', 'sibling', 'ting-visual-relation-slides-table-position');

  return $output;
}

/**
 * Submit hanlder for ting_visual_relation_app_settings_form.
 */
function ting_visual_relation_app_settings_form_submit($form, &$form_state) {
  // Update each slides position in the slideshow
  foreach ($form_state['values']['slides_table'] as $slide_id => $slide) {
    db_update('ting_visual_relation_slides')
      ->condition('slide_id', $slide_id)
      ->fields(array('position' => $slide['position']))
      ->execute();
  }
  variable_set('ting_visual_relation_slideshow_interval', $form_state['values']['interval']);
  drupal_set_message(t('Your settings have been saved'));
}

/**
 * Form for adding a slide to the Visual relation app.
 */
function ting_visual_relation_slide_form($form, &$form_state, $slide_id = NULL) {
  $slide = FALSE;
  // If this is the edit form load the slide settings from the database.
  if (isset($slide_id)) {
    $slide = db_select('ting_visual_relation_slides', 'tvrs')
      ->fields('tvrs')
      ->condition('tvrs.slide_id', $slide_id)
      ->execute()
      ->fetchObject();
    // Save the slide for later use
    $form_state['slide'] = $slide;
  }
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
    '#default_value' => $slide ? $slide->type : 'external',
  );
  // Datawell PID - Only visable when type external or circular is selected.
  $form['add_slide']['datawell_pid'] = array(
    '#type' => 'textfield',
    '#title' => t('Datawell PID'),
    '#description' => t('Enter the datawell PID the browser should be based on'),
    '#default_value' => $slide ? $slide->datawell_pid : '',
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
    '#default_value' => $slide ? $slide->search_query : '',
    '#states' => array(
      'visible' => array(
        ':input[name="type"]' => array('value' => 'structural'),
      ),
    ),
  );
  // Form actions
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => $slide ? t('Update') : t('Save'),
  );
  // Should only be included if user is editing an existing slide.
  if ($slide) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('ting_visual_relation_slide_form_delete'),
    );
  }
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('ting_visual_relation_slide_form_cancel'),
    // Bypass form-validation since this is a cancel-button
    '#limit_validation_errors' => array(),
    '#weight' => 40,
  );
  return $form;
}

/**
 * Validation handler for ting_visual_relation_add_slide_form.
 */
function ting_visual_relation_slide_form_validate($form, &$form_state) {
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
function ting_visual_relation_slide_form_submit($form, &$form_state) {
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
  // Determine if this is an update or insert
  $slide = isset($form_state['slide']) ? $form_state['slide'] : FALSE;
  if ($slide) {
    db_update('ting_visual_relation_slides')
      ->condition('slide_id', $slide->slide_id)
      ->fields($fields)
      ->execute();
  }
  else {
    db_insert('ting_visual_relation_slides')->fields($fields)->execute();
  }
  // Set message and go back to table overview
  drupal_set_message(t('Slide saved'));
  $form_state['redirect'] = 'admin/config/ting/ting-visual-relation/app-settings';
}

/**
 * Submit handler for the delete button on ting_visual_relation_slide_form.
 */
function ting_visual_relation_slide_form_delete($form, &$form_state) {
  // Pass along any destination parameter set from previous pages
  $destination = array();
  if (isset($_GET['destination'])) {
    $destination = drupal_get_destination();
    unset($_GET['destination']);
  }
  // Should allways be set since this is only available on the edit form.
  $slide_id = $form_state['slide']->slide_id;
  $path = 'admin/config/ting/ting-visual-relation/app-settings/' . $slide_id .'/delete';
  $form_state['redirect'] = array($path, array('query' => $destination));
}

/**
 * Submit handler for the cancel button on ting_visual_relation_add_slide_form.
 */
function ting_visual_relation_slide_form_cancel($form, &$form_state) {
  // Go back to app-settings (or destination if parameter is set)
  $form_state['redirect'] = 'admin/config/ting/ting-visual-relation/app-settings';
}

/**
 * Form builder for slide delete confirm form.
 */
function ting_visual_relation_delete_slide_confirm_form($form, &$form_state, $slide_id = NULL) {
  // Save the slide for the submit handler.
  $form['slide_id'] = array('#type' => 'value', '#value' => $slide_id);
  return confirm_form($form,
    t('Are you sure you want to delete the relation browser slide?'),
    'admin/config/ting/ting-visual-relation/app-settings/' . $slide_id .'/edit',
    t('This action cannot be undone.'),
    t('delete'),
    t('cancel')
  );
}

/**
 * Submit handler for the slide delete confirm form.
 */
function ting_visual_relation_delete_slide_confirm_form_submit($form, &$form_state) {
  // Perform the deletion
  $slide_id = $form_state['values']['slide_id'];
  $success = db_delete('ting_visual_relation_slides')
    ->condition('slide_id', $slide_id)
    ->execute();
  drupal_set_message('Slide deleted');
  $form_state['redirect'] = 'admin/config/ting/ting-visual-relation/app-settings';
}
