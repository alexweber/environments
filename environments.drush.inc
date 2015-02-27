<?php

/**
 * @file
 * Drush integration.
 */

/**
 * Implements hook_drush_help().
 */
function environments_drush_help($section) {
  switch ($section) {
    case 'environments':
      return 'Switches your site\'s configuration between server environment presets.';
  }
}

/**
 * Implements hook_drush_command().
 */
function environments_drush_command() {
  return array(
    'environments' => array(
			'aliases' => array('env'),
      'callback' => '_environments_drush_env',
      'description' => 'Switches your site\'s configuration between server environment presets. Omit the argument to list all available environments.',
      'arguments' => array(
        'envname' => 'Optional. Specify the environment by it\'s machine_name or alias.',
      ),
      'examples' => array(
        'drush env development' => 'Put the site in development mode (using machine_name)',
				'drush env dev' => 'Put the site in development mode (using alias)',
        'drush env prod' => 'Put the site in production mode',
      ),
    ),
		'environments-unset' => array(
			'aliases' => array('env-unset'),
			'callback' => '_environments_drush_env_unset',
			'description' => 'Un-sets your site\'s current environment.',
		),
  );
}

/**
 * Drush command callback.
 *
 * @see environments_drush_command()
 */
function _environments_drush_env_unset() {
	if (drush_confirm(t('Are you sure you want to unset your site\'s environment?'))) {
		environments_unset();
		drush_log(t('Your site\'s environment has been unset.'), 'status');
	}
	else {
		drush_user_abort();
	}
}

/**
 * Drush command callback.
 *
 * @param string $env
 * @return bool
 *
 * @see environments_drush_command()
 */
function _environments_drush_env($env = NULL) {
  // Determine current environment.
  $current_env = variable_get('environments_env');

	// If we have input, try to change current environment.
	if (isset($env)) {
		// Resolve our input.
		$query = db_select('environments', 'e')
			->fields('e', array('name'))
			->condition(
				db_or()
					->condition('e.name', $env, '=')
					->condition('e.alias', $env, '=')
			);

		if ($input_env = $query->execute()->fetchField()) {
			if ($environment = environments_load($input_env)) {
				if ($input_env != $current_env) {
					if (drush_confirm(t('Are you sure you want to change your site\'s environment to: @environment?', array('@environment' => $environment->admin_title)))) {
						environments_set($environment->name);
						return TRUE;
					}
					else {
						drush_user_abort();
						return FALSE;
					}
				}
				else {
					watchdog('environments', 'Environment is already set: @environment', array('@environment' => $environment->admin_title), WATCHDOG_INFO);
					drush_log(t('Environment is already set: @environment', array('@environment' => $environment->admin_title)), 'warning');
					return TRUE;
				}
			}
		}

		watchdog('environments', 'Environment not defined: @environment', array('@environment' => $env), WATCHDOG_ERROR);
		drush_log('Environment not defined: @environment', array('@environment' => $env), 'error');
		drush_user_abort();
		return FALSE;
  }
	// If not, display status.
  else {
		$environments = environments_load_all();
		$rows = array();
		$rows[] = array(
			t('Name'),
			t('Alias'),
			t('Active'),
		);
		$rows[] = array(
			'-----',
			'-----',
			'-----',
		);

    foreach ($environments as $environment) {
      $rows[] = array(
				$environment->admin_title,
				$environment->alias,
				$environment->name === $current_env ? '  x' : '',
			);
    }

		drush_print_table($rows, TRUE);
  }

  return TRUE;
}
