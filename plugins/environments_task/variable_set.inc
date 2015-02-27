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

	$selection = isset($form_state['values']['settings']['selection'])
		? $form_state['values']['settings']['selection']
		: 'list';

	$form['selection'] = array(
		'#title' => t('Selection mode'),
		'#description' => t('Specify how you would like to define the variable to modify. Selecting it from a list is easier, but only variables that have been explicitly set are available.'),
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
		$form['name']['#size'] = 8;
		$form['name']['#default_value'] = isset($task['settings']['name']) ? (string) $task['settings']['name'] : '';
	}

	$default_value = array_key_exists('value', $task['settings'])
		? $task['settings']['value']
		: '';
	$default_type = 'string';

	if ($default_value !== '') {
		if (is_bool($default_value)) {
			$default_type = 'boolean';
			$default_value = intval($default_value);
		}
		else if (is_numeric($default_value)) {
			if (strpos('.', $default_value) !== FALSE) {
				$default_type = 'float';
			}
			else {
				$default_type = 'integer';
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
		'#default_value' => $default_type,
	);

	$form['value'] = array(
		'#name' => 'settings[value]',
		'#type' => 'textfield',
		'#title' => t('Value'),
		'#description' => t('Note: Be sure to select the most appropriate type as this value will be type-casted. Use "1" and "0" for boolean values.'),
		'#default_value' => $default_value,
		'#element_validate' => array('_environments_task_variable_set_validate_value'),
	);

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
	$context['message'] = t('Set variable "@name" value to: @value', array('@name' => $name, '@value' => $value));
}

/**
 * AJAX Callback.
 *
 * Returns task settings form.
 *
 * @see environments_task_variable_set_settings()
 */
function environments_ajax_task_settings_variable_set_name($form, &$form_state) {
	return $form['settings']['name'];
}
