<?php

/**
 * @file
 * Export interface plugin for Env.
 */

$env_path = drupal_get_path('module', 'env');

/**
 * Define this Export UI plugin.
 */
$plugin = array(
  'schema' => 'env_environment', // As defined in hook_schema().
  'access' => 'administer env', // Define a permission users must have to access these pages.

  // Define the menu item.
  'menu' => array(
    'menu prefix' => 'admin/config/development',
    'menu item' => 'env',
    'menu title' => 'Environments',
    'menu description' => 'Administer Environments.',

    // Add extra menu items.
    'items' => array(
      // Manage Tasks form.
      'tasks' => array(
        'path' => 'list/%ctools_export_ui/tasks',
        'title' => 'Manage Tasks',
        'page callback' => 'drupal_get_form',
        'page arguments' => array('env_form_tasks', 5),
        'access arguments' => array('administer env'),
        'file' => 'env.admin.inc',
        'file path' => $env_path,
        'type' => MENU_LOCAL_TASK,
        'weight' => -5,
      ),
      // Add new task form.
      'task add' => array(
        'path' => 'list/%ctools_export_ui/tasks/add',
        'title' => 'Add Task',
        'page callback' => 'drupal_get_form',
        'page arguments' => array('env_form_tasks_edit', 5),
        'access arguments' => array('administer env'),
        'file' => 'env.admin.inc',
        'file path' => $env_path,
        'type' => MENU_LOCAL_ACTION,
      ),
      // Edit task form.
      'task edit' => array(
        'path' => 'list/%ctools_export_ui/tasks/%/edit',
        'title' => 'Edit Task',
        'page callback' => 'drupal_get_form',
        'page arguments' => array('env_form_tasks_edit', 5, 7),
        'access arguments' => array('administer env'),
        'file' => 'env.admin.inc',
        'file path' => $env_path,
//        'type' => MENU_CALLBACK,
      ),
      // Delete task form.
      'task delete' => array(
        'path' => 'list/%ctools_export_ui/tasks/%/delete',
        'title' => 'Delete Task',
        'page callback' => 'drupal_get_form',
        'page arguments' => array('env_form_tasks_delete', 5, 7),
        'access arguments' => array('administer env'),
        'file' => 'env.admin.inc',
        'file path' => $env_path,
//        'type' => MENU_CALLBACK,
      ),
    ),
  ),

  // Define user interface texts.
  'title singular' => t('Environment'),
  'title plural' => t('Environments'),
  'title singular proper' => t('Environment'),
  'title plural proper' => t('Environments'),

  // Define the names of the functions that provide the add/edit forms.
  'form' => array(
    'settings' => 'env_ctools_export_ui_form',
    'validate' => 'env_ctools_export_ui_form_validate',
    'submit' => 'env_ctools_export_ui_form_submit',
  ),

  // Override the UI handler.
  'handler' => array(
    'class' => 'env_ctools_export_ui_handler',
    'parent' => 'ctools_export_ui',
  ),

  // Add an extra operation.
  'allowed operations' => array(
    'tasks'  => array('title' => t('Manage Tasks')),
  ),

);

/**
 * Export UI form
 */
function env_ctools_export_ui_form(&$form, &$form_state) {
  // Load the admin form include.
  module_load_include('inc', 'env', 'env.admin');

  // Make environment reference in form_state.
  $form_state['environment'] = &$form_state['item'];

  // Load the configuration form.
  $form = drupal_retrieve_form('env_form_environment_edit', $form_state);
}

/**
 * Validation handler.
 *
 * @see env_form_environment_edit()
 */
function env_ctools_export_ui_form_validate(&$form, &$form_state) {
  $alias = $form_state['values']['alias'];

  if (!empty($alias)) {
    // First, check if alias is a valid machine name.
    if (!ctype_alpha($alias)) {
      form_set_error('alias', t('Alias must contain only letters.'));
    }
    else {
      // Then, check if alias is unique.
      $params = array(
        ':alias' => $form_state['values']['alias'],
        ':name' => $form_state['values']['name'],
      );

      $result = db_query("SELECT COUNT(*) FROM {env_environment} WHERE alias = :alias AND name <> :name", $params)
        ->fetchField();

      if ($result != 0) {
        form_set_error('alias', t('This alias is already in use.'));
      }
    }
  }
}

/**
 * Submit handler.
 *
 * @see env_form_environment_edit()
 */
function env_ctools_export_ui_form_submit(&$form, &$form_state) {
  // Edit the reference to $form_state['environment'] which will in turn
  // reference $form_state['item'] which is what CTools is looking for.
  $environment = &$form_state['environment'];
  $environment->admin_title = $form_state['values']['admin_title'];
  $environment->alias = $form_state['values']['alias'];
  $environment->settings = $form_state['values']['settings'];
}
