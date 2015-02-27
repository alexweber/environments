<?php
/**
 * @file
 * environments_example.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function environments_example_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "environments" && $api == "default_environments_environments") {
    return array("version" => "1");
  }
  if ($module == "environments_bundles" && $api == "default_environments_bundles") {
    return array("version" => "1");
  }
}
