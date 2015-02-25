<?php

/**
 * @file
 * Contains the "cache_clear" Env Task.
 */

$plugin = array(
	'admin_title' => t('Clear caches'),
	'task_callback' => 'environments_task_cache_clear',
	// This task has no settings.
);

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
function environments_task_cache_clear($settings, &$context) {
	drupal_flush_all_caches();
	$context['message'] = t('Flushed all caches.');
}