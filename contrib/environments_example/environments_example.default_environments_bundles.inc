<?php
/**
 * @file
 * environments_example.default_environments_bundles.inc
 */

/**
 * Implements hook_default_environments_bundles().
 */
function environments_example_default_environments_bundles() {
  $export = array();

  $bundle = new stdClass();
  $bundle->disabled = FALSE; /* Edit this to true to make a default bundle disabled initially */
  $bundle->api_version = 1;
  $bundle->name = 'demo_bundle';
  $bundle->admin_title = 'Demo Bundle';
  $bundle->description = 'For demonstration purposes!';
  $bundle->tasks = '';
  $export['demo_bundle'] = $bundle;

  return $export;
}
