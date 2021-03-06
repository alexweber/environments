<?php

/**
 * @file
 * Contains the "execute_drush" Environments Task.
 */

$plugin = array(
  'admin_title' => t('Execute drush command'),
  'task_callback' => 'environments_task_execute_drush',
  'settings' => array(
    'command' => '',
    'verbose' => TRUE,
  ),
  'settings_callback' => 'environments_task_execute_drush_settings',
  'summary_callback' => 'environments_task_execute_drush_summary',
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
function environments_task_execute_drush_settings(array $form_state) {
  $form = array();
  $task = isset($form_state['task']) ? $form_state['task'] : NULL;

  $form['command'] = array(
    '#name' => 'settings[command]',
    '#type' => 'textfield',
    '#title' => t('Command'),
    '#description' => t('Insert the command you wish to run, leave out "drush". The command will be executed as-is. For example, to execute "drush status", enter only "status".'),
    '#required' => TRUE,
    '#default_value' => isset($task['settings']['command']) ? $task['settings']['command'] : '',
    '#attributes' => array(
      'placeholder' => 'vset my_test_var foo',
    ),
  );

  $form['verbose'] = array(
    '#name' => 'settings[verbose]',
    '#type' => 'checkbox',
    '#title' => t('Verbose'),
    '#description' => t("If checked, the command's output will be printed to stdout. Otherwise, it will be ignored."),
    '#default_value' => isset($task['settings']['verbose']) ? $task['settings']['verbose'] : TRUE,
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
function environments_task_execute_drush_summary(array $settings) {
  $summary = array();

  $summary[] = '<strong>' . t('Command') . ':</strong> drush ' . $settings['command'];
  $summary[] = '<strong>' . t('Verbose') . ':</strong> <em>' . ($settings['verbose'] ? t('Yes') : t('No')) . '</em>';

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
function environments_task_execute_drush(array $settings, array &$context) {
  $command = $settings['command'];
  $verbose = $settings['verbose'];

  if ($drush_path = variable_get('environments_drush', '')) {
    $status = NULL;
    $uri = $GLOBALS['base_url'];
    $drush_command = "{$drush_path} {$command} --uri={$uri} --root=" . DRUPAL_ROOT;

    // $log_file = '/tmp/drupal_environments.log';
    // $drush_command_full = "{$drush_command} >{$log_file} 2>&1 &";
    // passthru($drush_command_full, $status);
    exec($drush_command, $output, $status);

    $context['message'] = t('Executed command: "!name".', array('!name' => $drush_command));

    if ($verbose) {
      $output_str = var_export($output, TRUE);
      $context['message'] = t('Command status: "!status".', array('!status' => $status));
      $context['message'] = t('Command output: "!output".', array('!output' => $output_str));
    }
  }
  else {
    $message = 'Drush is misconfigured.';
    watchdog('environments', $message, array(), WATCHDOG_ERROR);
    $context['message'] = t($message, array());
  }
}
