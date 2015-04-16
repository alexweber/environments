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

  $bundle = new stdClass();
  $bundle->disabled = FALSE; /* Edit this to true to make a default bundle disabled initially */
  $bundle->api_version = 1;
  $bundle->name = 'demo_modules';
  $bundle->admin_title = 'Dev Modules';
  $bundle->description = 'Enable dev modules & disable prod modules.';
  $bundle->tasks = array(
    '161c48b6cb3f2418f8bc061a1d3be2d6' => array(
      'name' => 'execute_bundle',
      'settings' => array(
        'name' => 'disable_demo_modules',
      ),
      '#id' => '161c48b6cb3f2418f8bc061a1d3be2d6',
      '#weight' => '-50',
    ),
    '249c8159a9319d1e1b55d75e776fdf38' => array(
      'name' => 'execute_bundle',
      'settings' => array(
        'name' => 'enable_demo_modules',
      ),
      '#id' => '249c8159a9319d1e1b55d75e776fdf38',
      '#weight' => '-49',
    ),
  );
  $export['demo_modules'] = $bundle;

  $bundle = new stdClass();
  $bundle->disabled = FALSE; /* Edit this to true to make a default bundle disabled initially */
  $bundle->api_version = 1;
  $bundle->name = 'demo_variables';
  $bundle->admin_title = 'Dev Variables';
  $bundle->description = 'Set variables for local development.';
  $bundle->tasks = array(
    '039f3eefc0332e5144f317e73c5ecd3c' => array(
      'name' => 'variable_set',
      'settings' => array(
        'name' => 'cache',
        'value' => FALSE,
        'value_type' => 'boolean',
      ),
      '#id' => '039f3eefc0332e5144f317e73c5ecd3c',
      '#weight' => '-50',
    ),
    'd779fc6d7d69e4aa718f3d762819b6fd' => array(
      'name' => 'variable_set',
      'settings' => array(
        'name' => 'cron_safe_threshold',
        'value' => 0,
        'value_type' => 'integer',
      ),
      '#id' => 'd779fc6d7d69e4aa718f3d762819b6fd',
      '#weight' => '-50',
    ),
    'dbc2a36bc77fc9a6c84b261c3e2b8d66' => array(
      'name' => 'variable_set',
      'settings' => array(
        'name' => 'jquery_update_jquery_cdn',
        'value' => 'none',
        'value_type' => 'string',
      ),
      '#id' => 'dbc2a36bc77fc9a6c84b261c3e2b8d66',
      '#weight' => '-50',
    ),
    'e4686923f6d7beddbfbaba4fdedf63fa' => array(
      'name' => 'variable_set',
      'settings' => array(
        'name' => 'preprocess_css',
        'value' => FALSE,
        'value_type' => 'boolean',
      ),
      '#id' => 'e4686923f6d7beddbfbaba4fdedf63fa',
      '#weight' => '-50',
    ),
    '3533ae53fb0cc1418061647bb1be49a3' => array(
      'name' => 'variable_set',
      'settings' => array(
        'name' => 'file_temporary_path',
        'value' => '/tmp',
        'value_type' => 'string',
      ),
      '#id' => '3533ae53fb0cc1418061647bb1be49a3',
      '#weight' => '-50',
    ),
    '166e8fd7c3a3178321c1215bb8ad3afa' => array(
      'name' => 'variable_set',
      'settings' => array(
        'name' => 'preprocess_js',
        'value' => FALSE,
        'value_type' => 'boolean',
      ),
      '#id' => '166e8fd7c3a3178321c1215bb8ad3afa',
      '#weight' => '-50',
    ),
    '47278075e21c23e8887a560e88c1c60b' => array(
      'name' => 'variable_set',
      'settings' => array(
        'name' => 'error_level',
        'value' => 2,
        'value_type' => 'integer',
      ),
      '#id' => '47278075e21c23e8887a560e88c1c60b',
      '#weight' => '-50',
    ),
  );
  $export['demo_variables'] = $bundle;

  $bundle = new stdClass();
  $bundle->disabled = FALSE; /* Edit this to true to make a default bundle disabled initially */
  $bundle->api_version = 1;
  $bundle->name = 'disable_demo_modules';
  $bundle->admin_title = 'Disable Production Modules';
  $bundle->description = 'Disable common production modules.';
  $bundle->tasks = array(
    '1a4d58336446411932374abc0d8c24d9' => array(
      'name' => 'module_disable',
      'settings' => array(
        'modules' => array(
          'entitycache' => 'entitycache',
          'memcache' => 'memcache',
          'memcache_admin' => 'memcache_admin',
          'securepages' => 'securepages',
          'update' => 'update',
        ),
        'disable_dependents' => TRUE,
      ),
      '#id' => '1a4d58336446411932374abc0d8c24d9',
      '#weight' => -50,
    ),
  );
  $export['disable_demo_modules'] = $bundle;

  $bundle = new stdClass();
  $bundle->disabled = FALSE; /* Edit this to true to make a default bundle disabled initially */
  $bundle->api_version = 1;
  $bundle->name = 'enable_demo_modules';
  $bundle->admin_title = 'Enable Dev Modules';
  $bundle->description = 'Enable common development modules.';
  $bundle->tasks = array(
    '50ffce3ade53769f678ffa89a6121ba0' => array(
      'name' => 'module_enable',
      'settings' => array(
        'modules' => array(
          'advanced_help' => 'advanced_help',
          'context_ui' => 'context_ui',
          'dblog' => 'dblog',
          'devel' => 'devel',
          'diff' => 'diff',
          'ds_ui' => 'ds_ui',
          'field_ui' => 'field_ui',
          'migrate_ui' => 'migrate_ui',
          'rules_admin' => 'rules_admin',
          'views_ui' => 'views_ui',
        ),
        'enable_dependencies' => TRUE,
      ),
      '#id' => '50ffce3ade53769f678ffa89a6121ba0',
      '#weight' => '-50',
    ),
  );
  $export['enable_demo_modules'] = $bundle;

  return $export;
}
