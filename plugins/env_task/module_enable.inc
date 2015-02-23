<?php

/**
 * @file
 * Contains the "module_enable" Env Task.
 */


/**
 * Define this Task plugin.
 */
$plugin = array(
	// Title for Admin UI.
	'admin_title' => t('Enable modules'),
	// Batch API callback to execute. You can also define this using the
	// alternative method below.
	'task_callback' => 'env_task_module_enable',
	// Batch API callback to execute. This is an alternative implementation that
	// gives you more control over where your callback lives and what it's called.
	// Which callback to use is automatically determined by CTools.
//  'task_callback' => array(
//    'file' => '',
//    'path' => '',
//    'function' => '',
//  ),

	// Define any settings your task might implement.
	// It's ok to have no settings. If, however, you *do* have some, make sure
	// you create a settings_callback to allow these values to be changed.
	'settings' => array(
		'modules' => array(),
		'enable_dependencies' => TRUE,
	),
	// Optional Form callback for settings form. You can also define this using
	// a similar alternative method as with the batch callbacks.
	'settings_callback' => 'env_task_module_enable_settings',
	// Optional Form callback for settings form. This is the alternative method.
	// Refer to the explanation for "task_callback" above for more information.
//  'settings_callback' => array(
//    'file' => '',
//    'path' => '',
//    'function' => '',
//  ),
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
function env_task_module_enable_settings($environment, $task = NULL) {
	$form = array();

	$form['modules'] = array(
		'#name' => 'settings[modules]',
		'#type' => 'select',
		'#title' => t('Modules'),
		'#multiple' => TRUE,
		'#options' => _env_get_modules(),
		'#empty_option' => t(' -Select some modules- '),
		'#default_value' => isset($task['settings']['modules']) ? $task['settings']['modules'] : array(),
	);
	$form['enable_dependencies'] = array(
		'#name' => 'settings[enable_dependencies]',
		'#type' => 'checkbox',
		'#title' => t('Enable dependencies'),
		'#description' => t('If TRUE, dependent modules will automatically be enabled in the correct order.'),
		'#default_value' => isset($task['settings']['enable_dependencies']) ? $task['settings']['enable_dependencies'] : TRUE,
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
function env_task_module_enable($settings, &$context) {
	$modules = $settings['modules'];
	$enable_dependencies = $settings['enable_dependencies'];

	// Filter already-enabled modules.
	$messages = array();
	$modules_filtered = array();

	foreach ($modules as $module) {
		if (module_exists($module)) {
			$messages[] = t('Can\'t enable module %module, it\'s already enabled.', array('%module' => $module));
		}
		else {
			$modules_filtered[] = $module;
		}
	}

	// Enable modules.
	if (!empty($modules_filtered)) {
		if (module_enable($modules_filtered, $enable_dependencies)) {
			// Display this message before all others.
			array_unshift($messages, t('Enabled modules: %modules.', array('%module' => implode(', ', array_values($modules_filtered)))));
		}
	}

	// Display messages.
	$context['message'] = implode("\n", $messages);
}
