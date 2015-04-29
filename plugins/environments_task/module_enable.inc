<?php

/**
 * @file
 * Contains the "module_enable" Environments Task.
 */

/**
 * Define this Task plugin.
 */
$plugin = array(
  // Title for Admin UI.
  'admin_title' => t('Enable module(s)'),
  // Batch API callback to execute. You can also define this using the
  // alternative method below.
  'task_callback' => 'environments_task_module_enable',
  // Batch API callback to execute. This is an alternative implementation that
  // gives you more control over where your callback lives and what it's called.
  // Which callback to use is automatically determined by CTools.
//  'task_callback' => array(
//    'file' => '',
//    'path' => '',
//    'function' => '',
//  ),

  // Define any settings your task might implement.
  // It's ok to have no settings. If, however, you *do* have some, make sure
  // you create a settings_callback to allow these values to be changed.
  'settings' => array(
    'modules' => array(),
    'enable_dependencies' => TRUE,
  ),
  // Optional Form callback for settings form. You can also define this using
  // a similar alternative method as with the batch callbacks.
  'settings_callback' => 'environments_task_module_enable_settings',
  // Optional Form callback for settings form. This is the alternative method.
  // Refer to the explanation for "task_callback" above for more information.
//  'settings_callback' => array(
//    'file' => '',
//    'path' => '',
//    'function' => '',
//  ),
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
function environments_task_module_enable_settings(array $form_state) {
  $form = array();
  $task = isset($form_state['task']) ? $form_state['task'] : NULL;

  $selection = isset($form_state['values']['settings']['selection'])
    ? $form_state['values']['settings']['selection']
    : 'list';

  $default_value = isset($task['settings']['modules']) ? $task['settings']['modules'] : array();

  // Cast value.
  _environments_cast_module_names($default_value, $selection);
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
      'callback' => 'environments_ajax_task_settings_module_enable_name',
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
  );

  if ($selection === 'list') {
    $form['modules']['#type'] = 'select';
    $form['modules']['#multiple'] = TRUE;
    $form['modules']['#options'] = _environments_get_modules();
    $form['modules']['#empty_option'] = ' -' . t('Select modules') . '- ';
  }
  elseif ($selection === 'custom') {
    $form['modules']['#type'] = 'textfield';
    $form['modules']['#maxlength'] = 255;
    $form['modules']['#size'] = 80;
    $form['modules']['#description'] = t("Separate multiple modules using a single space only. This is on you!");
  }

  $form['enable_dependencies'] = array(
    '#name' => 'settings[enable_dependencies]',
    '#type' => 'checkbox',
    '#title' => t('Enable dependencies'),
    '#description' => t('If TRUE, dependent modules will automatically be enabled in the correct order.'),
    '#default_value' => isset($task['settings']['enable_dependencies']) ? $task['settings']['enable_dependencies'] : TRUE,
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
function environments_task_module_enable(array $settings, array &$context) {
  $modules = $settings['modules'];
  $enable_dependencies = $settings['enable_dependencies'];

  // Filter already-enabled modules.
  $messages = array();
  $modules_filtered = array();

  foreach ($modules as $module) {
    if (module_exists($module)) {
      $messages[] = t('@module is already enabled.', array('@module' => $module));
    }
    else {
      $modules_filtered[] = $module;
    }
  }

  // Enable modules.
  if (!empty($modules_filtered)) {
    if (module_enable($modules_filtered, $enable_dependencies)) {
      // Display this message before all others.
      array_unshift($messages, t('Enabled modules: @modules.', array('@modules' => implode(', ', array_values($modules_filtered)))));
    }
  }

  // Display messages.
  $context['message'] = implode("\n", $messages);
}

/**
 * AJAX callback; updates the variable name field according to selection mode.
 *
 * @see environments_task_module_enable_settings()
 */
function environments_ajax_task_settings_module_enable_name($form, &$form_state) {
  return $form['settings']['modules'];
}
