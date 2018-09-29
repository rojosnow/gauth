<?php
/**
 * @file
 * Administration pages for Google Auth Account Services.
 */

/**
 * Menu callback; Listing of all current service accounts.
 */
function gauth_service_account_list() {

  $query = db_select('gauth_service_accounts', 'gsa')->extend('PagerDefault');
  $query->fields('gsa');

  $accounts = $query
    ->limit(25)
    ->execute();

  return theme('gauth_service_account_listing', array('accounts' => $accounts));
}

/**
 * Returns HTML for the page containing the list of Service Accounts.
 *
 * @param array $variables
 *   An associative array containing:
 *   - accounts: An array of all the accounts returned by
 *               gauth_user_services_load().
 *
 * @see gauth_user_services_load()
 * @ingroup themeable
 */
function theme_gauth_service_account_listing($variables) {
  $accounts = $variables['accounts'];
  $header = array(
    t('Name'),
    t('Client Id'),
    t('Email Address'),
    t('Key File Location'),
    t('Services'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $rows = array();
  foreach ($accounts as $account) {
    $row = array();
    $row[] = $account->name;
    $row[] = $account->client_id;
    $row[] = $account->developer_key;
    $row[] = $account->key_file;
    $row[] = implode(", ", gauth_google_services_names($account->services));
    $row[] = l(t('edit'), 'admin/config/services/gauth_service_account/edit/' . $account->id);
    $row[] = l(t('delete'), 'admin/config/services/gauth_service_account/delete/' . $account->id);
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'colspan' => 6,
        'data' => t('There are currently no Service Accounts. <a href="!url">Add a new one</a>.', array('!url' => url('admin/config/services/gauth_service_account/add'))),
      ),
    );
  }

  $build['google_descr'] = array(
    '#theme' => 'gauth_google_developers_console_help',
  );

  $build['google_descr_more'] = array(
    '#prefix' => '<p>',
    '#markup' => t('Make sure you follow the configuration information for <a href="@url" target="_blank">Google Service Accounts</a>',
        array('@url' => url('https://developers.google.com/accounts/docs/OAuth2ServiceAccount'))) . '.',
    '#suffix' => '</p>',
  );

  $build['pager_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  $build['pager_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Form builder; CRUD Service Account.
 *
 * @param array $form_state
 *   An associative array containing the current state of the form.
 * @param int $id
 *   An id of the account.
 *
 * @ingroup forms
 * @see gauth_service_account_edit_form_submit()
 * @see gauth_service_account_edit_form_validate()
 */
function gauth_service_account_edit_form($form, &$form_state, $id = NULL) {
  $account = array();
  if ($id) {
    $account = gauth_service_account_load($id, FALSE);
    $form['is_new'] = array(
      '#type' => 'value',
      '#value' => FALSE,
    );
  }
  else {
    $form['is_new'] = array(
      '#type' => 'value',
      '#value' => TRUE,
    );
  }
  $form['description'] = array(
    '#theme' => 'gauth_google_developers_console_help',
  );
  $form['description_more'] = array(
    '#prefix' => '<p>',
    '#markup' => t('Make sure you follow the configuration information for <a href="@url" target="_blank">Google Service Accounts</a>',
        array('@url' => url('https://developers.google.com/accounts/docs/OAuth2ServiceAccount'))) . '.',
    '#suffix' => '</p>',
  );
  $form['id'] = array(
    '#type' => 'hidden',
    '#default_value' => isset($account['id']) ? $account['id'] : '',
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Name'),
    '#description' => t('The unique name for this Service Account.'),
    '#default_value' => isset($account['name']) ? $account['name'] : '',
    '#required' => TRUE,
  );
  $form['client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Client Id'),
    '#description' => t('The CLIENT ID in the "Service Account" section.'),
    '#size' => 100,
    '#default_value' => isset($account['client_id']) ? $account['client_id'] : '',
    '#required' => TRUE,
  );
  $form['developer_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Email Address'),
    '#description' => t('The EMAIL ADDRESS in the "Service Account" section.'),
    '#size' => 100,
    '#default_value' => isset($account['developer_key']) ? $account['developer_key'] : '',
    '#required' => TRUE,
  );
  $form['key_file'] = array(
    '#type' => 'textfield',
    '#title' => t('Key File Location'),
    '#description' => t('Enter the path and filename of the P12 key file. Do
      not enter a leading "/" in the location. Please note, this location will be
      prefixed with "<strong>private://</strong>" referring to private files
      only. Due to security concerns, public files are not supported.'),
    '#default_value' => isset($account['key_file']) ? $account['key_file'] : '',
    '#required' => TRUE,
  );
  $options = gauth_google_services_names();
  $form['services'] = array(
    '#type' => 'select',
    '#title' => t('Services'),
    '#description' => t('Services that will be accessed by this Service Account. Each service refers to one or more scopes.'),
    '#options' => $options,
    '#multiple' => TRUE,
    '#default_value' => isset($account['services']) ? explode(",", $account['services']) : array(),
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#suffix' => l(t('Cancel'), 'admin/config/services/gauth_service_account'),
  );
  return $form;
}

/**
 * Validate handler for adding a new account to Google Auth Service Accounts.
 */
function gauth_service_account_edit_form_validate($form, &$form_state) {
  $accounts = gauth_service_account_load();
  $accounts = array_keys($accounts);
  if (in_array($form_state['values']['name'], $accounts)) {
    if (!isset($form_state['values']['is_new'])) {
      form_set_error('name', t('Name already in use. Please choose a unique name for the Service Account.'));
    }
  }
}

/**
 * Submit handler for adding a new account to Google Auth Service Accounts.
 */
function gauth_service_account_edit_form_submit($form, &$form_state) {
  $id = gauth_service_account_save($form_state['values']);
  drupal_set_message(t('Service Account saved. Click !url to configure permissions for this account', array('!url' => l(t('here'), 'admin/people/permissions', array('fragment' => 'module-gauth_service_account')))));
  $form_state['redirect'] = 'admin/config/services/gauth_service_account';
}

/**
 * Form builder; Form for deleting a Google Auth Service Account.
 *
 * @param int $id
 *   Id of the account to be deleted.
 *
 * @ingroup forms
 * @see gauth_service_account_delete_form_submit()
 */
function gauth_service_account_delete_form($form, &$form_state, $id) {
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  $question = check_plain(t('Are you sure you want to delete this Service Account'));
  $path = 'admin/config/services/gauth_service_account';
  $description = check_plain(t('This Service Account will be deleted from the system.'));
  $yes = check_plain(t('Delete'));
  $no = check_plain(t('Cancel'));
  return confirm_form($form, $question, $path, $description, $yes, $no);
}

/**
 * Submit handler to delete a Google Auth Service Account.
 */
function gauth_service_account_delete_form_submit($form, &$form_state) {
  if ($form_state['values']['id'] != NULL) {
    $rows = gauth_service_account_delete($form_state['values']['id'], FALSE);
    if ($rows == 1) {
      drupal_set_message(t('The Service Account was deleted successfully.'));
    }
    else {
      drupal_set_message(t('Error occurred while deleting the Service Account.'), 'error');
    }
  }
  else {
    drupal_set_message(t('Error occurred: Can\'t find the Service Account to delete.'), 'error');
  }
  $form_state['redirect'] = 'admin/config/services/gauth_service_account';
}