<?php
/**
 * @file
 * environments_example.default_environments_environments.inc
 */

/**
 * Implements hook_default_environments_environments().
 */
function environments_example_default_environments_environments() {
  $export = array();

  $environment = new stdClass();
  $environment->disabled = FALSE; /* Edit this to true to make a default environment disabled initially */
  $environment->api_version = 1;
  $environment->name = 'demo_environment';
  $environment->admin_title = 'Demo Environment';
  $environment->alias = 'demo';
  $environment->settings = array(
    'notify' => 1,
  );
  $environment->tasks = array(
    'a9a39b307fe325b6888d88d9a1ba46f0' => array(
      'name' => 'execute_bundle',
      'settings' => array(
        'name' => 'demo_bundle',
      ),
      '#id' => 'a9a39b307fe325b6888d88d9a1ba46f0',
      '#weight' => '-50',
    ),
    'cdebf2c0350bb2cf9dd4c77a2b6774e7' => array(
      'name' => 'variable_set',
      'settings' => array(
        'name' => 'foo',
        'value' => 123,
        'value_type' => 'integer',
      ),
      '#id' => 'cdebf2c0350bb2cf9dd4c77a2b6774e7',
      '#weight' => '-49',
    ),
  );
  $export['demo_environment'] = $environment;

  return $export;
}
