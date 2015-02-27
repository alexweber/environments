<?php

/**
 * @file
 * Contains the "variable_set" Environments Task.
 */

$plugin = array(
	'admin_title' => t('Set variable value'),
	'task_callback' => 'environments_task_variable_set',
	'settings' => array(
		'name' => '',
		'value' => '',
	),
	'settings_callback' => 'environments_task_variable_set_settings',
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
function environments_task_variable_set_settings($form_state) {
	$form = array();
	$task = isset($form_state['task']) ? $form_state['task'] : NULL;

	// Always start as a list for new ones and custom when editing.
	$selection = $task ? 'custom' : 'list';

	// Override with submitted values.
	if (isset($form_state['values']['settings']['selection'])) {
		$selection = $form_state['values']['settings']['selection'];
	}

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
			'callback' => 'environments_ajax_task_settings_variable_set_name',
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
		'#description' => t('If the variable you\'re looking for isn\'t in this list, try using the "Specify manually" selection mode.'),
		'#required' => TRUE,
	);

	if ($selection === 'list') {
		$form['name']['#type'] = 'select';
		$form['name']['#multiple'] = FALSE;
		$form['name']['#options'] = drupal_map_assoc(_environments_get_variables());
		$form['name']['#empty_option'] = t(' -Select a variable- ');
		$form['name']['#default_value'] = isset($task['settings']['name']) ? (array) $task['settings']['name'] : array();
	}
	else if ($selection === 'custom') {
		$form['name']['#type'] = 'textfield';
		$form['name']['#maxlength'] = 255;
		$form['name']['#size'] = 40;
		$form['name']['#default_value'] = isset($task['settings']['name']) ? (string) $task['settings']['name'] : '';
	}

	$default_value = isset($task['settings']) && array_key_exists('value', $task['settings'])
		? $task['settings']['value']
		: '';

	$value_type = isset($form_state['values']['settings']['value_type'])
		? $form_state['values']['settings']['value_type']
		: FALSE;

	if (!$value_type) {
		if ($default_value !== '') {
			if (is_bool($default_value)) {
				$value_type = 'boolean';
				$default_value = intval($default_value);
			}
			else if (is_numeric($default_value)) {
				if (strpos('.', $default_value) !== FALSE) {
					$value_type = 'float';
				}
				else {
					$value_type = 'integer';
				}
			}
		}
	}

	$form['value_type'] = array(
		'#type' => 'select',
		'#title' => t('Value type'),
		'#options' => array(
			'string' => t('String'),
			'boolean' => t('Boolean'),
			'integer' => t('Integer'),
			'float' => t('Float'),
		),
		'#ajax' => array(
			'callback' => 'environments_ajax_task_settings_variable_set_value',
			'wrapper' => 'environments-variable-value',
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#default_value' => $value_type,
	);

	$form['value'] = array(
		'#prefix' => '<div id="environments-variable-value">',
		'#suffix' => '</div>',
		'#name' => 'settings[value]',
		'#title' => t('Value'),
		'#default_value' => $default_value,
		'#element_validate' => array('_environments_task_variable_set_validate_value'),
	);

	if ($value_type === 'boolean') {
		$form['value']['#type'] = 'select';
		$form['value']['#options'] = array(
			1 => t('True'),
			0 => t('False'),
		);
	}
	else if ($value_type === 'integer' || $value_type === 'float') {
		$type = module_exists('elements') ? 'numberfield' : 'textfield';
		$form['value']['#type'] = $type;
		$form['value']['#size'] = 20;
	}
	else {
		$form['value']['#type'] = 'textfield';
		$form['value']['#size'] = 40;
	}

	return $form;
}

/**
 * Task callback.
 *
 * @param array
 *   An array of settings for the task.
 *
 * @param array
 *   $context is an array that will contain information about the
 *   status of the batch. The values in $context will retain their
 *   values as the batch progresses.
 */
function environments_task_variable_set($settings, &$context) {
	$name = $settings['name'];
	$value = $settings['value'];
	$value_type = $settings['value_type'];

	variable_set($name, $value);

	if ($value_type === 'boolean') {
		$value = $value ? t('True') : t('False');
	}
	else if ($value_type === 'string') {
		$value = "\"{$value}\"";
	}

	// Display message.
	$context['message'] = t('Set variable "@name" value to !value', array('@name' => $name, '!value' => $value));
}

/**
 * AJAX Callback.
 *
 * @see environments_task_variable_set_settings()
 */
function environments_ajax_task_settings_variable_set_name($form, &$form_state) {
	return $form['settings']['name'];
}

/**
 * AJAX Callback.
 *
 * @see environments_task_variable_set_settings()
 */
function environments_ajax_task_settings_variable_set_value($form, &$form_state) {
	return $form['settings']['value'];
}
