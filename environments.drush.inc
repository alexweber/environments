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
    case 'environment':
      return 'Switches your site\'s configuration between server environment presets.';

    case 'environment-unset':
      return 'Un-sets your site\'s current environment.';
  }
}

/**
 * Implements hook_drush_command().
 */
function environments_drush_command() {
  return array(
    'environment' => array(
      'aliases' => array('env'),
      'callback' => '_environments_drush_env',
      'description' => 'Switches your site\'s configuration between server environment presets. Omit the argument to list all available environments.',
      'arguments' => array(
        'envname' => 'Optional. Specify the environment by it\'s machine_name or alias.',
      ),
      'options' => array(
        'force' => 'Force set the environment, even it it\'s already the current one.',
      ),
      'examples' => array(
        'drush env development' => 'Put the site in development mode (using machine_name)',
        'drush env dev' => 'Put the site in development mode (using alias)',
        'drush env prod' => 'Put the site in production mode',
      ),
    ),
    'environment-unset' => array(
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
  if (drush_confirm(t("Are you sure you want to unset your site's environment?"))) {
    environments_unset();
    drush_log(t("Your site's environment has been unset."), 'status');
  }
  else {
    drush_user_abort();
  }
}

/**
 * Drush command callback.
 *
 * @param string $env
 *   The environment name or alias.
 *
 * @see environments_drush_command()
 */
function _environments_drush_env($env = NULL) {
  // Determine current environment.
  $current_env = variable_get('environments_env');

  // If we have input, try to figure out what environment we're supposed to
  // change to and do it.
  if (isset($env)) {
    $all_environments = environments_load_all();
    $environment = FALSE;

    // A full environment name is the best case scenario.
    if (array_key_exists($env, $all_environments)) {
      $environment = $all_environments[$env];
    }
    // Check for aliases.
    else {
      foreach ($all_environments as $temp) {
        if ($temp->alias === $env) {
          $environment = $temp;
          break;
        }
      }
    }

    // Proceed only if we have an environment.
    if ($environment) {
      $force = drush_get_option('force') ? TRUE : FALSE;

      if ($force || $environment->name !== $current_env) {
        if (drush_confirm(t("Are you sure you want to change your site's environment to: @environment?", array('@environment' => $environment->admin_title)))) {
          environments_set($environment->name);
          return;
        }
        else {
          drush_user_abort();
          return;
        }
      }
      else {
        watchdog('environments', 'Environment is already set: @environment', array('@environment' => $environment->admin_title), WATCHDOG_INFO);
        drush_log(t('Environment is already set: @environment', array('@environment' => $environment->admin_title)), 'warning');
        return;
      }
    }

    watchdog('environments', 'Environment not defined: @environment', array('@environment' => $env), WATCHDOG_ERROR);
    drush_log('Environment not defined: @environment', array('@environment' => $env), 'error');
    drush_user_abort();
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

    drush_print("\r");
    drush_print_table($rows, TRUE);
  }
}
