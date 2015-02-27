<?php

/**
 * @file
 * Contains the "module_uninstall" Environments Task.
 */

$plugin = array(
	'admin_title' => t('Uninstall module(s)'),
	'task_callback' => 'environments_task_module_uninstall',
	'settings' => array(
		'modules' => array(),
		'uninstall_dependents' => TRUE,
	),
	'settings_callback' => 'environments_task_module_uninstall_settings',
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
function environments_task_module_uninstall_settings($form_state) {
	$form = array();
	$task = isset($form_state['task']) ? $form_state['task'] : NULL;

	$form['modules'] = array(
		'#name' => 'settings[modules]',
		'#type' => 'select',
		'#title' => t('Modules'),
		'#multiple' => TRUE,
		'#options' => _environments_get_modules(),
		'#empty_option' => t(' -Select some modules- '),
		'#default_value' => isset($task['settings']['modules']) ? $task['settings']['modules'] : array(),
	);

	$form['uninstall_dependents'] = array(
		'#name' => 'settings[uninstall_dependents]',
		'#type' => 'checkbox',
		'#title' => t('Uninstall dependents'),
		'#description' => t('If TRUE, dependent modules will automatically be uninstalled in the correct order.'),
		'#default_value' => isset($task['settings']['uninstall_dependents']) ? $task['settings']['uninstall_dependents'] : TRUE,
	);

	$form['uninstall_enabled'] = array(
		'#name' => 'settings[uninstall_enabled]',
		'#type' => 'checkbox',
		'#title' => t('Disable enabled modules'),
		'#description' => t('If TRUE, enabled modules will automatically be disabled before being uninstalled. If FALSE, attempting to uninstall an enabled module will fail.'),
		'#default_value' => isset($task['settings']['uninstall_enabled']) ? $task['settings']['uninstall_enabled'] : TRUE,
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
function environments_task_module_uninstall($settings, &$context) {
	$modules = $settings['modules'];
	$uninstall_dependents = $settings['uninstall_dependents'];
	$uninstall_enabled = $settings['uninstall_enabled'];

	$messages = array();
	$modules_disable = array();

	// Check whether we're supposed to disable enabled modules ourselves so that
	// they can be uninstalled properly.
	if ($uninstall_enabled) {
		foreach ($modules as $module) {
			if (module_exists($module)) {
				$modules_disable[] = $module;
			}
		}

		// Disable enabled modules first.
		if (!empty($modules_disable)) {
			module_disable($modules_disable, $uninstall_dependents);
			$messages[] = t('The following modules were manually disabled before being uninstalled: %module.', array('%module' => implode(', ', array_values($modules_disable))));
		}
	}

	// Finally, uninstall modules.
	if (drupal_uninstall_modules($modules, $uninstall_dependents)) {
		$messages[] = t('Uninstalled modules: %modules.', array('%module' => implode(', ', array_values($modules))));
	}

	// Display messages.
	$context['message'] = implode(' ', $messages);
}
