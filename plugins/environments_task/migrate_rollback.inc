<?php

/**
 * @file
 * Contains the "migrate_rollback" Environments Task.
 */

$plugin = array(
  'admin_title' => t('Roll-back migration(s)'),
  'task_callback' => 'environments_task_migrate_rollback',
  'settings' => array(
    'migrations' => array(),
    'enable' => TRUE,
  ),
  'settings_callback' => 'environments_task_migrate_rollback_settings',
  'summary_callback' => 'environments_task_migrate_rollback_summary',
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
function environments_task_migrate_rollback_settings(array $form_state) {
  $form = array();
  $task = isset($form_state['task']) ? $form_state['task'] : NULL;

  $form['migrations'] = array(
    '#name' => 'settings[migrations]',
    '#type' => 'select',
    '#title' => t('Migrations'),
    '#description' => t('Caution: This task will fail if the Migrate module is not enabled.'),
    '#multiple' => TRUE,
    '#options' => _environments_get_migrations(),
    '#empty_option' => ' -' . t('Select migrations') . '- ',
    '#default_value' => isset($task['settings']['migrations']) ? $task['settings']['migrations'] : array(),
  );

  $form['enable'] = array(
    '#name' => 'settings[enable]',
    '#type' => 'checkbox',
    '#title' => t('Enable Migrate'),
    '#description' => t("If checked, the Migrate module will be explicitly enabled. Otherwise, it\'s up to you to make sure it's enabled when this task runs! This task will fail if Migrate is not enabled."),
    '#default_value' => isset($task['settings']['enable']) ? $task['settings']['enable'] : TRUE,
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
function environments_task_migrate_rollback_summary(array $settings) {
  $summary = array();

  $summary[] = '<strong>' . t('Migrations') . ':</strong> ' . implode(', ', $settings['migrations']);
  $summary[] = '<strong>' . t('Enable Migrate') . ':</strong> ' . ($settings['enable'] ? t('Yes') : t('No'));

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
 *
 * @throws Exception
 *   If the Migrate module is not enabled.
 */
function environments_task_migrate_rollback(array $settings, array &$context) {
  $migrations = $settings['migrations'];
  $enable = $settings['enable'];
  $messages = array();

  if ($enable && !module_exists('migrate')) {
    module_enable(array('migrate'));
  }

  // Keep it simple.
  if (!class_exists('Migration')) {
    throw new Exception(t('Migrate module must be enabled to execute the "migrate_rollback" Environments task.'));
  }

  foreach ($migrations as $machine_name) {
    $migration = Migration::getInstance($machine_name);
    $migration->processRollback();
    $messages[] = t('Migration rolled-back: @name', array('@name' => $machine_name));
  }

  // Display messages.
  $context['message'] = implode("\n", $messages);
}
