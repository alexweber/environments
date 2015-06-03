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
  $bundle->tasks = array(
    '83a37b8ce1467ce9a21ede8c040ed28f' => array(
      'name' => 'module_enable',
      'settings' => array(
        'modules' => array(
          'color' => 'color',
        ),
        'enable_dependencies' => TRUE,
        'selection' => 'list',
      ),
      '#id' => '83a37b8ce1467ce9a21ede8c040ed28f',
      '#weight' => '-50',
    ),
    'e7db93f457a3ee80b0a1f199c1bebc46' => array(
      'name' => 'variable_set',
      'settings' => array(
        'name' => 'bar',
        'value' => 'baz',
        'value_type' => 'string',
      ),
      '#id' => 'e7db93f457a3ee80b0a1f199c1bebc46',
      '#weight' => '-49',
    ),
    '37bccbcc15ee061898a6f78f681517ff' => array(
      'name' => 'module_disable',
      'settings' => array(
        'modules' => array(
          'color' => 'color',
        ),
        'disable_dependents' => TRUE,
        'selection' => 'list',
      ),
      '#id' => '37bccbcc15ee061898a6f78f681517ff',
      '#weight' => '-48',
    ),
    '1041056275bff455e16b3d07fd939fec' => array(
      'name' => 'variable_del',
      'settings' => array(
        'name' => 'bar',
      ),
      '#id' => '1041056275bff455e16b3d07fd939fec',
      '#weight' => '-47',
    ),
  );
  $export['demo_bundle'] = $bundle;

  return $export;
}
