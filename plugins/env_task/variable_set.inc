<?php

/**
 * @file
 * Contains the "variable_set" Env Task.
 */

$plugin = array(
  'admin_title' => t('Change variable value'),
  'task_callback' => 'env_task_variable_set',
  'settings' => array(
    'name' => '',
    'value' => '',
  ),
  'settings_callback' => 'env_task_variable_set_settings',
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
function env_task_variable_set_settings($environment, $task = NULL) {
  $form = array();

  $form['name'] = array(
    '#name' => 'settings[name]',
    '#type' => 'select',
    '#title' => t('Variable name'),
    '#multiple' => FALSE,
    '#options' => drupal_map_assoc(_env_get_variables()),
    '#empty_option' => t(' -Select a variable- '),
    '#default_value' => isset($task['settings']['name']) ? $task['settings']['name'] : array(),
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
		'#element_validate' => array('_env_task_variable_set_validate_value'),
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
function env_task_variable_set($settings, &$context) {
  $name = $settings['name'];
  $value = $settings['value'];

  variable_set($name, $value);

  // Display messages.
  $context['message'] = t('Set variable value @name to @value', array('@name' => $name, '@value' => $value));
}
