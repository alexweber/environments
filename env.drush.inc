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
      return 'Switches between website server environments.';
  }
}

/**
 * Implements hook_drush_command().
 */
function env_drush_command() {
  return array(
    'env' => array(
      'callback' => '_env_drush_env',
      'description' => "Switches between website server environments. If you ommit out the argument the function will list all environments and inform what is the current one.",
      'arguments' => array(
        'env' => "Optional. Specify the environment by machine_name. If you ommit the argument the function will list all environments and inform what is the current one.",
      ),
      'examples' => array(
        'drush env dev' => 'Put the site in development mode',
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
  // Load all environments.
  $environments = env_environment_load_all();

  // Get current environment.
  $current_env = variable_get('env');

  if (isset($env)) {
    if (!array_key_exists($env, $environments)) {
      $is_alias = FALSE;

      // Check for alias before giving up.
      // @TODO query database instead
      foreach ($environments as $environment) {
        if ($environment->alias === $env) {
          $env = $environment->name;
          $is_alias = TRUE;
          break;
        }
      }

      if (!$is_alias) {
        drush_log('Environment not defined: ' . $env, 'error');
        return FALSE;
      }
    }

    if ($env != $current_env) {
      env_set($env);
    }
    else {
      drush_log('Environment is already set: ' . $environments[$env]->admin_title, 'warning');
    }
  }
  else {
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
