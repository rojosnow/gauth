<?php
/**
 * @file
 * Administration pages for GAuth Login settings.
 */

/**
 * Administrative settings.
 *
 * @return
 *   An array containing form items to place on the module settings page.
 */
function gauth_login_settings() {
  $form['gauth_login_description'] = array(
    '#theme' => 'gauth_google_developers_console_help',
  );
  $form['gauth_login_client_id'] = array(
    '#type'  => 'textfield',
    '#title' => t('Client Id'),
    '#size' => 90,
    '#required' => TRUE,
    '#description' => t('The CLIENT ID in the "Client ID for web application" section.'),
    '#default_value' => variable_get('gauth_login_client_id', ''),
  );
  $form['gauth_login_developer_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Email Address'),
    '#size' => 90,
    '#required' => TRUE,
    '#description' => t('The EMAIL ADDRESS in the "Client ID for web
      application" section.'),
    '#default_value' => variable_get('gauth_login_developer_key', ''),
  );
  $form['gauth_login_client_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Client Secret'),
    '#size' => 40,
    '#required' => TRUE,
    '#description' => t('The CLIENT SECRET in the "Client ID for web
      application" section.'),
    '#default_value' => variable_get('gauth_login_client_secret', ''),
  );

  return system_settings_form($form);
}
