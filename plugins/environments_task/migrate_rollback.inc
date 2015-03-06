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
	),
	'settings_callback' => 'environments_task_migrate_rollback_settings',
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
function environments_task_migrate_rollback_settings($form_state) {
	$form = array();
	$task = isset($form_state['task']) ? $form_state['task'] : NULL;

	$form['migrations'] = array(
		'#name' => 'settings[migrations]',
		'#type' => 'select',
		'#title' => t('Migrations'),
		'#description' => t('Caution: This task will fail if the Migrate module is not enabled.'),
		'#multiple' => TRUE,
		'#options' => _environments_get_migrations(),
		'#empty_option' => t(' -Select some migrations- '),
		'#default_value' => isset($task['settings']['migrations']) ? $task['settings']['migrations'] : array(),
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
 *
 * @throws Exception
 *   If the Migrate module is not enabled.
 */
function environments_task_migrate_rollback($settings, &$context) {
	$migrations = $settings['migrations'];
	$messages = array();

	if (!module_exists('migrate')) {
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
