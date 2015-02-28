<?php

/**
 * @file
 * Contains the "variable_del" Environments Task.
 */

$plugin = array(
	'admin_title' => t('Delete variable'),
	'task_callback' => 'environments_task_variable_del',
	'settings' => array(
		'name' => array(),
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

	$selection = isset($form_state['values']['settings']['selection'])
		? $form_state['values']['settings']['selection']
		: 'custom';

	$form['selection'] = array(
		'#title' => t('Selection mode'),
		'#description' => t('Selecting from a list is easier, but only variables that have already been explicitly set are available.'),
		'#type' => 'select',
		'#default_value' => $selection,
		'#options' => array(
			'list' => t('Select from a list'),
			'custom' => t('Specify manually'),
		),
		'#ajax' => array(
			'callback' => 'environments_ajax_task_settings_variable_del_name',
			'wrapper' => 'environments-variable-name',
			'method' => 'replace',
			'effect' => 'slide',
		),
	);

	$form['name'] = array(
		'#prefix' => '<div id="environments-variable-name">',
		'#suffix' => '</div>',
		'#name' => 'settings[name]',
		'#title' => t('Variable name'),
		'#required' => TRUE,
		'#element_validate' => array('_environments_task_variable_validate_name'),
	);

	if ($selection === 'list') {
		$form['name']['#type'] = 'select';
		$form['name']['#multiple'] = FALSE;
		$form['name']['#options'] = drupal_map_assoc(_environments_get_variables());
		$form['name']['#empty_option'] = t(' -Select a variable- ');
		$form['name']['#default_value'] = isset($task['settings']['name']) ? (array) $task['settings']['name'] : array();
		$form['name']['#description'] = t('If the variable you\'re looking for isn\'t in this list, try using the "Specify manually" selection mode.');
	}
	else if ($selection === 'custom') {
		$form['name']['#type'] = 'textfield';
		$form['name']['#maxlength'] = 255;
		$form['name']['#size'] = 40;
		$form['name']['#default_value'] = isset($task['settings']['name']) ? (string) $task['settings']['name'] : '';
		$form['name']['#description'] = t('Variable names must start with a letter, consist only of letters, numbers and underscores and be all in lower-case.');
	}

	return $form;
}

/**
 * AJAX Callback.
 *
 * @see environments_task_variable_del_settings()
 */
function environments_ajax_task_settings_variable_del_name($form, &$form_state) {
	return $form['settings']['name'];
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
	$name = $settings['name'];
	variable_del($name);
	$context['message'] = t('Deleted variable: @variable.', array('@variable' => $name));
}
