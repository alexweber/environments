<?php

/**
 * @file
 * Contains the "features_revert" Environments Task.
 */

$plugin = array(
  'admin_title' => t('Revert feature(s)'),
  'task_callback' => 'environments_task_features_revert',
  'settings' => array(
    'modules' => array(),
    'enable' => TRUE,
  ),
  'settings_callback' => 'environments_task_features_revert_settings',
  'summary_callback' => 'environments_task_features_revert_summary',
);

/**
 * Form callback; returns the settings form for this task.
 *
 * @param array $form_state
 *   A copy of the form_state array.
 *
 * @return array
 *   A Form API array of task settings.
 */
function environments_task_features_revert_settings(array $form_state) {
  $task = isset($form_state['task']) ? $form_state['task'] : NULL;
  $form = _environments_module_task_settings_form($task, $form_state);

  // This one is a bit different than the module_* tasks so change some stuff.
  $form['selection']['#description'] = $form['selection']['#selection'] . ' ' . t('<strong>Caution</strong>: This task will fail if the Features module is not enabled.');
  $form['modules']['#title'] = t('Features');

  $selection = isset($form_state['values']['settings']['selection'])
    ? $form_state['values']['settings']['selection']
    : 'list';

  if ($selection === 'list') {
    $form['modules']['#options'] = _environments_get_features();
    $form['modules']['#empty_option'] = ' -' . t('Select features') . '- ';
  }
  else {
    $form['modules']['#attributes']['placeholder'] = 'my_awesome_feature original_blog_feature';
  }

  $form['enable'] = array(
    '#name' => 'settings[enable]',
    '#type' => 'checkbox',
    '#title' => t('Enable Features'),
    '#description' => t("If checked, the Features module will be explicitly enabled. Otherwise, it's up to you to make sure it's enabled when this task runs! This task will fail if Features is not enabled."),
    '#default_value' => isset($task['settings']['enable']) ? $task['settings']['enable'] : TRUE,
  );

  return $form;
}

/**
 * Summary callback; format a custom summary of our task settings.
 *
 * @param array $settings
 *   An array of settings.
 *
 * @return array
 *   An array containing a short summary for each setting.
 */
function environments_task_features_revert_summary(array $settings) {
  $summary = array();

  $module_summary = _environments_module_task_summary($settings);
  $summary[] = '<strong>' . t('Features') . ':</strong> ' . $module_summary;
  $summary[] = '<strong>' . t('Enable Features module') . ':</strong> ' . ($settings['enable'] ? t('Yes') : t('No'));

  return $summary;
}

/**
 * Task callback.
 *
 * @param array $settings
 *   An array of settings for the task.
 * @param array $context
 *   An array that will contain information about the status of the batch. The
 *   values in $context will retain their values as the batch progresses.
 *
 * @throws Exception
 *   If the Features module is not enabled.
 */
function environments_task_features_revert(array $settings, array &$context) {
  $modules = $settings['modules'];
  $enable = $settings['enable'];
  $messages = array();

  if ($enable && !module_exists('features')) {
    module_enable(array('features'));
  }

  // Keep it simple.
  if (!function_exists('features_get_storage')) {
    throw new Exception(t('Features module must be enabled to execute the "features_revert" Environments task.'));
  }

  module_load_include('inc', 'features', 'features.export');

  foreach ($modules as $module) {
    if (module_exists($module)) {
      $storage = features_get_storage($module);
      if (in_array($storage, array(
        FEATURES_OVERRIDDEN,
        FEATURES_NEEDS_REVIEW,
        FEATURES_REBUILDABLE,
      ))) {
        if ($feature = features_load_feature($module, TRUE)) {
          $items[$module] = array();

          foreach (array_keys($feature->info['features']) as $component) {
            if (features_hook($component, 'features_revert')) {
              $items[$module][] = $component;
            }
          }

          features_revert($items);
          $messages[] = t('Reverted feature: @feature.', array('@feature' => $module));
        }
        else {
          $messages[] = t('Error loading feature: @feature.', array('@feature' => $module));
        }
      }
      else {
        $messages[] = t('Feature already matches default state: @feature.', array('@feature' => $module));
      }
    }
    else {
      $messages[] = t("Can't revert a disabled feature: @feature.", array('@feature' => $module));
    }
  }

  // Display messages.
  $context['message'] = implode(' ', $messages);
}


/**
 * Returns all available Drupal features.
 *
 * @return array
 *   An array of module names, keyed by machine_name.
 */
function _environments_get_features() {
  static $features_options = array();

  if (empty($features)) {
    $features = features_get_features();

    foreach ($features as $name => $info) {
      $features_options[$name] = $info->info['name'];
    }
  }

  return $features_options;
}
