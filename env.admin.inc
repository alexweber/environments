<?php
/**
 * @file
 * Admin pages for Env.
 */

/**
 * Form builder; Form to edit a given environment.
 */
function env_form_environment_edit($form, &$form_state) {
  if (!empty($form_state['environment'])) {
    $environment = $form_state['environment'];
  }
  else {
    $environment = env_environment_create();
  }

  // Title
  $form['admin_title'] = array(
    '#type' => 'textfield',
    '#maxlength' => '255',
    '#title' => t('Title'),
    '#description' => t('A human-readable title for this environment.'),
    '#required' => TRUE,
    '#default_value' => $environment->admin_title,
  );

  // Machine name.
  $form['name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => '255',
    '#machine_name' => array(
      'source' => array('admin_title'),
      'exists' => 'env_environment_exists',
    ),
    '#required' => TRUE,
    '#default_value' => $environment->name,
  );

  // Alias.
  $form['alias'] = array(
    '#type' => 'textfield',
    '#title' => t('Alias'),
    '#description' => t('An alias for this environment, to use with Drush. Must only contain letters.'),
    '#default_value' => $environment->alias,
    '#size' => 8,
  );

  // Settings vertical tab group.
  $form['settings'] = array(
    '#title' => t('Settings'),
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#collapsible' => FALSE,
  );

  // Notify when environment is active.
  $form['settings']['notify'] = array(
    '#type' => 'checkbox',
    '#title' => t('Notify when this environment is active.'),
    '#description' => t('Displays a small notification banner across the the top of the site.'),
    '#default_value' => $environment->settings['notify'],
  );

  return $form;
}

/**
 * Form builder; Form to delete a given environment.
 */
function env_environment_form_delete($form, &$form_state, $environment) {
  $form_state['environment'] = &$environment;

  // Deleting an export in code will revert it.
  $op = ($environment->export_type & EXPORT_IN_CODE) ? 'Revert' : 'Delete';

  return confirm_form(
    $form,
    t('Are you sure you want to @action the environment %admin_title?', array('@action' => t(drupal_strtolower($op)), '%admin_title' => $environment->admin_title)),
    'admin/config/development/env',
    NULL,
    t($op),  t('Cancel')
  );
}

/**
 * Submit handler for deleting an environment.
 */
function env_environment_form_delete_submit($form, &$form_state) {
  $environment = &$form_state['environment'];
  $op = ($environment->export_type & EXPORT_IN_CODE) ? 'reverted' : 'deleted';

  ctools_include('export');
  ctools_export_crud_delete('env_environment', $environment);

  drupal_set_message(t('Environment %admin_title was ' . $op . '.', array('%admin_title' => $environment->admin_title)));
  $form_state['redirect'] = 'admin/config/development/env';
}

/**
 * Form callback.
 *
 * Displays task overview form.
 *
 * @see env_menu()
 */
function env_form_tasks($form, &$form_state, $environment) {
  if (is_string($environment)) {
    $environment = env_environment_load($environment);
  }

  $form_state['environment'] = $environment;

  // List tasks.
  if (is_object($environment) && !empty($environment->tasks)) {
    $all_tasks = env_get_tasks();

    $form['tasks'] = array(
      '#tree' => TRUE,
    );

    foreach (element_children($environment->tasks) as $id) {
      $task = $environment->tasks[$id];

      // Prepare settings summary.
      $settings_summary = array();
      foreach ($task['settings'] as $name => $value) {
        if ($value === TRUE) {
          $value = '1';
        }
        else if ($value === FALSE) {
          $value = '0';
        }
        else if (is_array($value)) {
          $value = implode(', ', $value);
        }

        $settings_summary[] = "<strong>{$name}:</strong> {$value}";
      }

      ctools_include('export');
      $plugin = ctools_get_export_ui('env_ctools_export_ui');
      $path = ctools_export_ui_plugin_menu_path($plugin, 'tasks', $environment->name);
      $destination = drupal_get_destination();

      // Prepare for Draggable Table.
      $form['tasks'][$id] = array(
        'task' => array(
          '#markup' => isset($all_tasks[$task['name']]) ? $all_tasks[$task['name']]['admin_title'] : $task['name'],
        ),
        'settings' => array(
          '#markup' => implode('<br>', $settings_summary),
        ),
        'weight' => array(
          '#type' => 'weight',
          '#title' => t('Weight'),
          '#default_value' => $task['#weight'],
          '#delta' => 50,
          '#title-display' => 'invisible',
        ),
        'op' => array(
          '#theme' => 'links',
          '#links' => array(
            'edit' => array(
              'title' => t('edit'),
              'href' => "{$path}/{$id}/edit",
              'query' => $destination,
            ),
            'delete' => array(
              'title' => t('delete'),
              'href' => "{$path}/{$id}/delete",
              'query' => $destination,
            ),
          ),
        ),
      );
    }
  }
  else {
    $form['tasks'] = array(
      '#markup' => '<div class="empty-text">' . t('There are no tasks set up yet.') . '</div>',
    );
  }

  // Don't allow the form to be submitted if we have no tasks.
  if (count($form['tasks']) > 1) {
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update Order'),
    );
  }

  return $form;
}

/**
 * Submit callback.
 *
 * Saves updated order for Tasks.
 *
 * @see env_form_tasks()
 */
function env_form_tasks_submit($form, &$form_state) {
  $environment = $form_state['environment'];

  foreach ($form_state['values']['tasks'] as $id => $data) {
    $environment->tasks[$id]['#weight'] = $data['weight'];
  }

  // Sort tasks by new weights.
  uasort($environment->tasks, 'element_sort');

  if (env_environment_save($environment)) {
    drupal_set_message(t('Task order updated!'), 'status', FALSE);
  }
  else {
    drupal_set_message(t('Error updating task order.'), 'error', FALSE);
    watchdog('env', 'Error updating task order for environment: @name', array('@name' => $environment->admin_title), WATCHDOG_ERROR);
  }

}

/**
 * Form callback.
 *
 * Displays delete task form.
 */
function env_form_tasks_delete($form, &$form_state, $environment, $task) {
  $form_state['environment'] = $environment;
  $form_state['task'] = $task;

  return confirm_form(
    $form,
    t('Are you sure you want to delete this task?'),
    'admin/config/development/env',
    NULL,
    t('Delete'),  t('Cancel')
  );
}

/**
 * Submit callback.
 *
 * @see env_form_tasks_delete()
 */
function env_form_tasks_delete_submit($form, &$form_state) {
  $environment = $form_state['environment'];
  $task = $form_state['task'];

  if (array_key_exists($task, $environment->tasks)) {
    unset($environment->tasks[$task]);
    if (env_environment_save($environment)) {
      drupal_set_message(t('Task deleted!'), 'status', FALSE);
      return;
    }
  }

  drupal_set_message(t('Error deleting task.'), 'error', FALSE);
  watchdog('env', 'Error deleting task: @name', array('@name' => $task), WATCHDOG_ERROR);
}

/**
 * Form callback.
 *
 * Displays add/edit task form.
 */
function env_form_tasks_edit($form, &$form_state, $environment, $task = NULL) {
  // Store environment in form state for AJAX.
  $form_state['environment'] = $environment;
  $current_task = !empty($form_state['values']['task']) ? $form_state['values']['task'] : '';

  // Prepare task info if editing.
  if (isset($task) && array_key_exists($task, $environment->tasks)) {
    // Grab task instance info.
    $task_info = $environment->tasks[$task];
    // Load task plugin.
    $task = env_get_task($task_info['name']);
    // Override with instance info.
    $task = array_merge($task, $task_info);
    // Store task in form state for AJAX.
    $form_state['task'] = $task;
  }

  // Prepare task type options.
  $task_types = array();
  foreach (env_get_tasks() as $type) {
    $task_types[$type['name']] = $type['admin_title'];
  }

  // Pick a task.
  // Default task can either com from submitted values or from form_state.
  if (isset($form_state['values']['task'])) {
    $default_task = $form_state['values']['task'];
  }
  else if (isset($form_state['task'])) {
    $default_task = $form_state['task']['name'];
  }
  else {
    $default_task = '';
  }

  $form['task'] = array(
    '#name' => 'task',
    '#type' => 'select',
    '#title' => t('Task'),
    '#description' => t('Select a task to run, you will be able to configure options for it later on.'),
    '#required' => TRUE,
    '#empty_option' => t(' -Select a task- '),
    '#default_value' => $default_task,
    '#options' => $task_types,
    '#ajax' => array(
      'callback' => 'env_ajax_task_settings',
      'wrapper' => 'env-task-settings',
      'method' => 'replace',
      'effect' => 'slide',
    ),
  );

  $form['settings'] = array(
    '#name' => 'settings',
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#tree' => TRUE,
    '#prefix' => '<div id="env-task-settings">',
    '#suffix' => '</div>',
    '#states' => array(
     'visible' => array(
       'select[name="task"]' => array('empty' => FALSE),
     ),
    ),
    '#attached' => array(
      'js' => array(
        drupal_get_path('module', 'env') . '/js/env_admin.js',
      ),
    ),
  );

  // Build settings form.
  if (isset($form_state['task'])) {
    if ($callback = ctools_plugin_get_function($form_state['task'], 'settings_callback')) {
      $settings_form = call_user_func($callback, $form_state['environment'], $form_state['task']);
      $form['settings'] += $settings_form;
    }
    else {
      $form['settings'] += array(
        '#description' => t('This task has no settings.'),
      );
    }
  }
  else if ($current_task) {
    if ($task_info = env_get_task($current_task)) {
      if ($callback = ctools_plugin_get_function($task_info, 'settings_callback')) {
        $settings_form = call_user_func($callback, $form_state['environment']);
        $form['settings'] += $settings_form;
      }
      else {
        $form['settings'] += array(
          '#description' => t('This task has no settings.'),
        );
      }
    }
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * AJAX Callback.
 *
 * Returns task settings form.
 */
function env_ajax_task_settings($form, &$form_state) {
  return $form['settings'];
}

/**
 * Submit callback.
 *
 * @see env_form_tasks_edit()
 */
function env_form_tasks_edit_submit($form, &$form_state) {
  $environment = $form_state['environment'];
  $new_task = $form_state['values']['task'];
  $new_settings = array_key_exists('settings', $form_state['values'])
    ? $form_state['values']['settings']
    : array();

  // Edit.
  if (array_key_exists('task', $form_state)) {
    $id = $form_state['task']['#id'];
    $environment->tasks[$id]['name'] = $new_task;
    $environment->tasks[$id]['settings'] = $new_settings;

    if (env_environment_save($environment)) {
      drupal_set_message(t('Task Updated'), 'status', FALSE);
      return;
    }

    drupal_set_message(t('Error updating task'), 'error', FALSE);
    watchdog('env', 'Error updating task: @name', array('@name' => $id), WATCHDOG_ERROR);
  }
  // Add.
  else {
    if ($task = env_get_task($new_task)) {
      // Merge settings with defaults.
      $settings = array_merge($task['settings'], $new_settings);

      // Generate unique ID.
      $id = md5($task['name'] . serialize($settings) . time());

      // Add new task to environment.
      $environment->tasks[$id] = array(
        'name' => $task['name'],
        'settings' => $settings,
        '#id' => $id,
        '#weight' => 0,
      );

      if (env_environment_save($environment)) {
        drupal_set_message(t('Task Added'), 'status', FALSE);
        return;
      }
    }

    drupal_set_message(t('Error adding task: @name', array('@name' => $task['name'])), 'error', FALSE);
    watchdog('env', 'Error adding task: @name', array('@name' => $task['name']), WATCHDOG_ERROR);
  }
}

/**
 * Form callback.
 *
 * Displays the "Site Environment" form.
 *
 * @see env_menu()
 */
function env_form_site_environment($form) {
  $environments_options = array();
  $environments = env_environment_load_all();

  foreach ($environments as $env) {
    $environments_options[$env->name] = $env->admin_title;
  }

  $form['env'] = array(
    '#title' => t('Current environment'),
    '#description' => t(''),
    '#type' => 'select',
    '#options' => $environments_options,
    '#empty_option' => t('- None -'),
    '#default_value' => variable_get('env'),
  );

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
  );

  $form['advanced']['env_css'] = array(
    '#title' => t('Include default CSS'),
    '#type' => 'checkbox',
    '#description' => t('Disable this if you want to write your own notification banner styles'),
    '#default_value' => variable_get('env_css', TRUE),
  );

  $form['advanced']['env_js'] = array(
    '#title' => t('Include default JS'),
    '#type' => 'checkbox',
    '#description' => t('Disable this if you use a Zurb Foundation sub-theme.'),
    '#default_value' => variable_get('env_js', TRUE),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit callback.
 *
 * @see env_form_site_environment()
 */
function env_form_site_environment_submit($form, &$form_state) {
  $new_env = $form_state['values']['env'];
  $old_env = variable_get('env');

  // Un-set environment if none selected.
  if (empty($new_env)) {
    variable_set('env', '');
  }
  // If there was actually an environment change, make it happen.
  else if ($new_env !== $old_env) {
    env_set($new_env);
  }

  // Persist CSS/JS settings.
  variable_set('env_css', $form_state['values']['env_css']);
  variable_set('env_js', $form_state['values']['env_js']);
}

/**
 * Returns all available Drupal modules. Used by the "module_enable" and
 * "module_disable" tasks. This is as a lightweight replacement for using
 * Drupal core APIs.
 *
 * @return array
 *   An array of module names, keyed by machine_name.
 */
function _env_get_modules() {
  static $modules = array();

  if (empty($modules)) {
    // No value added by using DBTNG.
    $result = db_query("SELECT name, info FROM {system} ORDER BY name ASC");
    foreach ($result->fetchAll() as $row) {
      $info = unserialize($row->info);

      if (!isset($info['hidden']) || !$info['hidden']) {
        $modules[$row->name] = $info['name'];
      }
    }
  }

  return $modules;
}

/**
 * Returns all defined Drupal variables. Used by the "variable_set" and
 * "variable_del" tasks. This is as a lightweight replacement for using Drupal
 * core APIs.
 *
 * @return array
 *   An array of variable names.
 */
function _env_get_variables() {
  static $variables = array();

  if (empty($variables)) {
    // No value added by using DBTNG.
    $result = db_query("SELECT name FROM {variable} ORDER BY name ASC");
    $result = $result->fetchAllAssoc('name');
    $variables = array_keys($result);
  }

  return $variables;
}

/**
 * Returns all available Drupal features. Used by the "features_revert". This
 * is as a convenience function to prepare a list of features for a "select"
 * Form API element.
 *
 * @return array
 *   An array of module names, keyed by machine_name.
 */
function _env_get_features() {
  static $features_options = array();

  if (empty($features)) {
    $features = features_get_features();

    foreach ($features as $name => $info) {
      $features_options[$name] = $info->info['name'];
    }
  }

  return $features_options;
}
