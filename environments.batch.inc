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
 * @see environments_set()
 */
function _environments_task_dispatcher($task_info, &$context) {
	// Load task plugin.
	if ($full_task = environments_get_task($task_info['name'])) {
		// Resolve callback function.
		if ($callback = ctools_plugin_get_function($full_task, 'task_callback')) {
			// Execute it. We call it directly instead of using call_user_func() to
			// be able to pass the $context var by reference.
			$callback($task_info['settings'], $context);
		}
		else {
			watchdog('environments', 'Error loading callback for task: @name', array('@name' => $task_info['name']), WATCHDOG_ERROR);
		}
	}
	else {
		watchdog('environments', 'Error loading task: @name', array('@name' => $task_info['name']), WATCHDOG_ERROR);
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
 * @see environments_set()
 */
function _environments_task_cleanup($operation, $object, &$context) {
	$context['message'] = t('@operation', array('@operation' => $operation));
	$context['results']['object'] = $object;
	drupal_flush_all_caches();
}

/**
 * Batch API operation callback.
 *
 * Flush Drupal's caches. This is a lightweight replacement for the cleanup
 * task, which isn't suitable for Task Bundles.
 *
 * This is called automatically as the last step for task bundle batch
 * operations.
 *
 * @see environments_task_execute_bundle()
 */
function _environments_task_flush($operation, &$context) {
	$context['message'] = t('@operation', array('@operation' => $operation));
	drupal_flush_all_caches();
}

/**
 * Batch API finished callback.
 *
 * @see environments_set()
 */
function _environments_task_finished($success, $results, $operations) {
	if ($success) {
		if ($env = $results['object']) {
			if ($environment = environments_load($env)) {
				variable_set('environments_env', $env);
				watchdog('environments', 'Environment set to: @name', array('@name' => $environment->admin_title), WATCHDOG_INFO);
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
	watchdog('environments', $message, array(), WATCHDOG_ERROR);
}

/**
 * Batch API finished callback.
 *
 * @see environments_bundles_exec_form_submit()
 */
function _environments_bundles_task_finished($success, $results, $operations) {
	if ($success) {
		if ($bundle = $results['object']) {
			watchdog('environments', 'Executed task bundle: !name', array('!name' => $bundle->admin_title), WATCHDOG_INFO);
			drupal_set_message(t('Executed task bundle: !name', array('!name' => $bundle->admin_title)));
			return;
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
	watchdog('environments', $message, array(), WATCHDOG_ERROR);
}
