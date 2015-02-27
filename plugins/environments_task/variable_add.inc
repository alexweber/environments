<?php

/**
 * @file
 * Contains the "variable_add" Environments Task.
 */

$plugin = array(
  'admin_title' => t('Add new variable'),
  'task_callback' => 'environments_task_variable_add',
  'settings' => array(
    'name' => '',
    'value' => '',
  ),
  'settings_callback' => 'environments_task_variable_add_settings',
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
function environments_task_variable_add_settings($form_state) {
	$task = isset($form_state['task']) ? $form_state['task'] : NULL;
  $form = array();

  $form['name'] = array(
    '#name' => 'settings[name]',
    '#type' => 'textfield',
    '#title' => t('Variable name'),
		'#description' => t('Specify the variable\'s name, if it already exists it will be overwritten.'),
		'#required' => TRUE,
    '#default_value' => isset($task['settings']['name']) ? $task['settings']['name'] : '',
  );

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
		// Use the same validation callback as the variable_set task.
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
function environments_task_variable_add($settings, &$context) {
  $name = $settings['name'];
  $value = $settings['value'];
	$value_type = $settings['value_type'];

  variable_set($name, $value);

	if ($value_type === 'boolean') {
		$value = $value ? t('True') : t('False');
	}

  // Display message.
  $context['message'] = t('Variable value @name initialized to @value', array('@name' => $name, '@value' => $value));
}
