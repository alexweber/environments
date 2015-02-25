<?php

/**
 * @file
 * Drush integration.
 */

/**
 * Implements hook_drush_help().
 */
function env_drush_help($section) {
  switch ($section) {
    case 'env':
      return 'Switches between site server environments.';
  }
}

/**
 * Implements hook_drush_command().
 */
function env_drush_command() {
  return array(
    'env' => array(
      'callback' => '_env_drush_env',
      'description' => 'Switches between site server environments. If you leave out the argument, it will list all environments and inform which is the current one.',
      'arguments' => array(
        'env' => 'Optional. Specify the environment by machine_name or alias. If you leave out this argument, it will list all environments and inform which is the current one.',
      ),
      'examples' => array(
        'drush env dev' => 'Put the site in development mode (using alias)',
        'drush env development' => 'Put the site in development mode (using machine_name)',
        'drush env stage' => 'Put the site in staging mode',
        'drush env prod' => 'Put the site in production mode',
      ),
    ),
  );
}

/**
 * Router for 'env' drush command.
 *
 * @param string $env
 * @return bool
 */
function _env_drush_env($env = NULL) {
  // Determine current environment.
  $current_env = variable_get('environments_env');

	// If we have input, try to change current environment.
	if (isset($env)) {
		// Resolve our input.
		$query = db_select('env_environment', 'e')
			->fields('e', array('name'))
			->condition(
				db_or()
					->condition('e.name', $env, '=')
					->condition('e.alias', $env, '=')
			);

		if ($input_env = $query->execute()->fetchField()) {
			if ($environment = env_environment_load($input_env)) {
				if ($input_env != $current_env) {
					if (drush_confirm(t('Are you sure you want to change your site\'s environment to: @env?', array('@env' => $environment->admin_title)))) {
						environments_set($environment->name);
						return TRUE;
					}
					else {
						drush_user_abort();
						return FALSE;
					}
				}
				else {
					watchdog('environments', 'Environment is already set: @env', array('@env' => $environment->admin_title), WATCHDOG_INFO);
					drush_log(t('Environment is already set: @env', array('@env' => $environment->admin_title)), 'warning');
					return TRUE;
				}
			}
		}

		watchdog('environments', 'Environment not defined: @env', array('@env' => $env), WATCHDOG_ERROR);
		drush_log('Environment not defined: @env', array('@env' => $env), 'error');
		drush_user_abort();
		return FALSE;
  }
	// If not, display status.
  else {
		$environments = env_environment_load_all();
    $output = array();

    foreach ($environments as $env_info) {
      $temp = "{$env_info->admin_title} ({$env_info->alias})";

      if ($env_info->name === $current_env) {
        $temp .= " *active*";
      }

      $output[] = $temp;
    }

    drush_log(implode("\n", $output), 'ok');
  }

  return TRUE;
}
