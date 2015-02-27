<?php

/**
 * @file
 * Contains the "execute_bundle" Environments Task.
 */

$plugin = array(
	'admin_title' => t('Execute task bundle'),
	'task_callback' => 'environments_task_execute_bundle',
	'settings' => array(
		'bundle' => '',
	),
	'settings_callback' => 'environments_task_execute_bundle_settings',
);

/**
 * Form bundle.
 *
 * Returns the settings form for this task.
 *
 * @param array
 *   A copy of the form_state array.
 *
 * @return array
 *   A Form API array of task settings.
 */
function environments_task_execute_bundle_settings($form_state) {
	$form = array();
	$task = isset($form_state['task']) ? $form_state['task'] : NULL;
	$options = array();
	$empty_description = t('You haven\'t created any Task Bundles yet.');

	foreach (environments_bundles_load_all() as $key => $value) {
		$options[$key] = $value->admin_title;
	}

	if (isset($form_state['object'])
		&& $form_state['object']->table === 'environments_bundles') {
		unset($options[$form_state['object']->name]);
		$empty_description = t('You haven\'t created any other Task Bundles yet.');
	}

	$form['name'] = array(
		'#name' => 'settings[name]',
		'#type' => 'select',
		'#title' => t('Bundle'),
		'#required' => TRUE,
		'#default_value' => isset($task['settings']['name']) ? $task['settings']['name'] : array(),
		'#options' => $options,
	);

	if (empty($options)) {
		$form['name']['#disabled'] = TRUE;
		$form['name']['#description'] = $empty_description;
	}

	return $form;
}

/**
 * Task bundle.
 *
 * @param array
 *   An array of settings for the task.
 * @param array
 *   $context is an array that will contain information about the
 *   status of the batch. The values in $context will retain their
 *   values as the batch progresses.
 */
function environments_task_execute_bundle($settings, &$context) {
	$bundle = $settings['name'];

	if ($bundle_object = environments_bundles_load($bundle)) {
		if (is_array($bundle_object->tasks)) {
			$operations = array();

			foreach (element_children($bundle_object->tasks, TRUE) as $task) {
				$task_info = $bundle_object->tasks[$task];
				$operations[] = array('_environments_task_dispatcher', array($task_info));
			}

			if (!empty($operations)) {
				$operations[] = array('_environments_task_flush', array(t('Cleaning up.')));

				$batch = array(
					'title' => t('Processing task bundle.'),
					'operations' => $operations,
					'init_message' => t('Preparing to execute task bundle.'),
					'progress_message' => t('Processed @current out of @total tasks.'),
					'error_message' => t('Error executing task bundle.'),
					'file' => drupal_get_path('module', 'environments') . '/environments.batch.inc',
				);

				batch_set($batch);
			}
			else {
				$context['message'] = t('Task bundle has no tasks: @name()', array('@name' => $bundle_object->admin_title));
			}
		}
	}
	else {
		$message = 'Error loading task bundle: @name()';
		$variables = array('@name' => $bundle);
		watchdog('environments', $message, $variables, WATCHDOG_ERROR);
		$context['message'] = t($message, $variables);
	}
}
