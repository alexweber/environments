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
		'enable' => TRUE,
	),
	'settings_callback' => 'environments_task_rules_component_settings',
	'summary_callback' => 'environments_task_rules_component_summary',
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
		'#description' => t('You can select one or more Rules "Action" components.'),
		'#multiple' => TRUE,
		'#options' => _environments_get_rules_components(),
		'#empty_option' => t(' -Select components- '),
		'#default_value' => isset($task['settings']['components']) ? $task['settings']['components'] : array(),
	);

	$form['enable'] = array(
		'#name' => 'settings[enable]',
		'#type' => 'checkbox',
		'#title' => t('Enable Rules'),
		'#description' => t('If checked, the Rules module will be explicitly enabled. Otherwise, it\'s up to you to make sure it\'s enabled when this task runs! This task will fail if Rules is not enabled.'),
		'#default_value' => isset($task['settings']['enable']) ? $task['settings']['enable'] : TRUE,
	);

	return $form;
}

/**
 * Summary callback; format a custom summary of our task settings.
 *
 * @param array
 *   An array of settings.
 *
 * @return array
 *   An array containing a short summary for each setting.
 */
function environments_task_rules_component_summary($settings) {
	$summary = array();

	$summary[] = '<strong>' . t('Components') . ':</strong> ' . implode(', ', $settings['components']);
	$summary[] = '<strong>' . t('Enable Rules') . ':</strong> ' . ($settings['enable'] ? t('yes') : t('no'));

	return $summary;
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
	$enable = $settings['enable'];
	$messages = array();

	if ($enable && !module_exists('rules')) {
		module_enable(array('rules'));
	}

	// Keep it simple.
	if (!function_exists('rules_invoke_component')) {
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

/**
 * Returns all available Rules "action" Components.
 *
 * @return array
 *   An array of component names, keyed by machine_name.
 */
function _environments_get_rules_components() {
	static $components = array();

	if (empty($components) && function_exists('rules_get_components')) {
		$loaded_components = rules_get_components(FALSE, 'action');

		foreach ($loaded_components as $machine_name => $component) {
			$components[$machine_name] = $component->label();
		}
	}

	return $components;
}
