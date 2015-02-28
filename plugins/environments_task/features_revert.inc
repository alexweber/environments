<?php

/**
 * @file
 * Contains the "features_revert" Environments Task.
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
 * @param array
 *   A copy of the form_state array.
 *
 * @return array
 *   A Form API array of task settings.
 */
function environments_task_features_revert_settings($form_state) {
	$form = array();
	$task = isset($form_state['task']) ? $form_state['task'] : NULL;

	$form['modules'] = array(
		'#name' => 'settings[modules]',
		'#type' => 'select',
		'#title' => t('Features'),
		'#description' => t('Caution: This task will fail if the Features module is not enabled.'),
		'#multiple' => TRUE,
		'#options' => _environments_get_features(),
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
 *
 * @throws Exception
 *   If the Migrate module is not enabled.
 */
function environments_task_features_revert($settings, &$context) {
	$modules = $settings['modules'];
	$messages = array();

	if (!module_exists('features')) {
		throw new Exception(t('Features module must be enabled to execute the "features_revert" Environments task.'));
	}

	module_load_include('inc', 'features', 'features.export');

	foreach ($modules as $module) {
		if (module_exists($module)) {
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
					$messages[] = t('Reverted feature: !feature.', array('!feature' => $module));
				}
				else {
					$messages[] = t('Error loading feature: !feature.', array('!feature' => $module));
				}
			}
			else {
				$messages[] = t('Feature already matches default state: !feature.', array('!feature' => $module));
			}
		}
		else {
			$messages[] = t('Can\'t revert a disabled feature: !feature.', array('!feature' => $module));
		}
	}

	// Display messages.
	$context['message'] = implode(' ', $messages);
}
