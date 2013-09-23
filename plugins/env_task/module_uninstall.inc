<?php
/**
 * @file
 * Definition of the "module_uninstall" Env Task.
 */

$plugin = array(
  'admin_title' => t('Uninstall modules'),
  'task_callback' => 'env_task_module_uninstall',
  'settings' => array(
    'modules' => array(),
    'uninstall_dependents' => TRUE,
  ),
  'settings_callback' => 'env_task_module_uninstall_settings',
);

/**
 * Form callback.
 *
 * Returns the settings form for this task.
 *
 * This is not a typical form callback in that we don't get a $form or
 * $form_state.
 *
 * @param stdClass
 *   The current environment.
 * @param array
 *   The current task.
 *
 * @return array
 *   A Form API array of task settings.
 */
function env_task_module_uninstall_settings($environment, $task = NULL) {
  $form = array();

  $form['modules'] = array(
    '#name' => 'settings[modules]',
    '#type' => 'select',
    '#title' => t('Modules'),
    '#multiple' => TRUE,
    '#options' => _env_get_modules(),
    '#empty_option' => t(' -Select some modules- '),
    '#default_value' => isset($task['settings']['modules']) ? $task['settings']['modules'] : array(),
  );

  $form['uninstall_dependents'] = array(
    '#name' => 'settings[uninstall_dependents]',
    '#type' => 'checkbox',
    '#title' => t('Uninstall dependents'),
    '#description' => t('If TRUE, dependent modules will automatically be uninstalled in the correct order.'),
    '#default_value' => isset($task['settings']['uninstall_dependents']) ? $task['settings']['uninstall_dependents'] : TRUE,
  );

  return $form;
}

/**
 * Task callback.
 *
 * @param array
 *   An array of settings for the task.
 * @param array
 *   $context is an array that will contain information about the
 *   status of the batch. The values in $context will retain their
 *   values as the batch progresses.
 */
function env_task_module_uninstall($settings, &$context) {
  $modules = $settings['modules'];
  $uninstall_dependents = $settings['uninstall_dependents'];

  $messages = array();
  $modules_disable = array();

  // drupal_uninstall_modules() requires modules to be disabled before being
  // uninstalled, so find modules we need to manually disable.
  foreach ($modules as $module) {
    if (module_exists($module)) {
      $modules_disable[] = $module;
    }
  }

  // First, disable modules.
  if (!empty($modules_disable)) {
    module_disable($modules_disable, $uninstall_dependents);
    $messages[] = t('The following modules were manually disabled before being uninstalled: %module.', array('%module' => implode(', ', array_values($modules_disable))));
  }

  // Finally, uninstall modules.
  if (drupal_uninstall_modules($modules, $uninstall_dependents)) {
    // Display this message before all others.
    $messages[] = t('Uninstalled modules: %modules.', array('%module' => implode(', ', array_values($modules))));
  }

  // Display messages.
  $context['message'] = implode(' ', $messages);
}
