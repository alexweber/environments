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
 * Form callback; returns the settings form for this task.
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
		$form['name']['#attributes']['placeholder'] = 'my_awesome_variable';
	}

	$default_value = isset($task['settings']) && array_key_exists('value', $task['settings'])
		? $task['settings']['value']
		: '';

	$value_type = isset($form_state['values']['settings']['value_type'])
		? $form_state['values']['settings']['value_type']
		: FALSE;

	// If we pressed the "Try to find value" button, try to find and set it.
	$set_var_value = NULL;
	if (isset($form_state['triggering_element']['#name']) && $form_state['triggering_element']['#name'] === 'getvalue') {
		$varname = $form_state['values']['settings']['name'];
		$varval = variable_get($varname, NULL);

		if (isset($varval)) {
			$set_var_value = $varval;

			if (is_array($default_value)) {
				$set_var_value = serialize($set_var_value);
			}

			$default_value = $set_var_value;
			$value_type = NULL;
		}
	}

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
			else if ($default_value[0] === '{') {
				$value_type = 'json';
			}
			else if (strpos($default_value, ':') !== FALSE) {
				$value_type = 'serialized';
			}
		}

		if (!$value_type) {
			$value_type = 'string';
		}
	}

	$form['value_type'] = array(
		'#prefix' => '<div id="environments-variable-value-type" class="env-var-val-type">',
		'#suffix' => '</div>',
		'#type' => 'select',
		'#title' => t('Value type'),
		'#options' => array(
			'string' => t('String'),
			'boolean' => t('Boolean'),
			'integer' => t('Integer'),
			'float' => t('Float'),
			'serialized' => t('Serialized'),
			'json' => t('JSON'),
		),
		'#ajax' => array(
			'callback' => 'environments_ajax_task_settings_variable_set_value',
			'wrapper' => 'environments-variable-value',
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#default_value' => $value_type,
	);

	if (isset($set_var_value)) {
		$form['value_type']['#value'] = $value_type;
	}

	$form['value'] = array(
		'#prefix' => '<div id="environments-variable-value" class="env-var-val">',
		'#suffix' => '</div>',
		'#name' => 'settings[value]',
		'#title' => t('Value'),
		'#default_value' => $default_value,
		'#element_validate' => array('_environments_task_variable_set_validate_value'),
	);

	if (isset($set_var_value)) {
		$form['value']['#value'] = $default_value;
	}

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
		$form['value']['#size'] = 10;
		$form['value']['#attributes']['placeholder'] = '123';
	}
	else if ($value_type === 'serialized' || $value_type === 'json') {
		$form['value']['#type'] = 'textarea';
	}
	else {
		$form['value']['#type'] = 'textfield';
		$form['value']['#size'] = 40;
		$form['value']['#attributes']['placeholder'] = 'foo bar baz';
	}

	$form['getvalue'] = array(
		'#name' => 'getvalue',
		'#type' => 'button',
		'#button_type' => 'submit',
		'#executes_submit_callback' => FALSE,
		'#value' => t('Try to find value'),
		'#ajax' => array(
			'callback' => 'environments_ajax_task_settings_variable_get_value',
			'wrapper' => 'environments-variable-value',
			'method' => 'replace',
			'effect' => 'slide',
		),
		'#states' => array(
		  'visible' => array(
		    ':input[name="settings[name]"]' => array('filled' => TRUE),
		  ),
		),
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

	// Preprocess value & format display value.
	if ($value_type === 'boolean') {
		$value_display = $value ? t('True') : t('False');
	}
	else if ($value_type === 'string') {
		$value_display = "\"{$value}\"";
	}
	else if ($value_type === 'serialized') {
		$value = unserialize($value);
		$value_display = var_export($value, TRUE);
	}
	else if ($value_type === 'json') {
		$value = json_decode($value);
		$value_display = var_export($value, TRUE);
	}
	else {
		$value_display = $value;
	}

	// Set variable.
	variable_set($name, $value);

	// Display message.
	$context['message'] = t('Set variable "@name" value to !value', array('@name' => $name, '!value' => $value_display));
}

/**
 * AJAX callback; updates the variable name field according to selection mode.
 *
 * @see environments_task_variable_set_settings()
 */
function environments_ajax_task_settings_variable_set_name($form, &$form_state) {
	return $form['settings']['name'];
}

/**
 * AJAX callback; updates the variable value field according to the value type.
 *
 * @see environments_task_variable_set_settings()
 */
function environments_ajax_task_settings_variable_set_value($form, &$form_state) {
	return $form['settings']['value'];
}

/**
 * AJAX callback; updates both the variable type and value fields with values
 * determined automatically, if available.
 *
 * @see environments_task_variable_set_settings()
 */
function environments_ajax_task_settings_variable_get_value($form, &$form_state) {
	$commands = array();

	// Manually replace the contents of both fields.
	$commands[] = ajax_command_html('#environments-variable-value-type', render($form['settings']['value_type']));
	$commands[] = ajax_command_html('#environments-variable-value', render($form['settings']['value']));

	// Finally, return commands.
	return array('#type' => 'ajax', '#commands' => $commands);
}
