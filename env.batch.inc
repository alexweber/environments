<?php

/**
 * @file
 * Batch API callbacks.
 */

/**
 * Batch API operation callback.
 *
 * Includes necessary files and executes task callbacks for a given environment.
 *
 * @see env_set()
 */
function _env_task_dispatcher($task_info, &$context) {
	// Load task plugin.
	if ($full_task = env_get_task($task_info['name'])) {
		// Resolve callback function.
		if ($callback = ctools_plugin_get_function($full_task, 'task_callback')) {
			// Execute it. We call it directly instead of using call_user_func() to
			// be able to pass the $context var by reference.
			$callback($task_info['settings'], $context);
		}
		else {
			watchdog('env', 'Error loading callback for task: @name', array('@name' => $task_info), WATCHDOG_ERROR);
		}
	}
	else {
		watchdog('env', 'Error loading task: @name', array('@name' => $task_info), WATCHDOG_ERROR);
	}
}

/**
 * Batch API operation callback.
 *
 * Flush Drupal's caches and set the environment in results context var for
 * finished callback.
 *
 * This is called automatically as the last step to
 * task batch operations.
 *
 * @see env_set()
 */
function _env_task_cleanup($operation, $env, &$context) {
	$context['message'] = t('@operation', array('@operation' => $operation));
	$context['results']['env'] = $env;
	drupal_flush_all_caches();
}

/**
 * Batch API finished callback.
 *
 * @see env_set()
 */
function _env_task_finished($success, $results, $operations) {
	if ($success) {
		if ($env = $results['env']) {
			if ($environment = env_environment_load($env)) {
				variable_set('env', $env);
				watchdog('env', 'Environment set to: @name', array('@name' => $environment->admin_title), WATCHDOG_INFO);
				drupal_set_message(t('Environment set to: @name', array('@name' => $environment->admin_title)));
				return;
			}
		}
	}

	// An error occurred.
	// $operations contains the operations that remained unprocessed.
	$error_operation = reset($operations);
	$message = t('An error occurred while processing %error_operation with arguments: @arguments', array(
		'%error_operation' => $error_operation[0],
		'@arguments' => print_r($error_operation[1], TRUE)
	));
	drupal_set_message($message, 'error');
}
