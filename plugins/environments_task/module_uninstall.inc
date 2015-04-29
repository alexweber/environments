<?php

/**
 * @file
 * Contains the "module_uninstall" Environments Task.
 */

$plugin = array(
  'admin_title' => t('Uninstall module(s)'),
  'task_callback' => 'environments_task_module_uninstall',
  'settings' => array(
    'modules' => array(),
    'uninstall_dependents' => TRUE,
  ),
  'settings_callback' => 'environments_task_module_uninstall_settings',
  'summary_callback' => 'environments_task_module_uninstall_summary',
);

/**
 * Form callback; returns the settings form for this task.
 *
 * @param array $form_state
 *   A copy of the form_state array.
 *
 * @return array
 *   A Form API array of task settings.
 */
function environments_task_module_uninstall_settings(array $form_state) {
  $task = isset($form_state['task']) ? $form_state['task'] : NULL;
  $form = _environments_module_task_settings_form($task, $form_state);

  $form['uninstall_dependents'] = array(
    '#name' => 'settings[uninstall_dependents]',
    '#type' => 'checkbox',
    '#title' => t('Uninstall dependents'),
    '#description' => t('If TRUE, dependent modules will automatically be uninstalled in the correct order.'),
    '#default_value' => isset($task['settings']['uninstall_dependents']) ? $task['settings']['uninstall_dependents'] : TRUE,
  );

  $form['uninstall_enabled'] = array(
    '#name' => 'settings[uninstall_enabled]',
    '#type' => 'checkbox',
    '#title' => t('Disable enabled modules'),
    '#description' => t('If TRUE, enabled modules will automatically be disabled before being uninstalled. If FALSE, attempting to uninstall an enabled module will fail.'),
    '#default_value' => isset($task['settings']['uninstall_enabled']) ? $task['settings']['uninstall_enabled'] : TRUE,
  );

  return $form;
}

/**
 * Summary callback; format a custom summary of our task settings.
 *
 * @param array $settings
 *   An array of settings.
 *
 * @return array
 *   An array containing a short summary for each setting.
 */
function environments_task_module_uninstall_summary(array $settings) {
  $summary = array();

  $module_summary = _environments_module_task_summary($settings);
  $summary[] = '<strong>' . t('Modules') . ':</strong> ' . $module_summary;
  $summary[] = '<strong>' . t('Uninstall dependents') . ':</strong> <em>' . ($settings['uninstall_dependents'] ? t('Yes') : t('No')) . '</em>';
  $summary[] = '<strong>' . t('Disable enabled modules') . ':</strong> <em>' . ($settings['uninstall_enabled'] ? t('Yes') : t('No')) . '</em>';

  return $summary;
}

/**
 * Task callback.
 *
 * @param array $settings
 *   An array of settings for the task.
 * @param array $context
 *   An array that will contain information about the status of the batch.
 *   The values in $context will retain their values as the batch progresses.
 */
function environments_task_module_uninstall(array $settings, array &$context) {
  $modules = $settings['modules'];
  $uninstall_dependents = $settings['uninstall_dependents'];
  $uninstall_enabled = $settings['uninstall_enabled'];

  $messages = array();
  $modules_disable = array();

  // Check whether we're supposed to disable enabled modules ourselves so that
  // they can be uninstalled properly.
  if ($uninstall_enabled) {
    foreach ($modules as $module) {
      if (module_exists($module)) {
        $modules_disable[] = $module;
      }
    }

    // Disable enabled modules first.
    if (!empty($modules_disable)) {
      module_disable($modules_disable, $uninstall_dependents);
      $messages[] = t('The following modules were manually disabled before being uninstalled: @modules.', array('@modules' => implode(', ', array_values($modules_disable))));
    }
  }

  // Finally, uninstall modules.
  if (drupal_uninstall_modules($modules, $uninstall_dependents)) {
    $messages[] = t('The folowing modules were uninstalled: @modules.', array('@modules' => implode(', ', array_values($modules))));
  }

  // Display messages.
  $context['message'] = implode(' ', $messages);
}
