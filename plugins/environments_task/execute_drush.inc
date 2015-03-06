<?php

/**
 * @file
 * Contains the "execute_drush" Environments Task.
 */

$plugin = array(
	'admin_title' => t('Execute Drush command'),
	'task_callback' => 'environments_task_execute_drush',
	'settings' => array(
		'command' => '',
	),
	'settings_callback' => 'environments_task_execute_drush_settings',
//	'summary_callback' => 'environments_task_execute_drush_summary',
);

/**
 * Form callback.
 *
 * Returns the settings form for this task.
 *
 * @param array
 *   A copy of the form_state array.
 *
 * @return array
 *   A Form API array of task settings.
 */
function environments_task_execute_drush_settings($form_state) {
	$form = array();
	$task = isset($form_state['task']) ? $form_state['task'] : NULL;

	$form['command'] = array(
		'#name' => 'settings[command]',
		'#type' => 'textfield',
		'#title' => t('Command'),
		'#description' => t('Insert the command you wish to run, leave out "drush". The command will be executed as-is.'),
		'#required' => TRUE,
		'#default_value' => isset($task['settings']['command']) ? $task['settings']['command'] : '',
		'#attributes' => array(
			'placeholder' => 'vset my_test_var foo',
		),
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
function environments_task_execute_drush($settings, &$context) {
	$command = $settings['command'];

	if ($drush_path = variable_get('environments_drush', '')) {
		$status = NULL;
		$output = array();
		$uri = $GLOBALS['base_url'];
		$uid = $GLOBALS['user']->uid;
		$drush_command = "{$drush_path} {$command} --user={$uid} --uri={$uri} --root=" . DRUPAL_ROOT;

		$log_file = '/tmp/drupal_environments.log';
		$drush_command .= " >$log_file 2>&1 &";

		exec($drush_command, $output, $status);
		$output_str = count($output) ? "\n" . implode("\n", $output) : '(empty)';
		$context['message'] = t("Executed command: \"!name\", status was: !status and output was: \n!output.", array('!name' => $drush_command, '!status' => $status, '!output' => $output_str));
	}
	else {
		$message = 'Drush is misconfigured.';
		$variables = array();
		watchdog('environments', $message, $variables, WATCHDOG_ERROR);
		$context['message'] = t($message, $variables);
	}
}
