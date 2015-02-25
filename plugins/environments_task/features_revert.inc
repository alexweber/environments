<?php

/**
 * @file
 * Contains the "features_revert" Env Task.
 */

$plugin = array(
	'admin_title' => t('Revert feature(s)'),
	'task_callback' => 'environments_task_features_revert',
	'settings' => array(
		'modules' => array(),
	),
	'settings_callback' => 'environments_task_features_revert_settings',
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
function environments_task_features_revert_settings($environment, $task = NULL) {
	$form = array();

	$form['modules'] = array(
		'#name' => 'settings[modules]',
		'#type' => 'select',
		'#title' => t('Features'),
		'#multiple' => TRUE,
		'#options' => _env_get_features(),
		'#empty_option' => t(' -Select some features- '),
		'#default_value' => isset($task['settings']['modules']) ? $task['settings']['modules'] : array(),
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
function environments_task_features_revert($settings, &$context) {
	$modules = $settings['modules'];
	$messages = array();

	foreach ($modules as $module) {
		if (module_exists($module)) {
			module_load_include('inc', 'features', 'features.export');
			$storage = features_get_storage($module);
			if (in_array($storage, array(
				FEATURES_OVERRIDDEN,
				FEATURES_NEEDS_REVIEW,
				FEATURES_REBUILDABLE
			))) {
				if ($feature = features_load_feature($module, TRUE)) {
					$items[$module] = array();

					foreach (array_keys($feature->info['features']) as $component) {
						if (features_hook($component, 'features_revert')) {
							$items[$module][] = $component;
						}
					}

					features_revert($items);
					$messages[] = t('Reverted feature: %feature.', array('%feature' => $module));
				}
				else {
					$messages[] = t('Error loading feature: %feature.', array('%feature' => $module));
				}
			}
			else {
				$messages[] = t('Feature already matches default state: %feature.', array('%feature' => $module));
			}
		}
		else {
			$messages[] = t('Can\'t revert a disabled feature: %feature.', array('%feature' => $module));
		}
	}

	// Display messages.
	$context['message'] = implode(' ', $messages);
}
