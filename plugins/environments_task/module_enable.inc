<?php

/**
 * @file
 * Contains the "module_enable" Environments Task.
 */

$plugin = array(
  'admin_title' => t('Enable module(s)'),
  'task_callback' => 'environments_task_module_enable',
  'settings' => array(
    'modules' => array(),
    'enable_dependencies' => TRUE,
  ),
  'settings_callback' => 'environments_task_module_enable_settings',
  'summary_callback' => 'environments_task_module_enable_summary',
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
function environments_task_module_enable_settings(array $form_state) {
  $task = isset($form_state['task']) ? $form_state['task'] : NULL;
  $form = _environments_module_task_settings_form($task, $form_state);

  $form['enable_dependencies'] = array(
    '#name' => 'settings[enable_dependencies]',
    '#type' => 'checkbox',
    '#title' => t('Enable dependencies'),
    '#description' => t('If TRUE, dependency modules will automatically be enabled in the correct order.'),
    '#default_value' => isset($task['settings']['enable_dependencies']) ? $task['settings']['enable_dependencies'] : TRUE,
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
function environments_task_module_enable_summary(array $settings) {
  $summary = array();

  $module_summary = _environments_module_task_summary($settings);
  $summary[] = '<strong>' . t('Modules') . ':</strong> ' . $module_summary;
  $summary[] = '<strong>' . t('Enable dependencies') . ':</strong> <em>' . ($settings['enable_dependencies'] ? t('Yes') : t('No')) . '</em>';

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
function environments_task_module_enable(array $settings, array &$context) {
  $modules = $settings['modules'];
  $enable_dependencies = $settings['enable_dependencies'];

  // Filter already-enabled modules.
  $messages = array();
  $modules_filtered = array();

  foreach ($modules as $module) {
    if (module_exists($module)) {
      $messages[] = t('@module is already enabled.', array('@module' => $module));
    }
    else {
      $modules_filtered[] = $module;
    }
  }

  // Enable modules.
  if (!empty($modules_filtered)) {
    if (module_enable($modules_filtered, $enable_dependencies)) {
      // Display this message before all others.
      array_unshift($messages, t('Enabled modules: @modules.', array('@modules' => implode(', ', array_values($modules_filtered)))));
    }
  }

  // Display messages.
  $context['message'] = implode("\n", $messages);
}
