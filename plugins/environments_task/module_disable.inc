<?php

/**
 * @file
 * Contains the "module_disable" Environments Task.
 */

$plugin = array(
	'admin_title' => t('Disable module(s)'),
	'task_callback' => 'environments_task_module_disable',
	'settings' => array(
		'modules' => array(),
		'disable_dependents' => TRUE,
	),
	'settings_callback' => 'environments_task_module_disable_settings',
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
function environments_task_module_disable_settings($form_state) {
	$form = array();
	$task = isset($form_state['task']) ? $form_state['task'] : NULL;

	$form['modules'] = array(
		'#name' => 'settings[modules]',
		'#type' => 'select',
		'#title' => t('Modules'),
		'#multiple' => TRUE,
		'#options' => _environments_get_modules(),
		'#empty_option' => t(' -Select modules- '),
		'#default_value' => isset($task['settings']['modules']) ? $task['settings']['modules'] : array(),
	);

	$form['disable_dependents'] = array(
		'#name' => 'settings[disable_dependents]',
		'#type' => 'checkbox',
		'#title' => t('Disable dependents'),
		'#description' => t('If TRUE, dependent modules will automatically be disabled in the correct order.'),
		'#default_value' => isset($task['settings']['disable_dependents']) ? $task['settings']['disable_dependents'] : TRUE,
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
function environments_task_module_disable($settings, &$context) {
	$modules = $settings['modules'];
	$disable_dependents = $settings['disable_dependents'];

	// Filter already-disabled modules.
	$messages = array();
	$modules_filtered = array();

	foreach ($modules as $module) {
		if (!module_exists($module)) {
			$messages[] = t('@module is already disabled.', array('@module' => $module));
		}
		else {
			$modules_filtered[] = $module;
		}
	}

	// Disable modules.
	if (!empty($modules_filtered)) {
		module_disable($modules_filtered, $disable_dependents);

		// Display this message before all others.
		array_unshift($messages, t('Disabled modules: @modules.', array('@modules' => implode(', ', array_values($modules_filtered)))));
	}

	// Display messages.
	$context['message'] = implode("\n", $messages);
}
