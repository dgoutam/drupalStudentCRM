<?php

/**
 * @file
 * CRMCase editing UI.
 *
 * We make very little use of the EntityAPI interface for this - preferring instead to use
 * views. That offers more flexibility to change a UI that will, more often than not,
 * be end-user facing.
 */


/**
 * Form callback wrapper: create or edit a crm_case.
 *
 * @param $crm_case
 *   The crm_case object being edited by this form.
 *
 * @see crm_case_edit_form()
 */
function crm_case_form_wrapper($crm_case) {
  // Add the breadcrumb for the form's location.
  crm_case_set_breadcrumb();
  return drupal_get_form('crm_case_edit_form', $crm_case);
}


/**
 * Form callback wrapper: delete a crm_case.
 *
 * @param $crm_case
 *   The crm_case object being edited by this form.
 *
 * @see crm_case_edit_form()
 */
function crm_case_delete_form_wrapper($crm_case) {
  // Add the breadcrumb for the form's location.
  //crm_case_set_breadcrumb();
  return drupal_get_form('crm_case_delete_form', $crm_case);
}


/**
 * Form callback: create or edit a crm_case.
 *
 * @param $crm_case
 *   The crm_case object to edit or for a create form an empty crm_case object
 *     with only a crm_case type defined.
 */
function crm_case_edit_form($form, &$form_state, $crm_case) {
  drupal_set_title(t('Edit @name', array('@name' => $crm_case->name)));
  drupal_add_js(drupal_get_path('module', 'crm_case') . '/js/crm_case.form.js');
  $type = crm_case_type_load($crm_case->type);
  if (isset($type->data['instructions']) && $message = check_markup($type->data['instructions']['value'], $type->data['instructions']['format'])) {
    $form['form_instructions'] = array(
      '#markup' => $message,
    );
  }
  
  // Add the default field elements.
  if (!isset($crm_case->name)) {
    $form['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Case name'),
      '#maxlength' => 255,
      '#required' => TRUE,
      '#weight' => -5,
    );
  }
  else {
    $form['name'] = array(
      '#type' => 'hidden',
      '#value' => $crm_case->name,
    );
  }
  
  $form['data']['#tree'] = TRUE;
  
  // Add the field related form elements.
  $form_state['crm_case'] = $crm_case;
  field_attach_form('crm_case', $crm_case, $form, $form_state);
  foreach (field_info_instances('crm_case', $crm_case->type) as $field_name => $field) {
    if (!user_access("edit any $crm_case->type crm_case") && !$field['crm_case_contact_editable']) {
      $form[$field_name]['#access'] = FALSE;
    }
  }
  if (!user_access('open and close cases')) {
    $form['case_status']['#access'] = FALSE;
  }
  if (arg(4) && !$crm_case->cid) {
    $contact = crm_core_contact_load(arg(4));
    if ($contact) {
      $form['crm_core_contact'] = array(
        '#type' => 'hidden',
        '#value' => $contact->contact_id,
      );
    }
  }
  
  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }
  
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save @type', array('@type' => $type->label)),
    '#submit' => $submit + array('crm_case_edit_form_submit'),
  );
  
  if (!empty($crm_case->name) && crm_case_access('delete', $crm_case)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete case'),
      '#suffix' => l(t('Cancel'), 'crm/case'),
      '#submit' => $submit + array('crm_case_form_submit_delete'),
      '#weight' => 45,
    );
  }

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'crm_case_edit_form_validate';
  return $form;
}

/**
 * Form API validate callback for the crm_case form
 */
function crm_case_edit_form_validate(&$form, &$form_state) {
  $crm_case = $form_state['crm_case'];
  
  // Notify field widgets to validate their data.
  field_attach_form_validate('crm_case', $crm_case, $form, $form_state);
}


/**
 * Form API submit callback for the crm_case form.
 * 
 * @todo remove hard-coded link
 */
function crm_case_edit_form_submit(&$form, &$form_state) {
  
  $crm_case = entity_ui_controller('crm_case')->entityFormSubmitBuildEntity($form, $form_state);
  // Save the crm_case and go back to the list of crm_cases
  
  // Add in created and changed times.
  if ($crm_case->is_new = isset($crm_case->is_new) ? $crm_case->is_new : 0) {
    $crm_case->created = time();
  }

  $crm_case->changed = time();
  
  $crm_case->save();
  
  $form_state['redirect'] = 'crm/case/' . $crm_case->cid;
}

/**
 * Form API submit callback for the delete button.
 * 
 * @todo Remove hard-coded path
 */
function crm_case_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'crm/case/' . $form_state['crm_case']->cid . '/delete';
}

/**
 * Form callback: confirmation form for deleting a crm_case.
 *
 * @param $crm_case
 *   The crm_case to delete
 *
 * @see confirm_form()
 */
function crm_case_delete_form($form, &$form_state, $crm_case) {
  $form_state['crm_case'] = $crm_case;

  $form['#submit'][] = 'crm_case_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete the case %name?', array('%name' => $crm_case->name)),
    'crm/case/crm_case',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );
  
  return $form;
}

/**
 * Submit callback for crm_case_delete_form
 */
function crm_case_delete_form_submit($form, &$form_state) {
  $crm_case = $form_state['crm_case'];

  crm_case_delete($crm_case);

  drupal_set_message(t('The case %name has been deleted.', array('%name' => $crm_case->name)));
  watchdog('crm_case', 'Deleted case %name.', array('%name' => $crm_case->name));

  $form_state['redirect'] = 'crm/case';
}



/**
 * Page to add CRMCase Entities.
 *
 * @todo Pass this through a proper theme function
 */
function crm_case_add_page() {
  $controller = entity_ui_controller('crm_case');
  return $controller->addPage();
}


/**
 * Displays the list of available crm_case types for crm_case creation.
 *
 * @ingroup themeable
 */
function theme_crm_case_add_list($variables) {
  $content = $variables['content'];
  $output = '';
  if ($content) {
    $output = '<dl class="crm_case-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    if (user_access('administer crm_case types')) {
      $output = '<p>' . t('CRMCase Entities cannot be added because you have not created any case types yet. Go to the <a href="@create-crm_case-type">crm_case type creation page</a> to add a new crm_case type.', array('@create-crm_case-type' => url('admin/structure/crm/case-types/add'))) . '</p>';
    }
    else {
      $output = '<p>' . t('No crm_case types have been created yet for you to use.') . '</p>';
    }
  }

  return $output;
}

/**
 * Sets the breadcrumb for administrative crm_case pages.
 */
function crm_case_set_breadcrumb() {
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Content'), 'admin/content'),
    l(t('CRMCases'), 'crm/case'),
  );

  drupal_set_breadcrumb($breadcrumb);
}



