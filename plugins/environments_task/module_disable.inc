<?php

/**
 * @file
 * Contains the "module_disable" Environments Task.
 */

$plugin = array(
  'admin_title' => t('Disable module(s)'),
  'task_callback' => 'environments_task_module_disable',
  'settings' => array(
    'modules' => array(),
    'disable_dependents' => TRUE,
  ),
  'settings_callback' => 'environments_task_module_disable_settings',
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
function environments_task_module_disable_settings(array $form_state) {
  $form = array();
  $task = isset($form_state['task']) ? $form_state['task'] : NULL;

  $selection = isset($form_state['values']['settings']['selection'])
    ? $form_state['values']['settings']['selection']
    : 'list';

  $default_value = isset($task['settings']['modules']) ? $task['settings']['modules'] : array();

  // Cast value.
  $default_value = _environments_cast_module_names($default_value, $selection);

  // Also cast form inputs when switching between selection modes.
  if (isset($form_state['values']['settings']['modules'])) {
    $ajax_modules = $form_state['values']['settings']['modules'];
    $ajax_selection = $form_state['values']['settings']['selection'];
    $ajax_modules = _environments_cast_module_names($ajax_modules, $ajax_selection);
  }

  $form['selection'] = array(
    '#title' => t('Selection mode'),
    '#description' => t('Selecting from a list is easier, but only modules that have already been downloaded are available. You may need to flush the caches to see newly-downloaded modules in the list.'),
    '#type' => 'select',
    '#default_value' => $selection,
    '#options' => array(
      'list' => t('Select from a list'),
      'custom' => t('Specify manually'),
    ),
    '#ajax' => array(
      'callback' => 'environments_ajax_task_settings_module_disable_name',
      'wrapper' => 'environments-module-names',
      'method' => 'replace',
      'effect' => 'slide',
    ),
  );

  $form['modules'] = array(
    '#prefix' => '<div id="environments-module-names">',
    '#suffix' => '</div>',
    '#name' => 'settings[modules]',
    '#title' => t('Modules'),
    '#default_value' => $default_value,
    '#element_validate' => array('_environments_task_module_validate'),
  );

  if ($selection === 'list') {
    $form['modules']['#type'] = 'select';
    $form['modules']['#multiple'] = TRUE;
    $form['modules']['#options'] = _environments_get_modules();
    $form['modules']['#empty_option'] = ' -' . t('Select modules') . '- ';

    if (isset($ajax_selection)) {
      $form['modules']['#value'] = $ajax_modules;
    }
  }
  elseif ($selection === 'custom') {
    $form['modules']['#type'] = 'textfield';
    $form['modules']['#maxlength'] = 255;
    $form['modules']['#size'] = 80;
    $form['modules']['#description'] = t("Separate multiple modules using a single space only. This is on you!");
    $form['modules']['#attributes']['placeholder'] = 'module_foo module_bar';
  }

  $form['disable_dependents'] = array(
    '#name' => 'settings[disable_dependents]',
    '#type' => 'checkbox',
    '#title' => t('Disable dependents'),
    '#description' => t('If TRUE, dependent modules will automatically be disabled in the correct order.'),
    '#default_value' => isset($task['settings']['disable_dependents']) ? $task['settings']['disable_dependents'] : TRUE,
  );

  return $form;
}

/**
 * Task callback.
 *
 * @param array $settings
 *   An array of settings for the task.
 * @param array $context
 *   An array that will contain information about the status of the batch.
 *   The values in $context will retain their values as the batch progresses.
 */
function environments_task_module_disable(array $settings, array &$context) {
  $modules = $settings['modules'];
  $disable_dependents = $settings['disable_dependents'];

  // Filter already-disabled modules.
  $messages = array();
  $modules_filtered = array();

  foreach ($modules as $module) {
    if (!module_exists($module)) {
      $messages[] = t('@module is already disabled.', array('@module' => $module));
    }
    else {
      $modules_filtered[] = $module;
    }
  }

  // Disable modules.
  if (!empty($modules_filtered)) {
    module_disable($modules_filtered, $disable_dependents);

    // Display this message before all others.
    array_unshift($messages, t('Disabled modules: @modules.', array('@modules' => implode(', ', array_values($modules_filtered)))));
  }

  // Display messages.
  $context['message'] = implode("\n", $messages);
}

/**
 * AJAX callback; updates the module selection input mode.
 *
 * @see environments_task_module_disable_settings()
 */
function environments_ajax_task_settings_module_disable_name($form, &$form_state) {
  return $form['settings']['modules'];
}
