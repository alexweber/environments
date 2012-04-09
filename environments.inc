<?php

/**
 *
 * Environment-Specific configuration manager
 *
 */
class DrupalEnvironment {

  const ENVDIR = 'environments/';

  /**
   * @static
   *
   * Static initializer.
   */
  public static function init() {

    $envname_server = !empty($_SERVER['DRUPAL_ENV_NAME']) ? $_SERVER['DRUPAL_ENV_NAME'] : "";
    $envname_shell = !empty($_ENV['DRUPAL_ENV_NAME']) ? $_ENV['DRUPAL_ENV_NAME'] : "";
    $envname = !empty($envname_server) ? $envname_server : $envname_shell;

    if(empty($envname)) {
        die("Drupal Environment was not properly set up. Please set up DRUPAL_ENV_NAME variable either
             in shell or in your web-server configuration. Aborting.\n");
    }

    $configFilePath = __DIR__ . '/' . self::ENVDIR . '/' . $envname . ".inc";

    return $configFilePath;
  }

}

$drupalEnvConfigFilePath = DrupalEnvironment::init();

if(!file_exists($drupalEnvConfigFilePath)) {
  die ("Environment configuration file $drupalEnvConfigFilePath not found");
}

require($drupalEnvConfigFilePath);