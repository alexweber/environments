<?php

/**
 * @file
 * Contains the "execute_callback" Environments Task.
 */

$plugin = array(
	'admin_title' => t('Execute callback'),
	'task_callback' => 'environments_task_execute_callback',
	'settings' => array(
		'callback' => '',
	),
	'settings_callback' => 'environments_task_execute_callback_settings',
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
function environments_task_execute_callback_settings($form_state) {
	$task = isset($form_state['task']) ? $form_state['task'] : NULL;
	$form = array();

	$form['callback'] = array(
		'#name' => 'settings[callback]',
		'#type' => 'textfield',
		'#title' => t('Callback'),
		'#required' => TRUE,
		'#description' => t('Insert a function name without parentheses.'),
		'#default_value' => isset($task['settings']['callback']) ? $task['settings']['callback'] : '',
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
function environments_task_execute_callback($settings, &$context) {
	$callback = $settings['callback'];

	if (function_exists($callback)) {
		call_user_func($callback);
		$context['message'] = t('Executed callback: @name()', array('@name' => $callback));
	}
	else {
		$message = 'Error executing callback: @name()';
		$variables = array('@name' => $callback);
		watchdog('environments', $message, $variables, WATCHDOG_ERROR);
		$context['message'] = t($callback, $variables);
	}
}
