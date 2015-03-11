<?php

/**
 * @file
 * Contains the "rules_component" Environments Task.
 */

$plugin = array(
	'admin_title' => t('Execute rules component(s)'),
	'task_callback' => 'environments_task_rules_component',
	'settings' => array(
		'components' => array(),
	),
	'settings_callback' => 'environments_task_rules_component_settings',
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
function environments_task_rules_component_settings($form_state) {
	$form = array();
	$task = isset($form_state['task']) ? $form_state['task'] : NULL;

	$form['components'] = array(
		'#name' => 'settings[components]',
		'#type' => 'select',
		'#title' => t('Components'),
		'#description' => t('Caution: This task will fail if the Rules module is not enabled.'),
		'#multiple' => TRUE,
		'#options' => _environments_get_rules_components(),
		'#empty_option' => t(' -Select components- '),
		'#default_value' => isset($task['settings']['components']) ? $task['settings']['components'] : array(),
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
 *   If the Rules module is not enabled.
 */
function environments_task_rules_component($settings, &$context) {
	$components = $settings['components'];
	$messages = array();

	if (!module_exists('rules')) {
		throw new Exception(t('Rules module must be enabled to execute the "rules_component" Environments task.'));
	}

	foreach ($components as $machine_name) {
		if (rules_invoke_component($machine_name)) {
			$messages[] = t('Executed component: @name', array('@name' => $machine_name));
		}
		else {
			$messages[] = t('Error executing component: @name', array('@name' => $machine_name));
		}
	}

	// Display messages.
	$context['message'] = implode("\n", $messages);
}
