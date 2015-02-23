<?php

/**
 * @file
 * Contains the "variable_del" Env Task.
 */

$plugin = array(
	'admin_title' => t('Delete variables'),
	'task_callback' => 'env_task_variable_del',
	'settings' => array(
		'variables' => array(),
	),
	'settings_callback' => 'env_task_variable_del_settings',
);

/**
 * Form callback.
 *
 * Returns the settings form for this task.
 *
 * @param stdClass
 *   The current environment.
 * @param array
 *   The current task.
 *
 * @return array
 *   A Form API array of task settings.
 */
function env_task_variable_del_settings($environment, $task = NULL) {
	$form = array();

	$form['variables'] = array(
		'#name' => 'settings[variables]',
		'#type' => 'select',
		'#title' => t('Variables'),
		'#multiple' => TRUE,
		'#options' => drupal_map_assoc(_env_get_variables()),
		'#empty_option' => t(' -Select some variables- '),
		'#default_value' => isset($task['settings']['variables']) ? $task['settings']['variables'] : array(),
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
function env_task_variable_del($settings, &$context) {
	$variables = $settings['variables'];

	foreach ($variables as $variable) {
		variable_del($variable);
	}

	$context['message'] = t('Deleted variables: @variable.', array('@variable' => implode(', ', $variables)));
}
