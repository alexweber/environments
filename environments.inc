<?php

/**
 * Environment-Specific configuration manager
 */
class DrupalEnvironment {

  const ENVDIR = 'environments';

  private static $currentEnvironment = null;

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

    $configFilePath = __DIR__ . DIRECTORY_SEPARATOR . self::ENVDIR . DIRECTORY_SEPARATOR . $envname . ".inc";
    self::$currentEnvironment = $envname;

    return $configFilePath;
  }

  public static function getCurrent() {
    return self::$currentEnvironment;
  }

}

$drupalEnvConfigFilePath = DrupalEnvironment::init();

// Not checking file_exists here because we need every bit of performance and file_exists() is not that fast.
require($drupalEnvConfigFilePath);
