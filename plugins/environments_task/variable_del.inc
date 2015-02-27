<?php

/**
 * @file
 * Contains the "variable_del" Environments Task.
 */

$plugin = array(
	'admin_title' => t('Delete variable(s)'),
	'task_callback' => 'environments_task_variable_del',
	'settings' => array(
		'variables' => array(),
	),
	'settings_callback' => 'environments_task_variable_del_settings',
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
function environments_task_variable_del_settings($form_state) {
	$form = array();
	$task = isset($form_state['task']) ? $form_state['task'] : NULL;

	$form['variables'] = array(
		'#name' => 'settings[variables]',
		'#type' => 'select',
		'#title' => t('Variables'),
		'#multiple' => TRUE,
		'#options' => drupal_map_assoc(_environments_get_variables()),
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
function environments_task_variable_del($settings, &$context) {
	$variables = $settings['variables'];

	foreach ($variables as $variable) {
		variable_del($variable);
	}

	$context['message'] = t('Deleted variables: @variable.', array('@variable' => implode(', ', $variables)));
}
