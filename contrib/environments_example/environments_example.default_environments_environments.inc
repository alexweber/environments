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
  $environment->tasks = '';
  $export['demo_environment'] = $environment;

  return $export;
}
