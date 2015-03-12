<?php

/**
 * @file
 * Drush integration.
 */

/**
 * Implements hook_drush_help().
 */
function environments_bundles_drush_help($section) {
	switch ($section) {
		case 'environments-bundle':
			return 'Executes an Environments Task Bundle.';
	}
}

/**
 * Implements hook_drush_command().
 */
function environments_bundles_drush_command() {
	return array(
		'environments-bundle' => array(
			'aliases' => array('envb'),
			'callback' => '_environments_bundles_drush_exec',
			'description' => 'Executes an Environments Task Bundle.',
			'arguments' => array(
				'bundle' => 'Optional. Specify the bundle\'s machine_name or omit it to list all available task bundles.',
			),
			'examples' => array(
				'drush envb demo_bundle' => 'Execute the "demo_bundle" task bundle.',
			),
		),
	);
}

/**
 * Drush command.
 *
 * @see environments_bundles_drush_command()
 */
function _environments_bundles_drush_exec($bundle_name = NULL) {
	if (isset($bundle_name)) {
		if ($bundle = environments_bundles_load($bundle_name)) {
			if (drush_confirm(t('Are you sure you want to execute this task bundle: !name?', array('!name' => $bundle->admin_title)))) {
				environments_bundles_exec($bundle);
			}
			else {
				drush_user_abort();
			}
		}
		else {
			drush_log(t('Invalid bundle name'), 'error');
			drush_user_abort();
		}
	}
	else {
		$bundles = environments_bundles_load_all();
		$rows = array();
		$rows[] = array(
			t('Name'),
			t('Machine Name'),
			t('Description'),
		);
		$rows[] = array(
			'-----',
			'-----',
			'-----',
		);

		foreach ($bundles as $bundle) {
			$rows[] = array(
				$bundle->admin_title,
				$bundle->name,
				$bundle->description,
			);
		}

		drush_print("\r");
		drush_print_table($rows, TRUE);
	}
}
