<?php
/**
 * @file
 * Definition of the "variable_set" Env Task.
 */

$plugin = array(
  'admin_title' => t('Set variable values'),
  'task_callback' => 'env_task_variable_set',
  'settings' => array(
    'name' => '',
    'value' => '',
  ),
  'settings_callback' => 'env_task_variable_set_settings',
);

/**
 * Form callback.
 *
 * Returns the settings form for this task.
 *
 * This is not a typical form callback in that we don't get a $form or
 * $form_state.
 *
 * @param stdClass
 *   The current environment.
 * @param array
 *   The current task.
 *
 * @return array
 *   A Form API array of task settings.
 */
function env_task_variable_set_settings($environment, $task = NULL) {
  $form = array();

  $form['name'] = array(
    '#name' => 'settings[name]',
    '#type' => 'select',
    '#title' => t('Variable'),
    '#multiple' => FALSE,
    '#options' => drupal_map_assoc(_env_get_variables()),
    '#empty_option' => t(' -Select a variable- '),
    '#default_value' => isset($task['settings']['name']) ? $task['settings']['name'] : array(),
  );

  $form['value'] = array(
    '#name' => 'settings[value]',
    '#type' => 'textfield',
    '#title' => t('Value'),
    '#description' => t('Note: At the moment this task is limited to setting string values. You can use "1" and "0" for boolean variables too.'),
    '#default_value' => isset($task['settings']['value']) ? $task['settings']['value'] : array(),
  );

  return $form;
}

/**
 * Task callback.
 *
 * @param array
 *   An array of settings for the task.
 * @param array
 *   $context is an array that will contain information about the
 *   status of the batch. The values in $context will retain their
 *   values as the batch progresses.
 */
function env_task_variable_set($settings, &$context) {
  $name = $settings['name'];
  $value = $settings['value'];

  variable_set($name, $value);

  // Display messages.
  $context['message'] = t('Set variable value @name to @value', array('@name' => $name, '@value' => $value));
}
