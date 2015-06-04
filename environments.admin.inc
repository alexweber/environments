<?php

/**
 * @file
 * Admin pages.
 */

/**
 * Form builder; Form to delete a given environment.
 */
function environments_form_delete($form, &$form_state, $environment) {
  $form_state['environment'] = &$environment;

  // Deleting an export in code will revert it.
  $op = ($environment->export_type & EXPORT_IN_CODE) ? t('Revert') : t('Delete');

  return confirm_form(
    $form,
    t('Are you sure you want to @action the environment %admin_title?', array(
      '@action' => drupal_strtolower($op),
      '%admin_title' => $environment->admin_title,
    )),
    'admin/config/development/environments',
    NULL,
    $op, t('Cancel')
  );
}

/**
 * Submit handler for deleting an environment.
 */
function environments_form_delete_submit($form, &$form_state) {
  $environment = &$form_state['environment'];
  $op = ($environment->export_type & EXPORT_IN_CODE) ? t('reverted') : t('deleted');

  ctools_include('export');
  ctools_export_crud_delete('environments', $environment);

  drupal_set_message(t('Environment %admin_title was %op.', array(
    '%admin_title' => $environment->admin_title,
    '%op' => $op,
  )));
  $form_state['redirect'] = 'admin/config/development/environments';
  unset($_GET['destination']);
}

/**
 * Form callback.
 *
 * Displays task overview form.
 *
 * @see environments_menu()
 */
function environments_task_form($form, &$form_state, $object) {
  if ($object->table === 'environments_bundles') {
    $load_callback = 'environments_bundles_load';
    $export_ui_name = 'environments_bundles';
  }
  else {
    $load_callback = 'environments_load';
    $export_ui_name = 'environments';
  }

  if (is_string($object)) {
    $object = call_user_func($load_callback, $object);
  }

  $form_state['object'] = $object;

  ctools_include('export');
  $plugin = ctools_get_export_ui($export_ui_name);

  // Set better page title.
  $page_title = t('Manage Tasks !type @name', array(
    '!type' => $plugin['title singular proper'],
    '@name' => $object->admin_title,
  ));
  drupal_set_title($page_title, PASS_THROUGH);

  // List tasks.
  if (is_object($object) && !empty($object->tasks)) {
    $all_tasks = environments_get_tasks();

    $form['tasks'] = array(
      '#tree' => TRUE,
    );

    foreach (element_children($object->tasks) as $id) {
      $task = $object->tasks[$id];

      // Prevent warnings.
      $plugin_def = isset($all_tasks[$task['name']])
        ? $all_tasks[$task['name']]
        : NULL;

      // If the task has a custom summary callback, use it.
      if ($callback = ctools_plugin_get_function($plugin_def, 'summary_callback')) {
        $settings_summary = (array) call_user_func($callback, $task['settings']);
      }
      // If not, generate a default summary as best we can.
      else {
        $settings_summary = environments_default_summary($task);
      }

      $path = ctools_export_ui_plugin_menu_path($plugin, 'tasks', $object->name);
      $destination = drupal_get_destination();

      // Prepare operations links.
      $operations = array(
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
      );

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
          '#theme' => 'links__ctools_dropbutton',
          '#links' => $operations,
          '#attributes' => array('class' => array('links', 'inline')),
        ),
      );
    }
  }
  else {
    $form['tasks'] = array(
      '#markup' => '<div class="empty-text">' . t('There are no tasks set up yet. Click "Add Task" to get started!') . '</div>',
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
 * @see environments_task_form()
 */
function environments_task_form_submit($form, &$form_state) {
  $object = $form_state['object'];

  foreach ($form_state['values']['tasks'] as $id => $data) {
    $object->tasks[$id]['#weight'] = $data['weight'];
  }

  // Sort tasks by new weights.
  uasort($object->tasks, 'element_sort');

  $save_callback = _environments_get_callback($object);

  if ($save_callback($object)) {
    drupal_set_message(t('Task order updated!'), 'status', FALSE);
  }
  else {
    drupal_set_message(t('Error updating task order.'), 'error', FALSE);
    watchdog('environments', 'Error updating task order for: @name', array('@name' => $object->admin_title), WATCHDOG_ERROR);
  }
}

/**
 * Form callback.
 *
 * Displays delete task form.
 */
function environments_task_form_delete($form, &$form_state, $object, $task) {
  $form_state['object'] = $object;
  $form_state['task'] = $task;

  return confirm_form(
    $form,
    t('Are you sure you want to delete this task?'),
    'admin/config/development/environments',
    NULL,
    t('Delete'), t('Cancel')
  );
}

/**
 * Submit callback.
 *
 * @see environments_task_form_delete()
 */
function environments_task_form_delete_submit($form, &$form_state) {
  $object = $form_state['object'];
  $task = $form_state['task'];

  if (array_key_exists($task, $object->tasks)) {
    unset($object->tasks[$task]);
    $save_callback = _environments_get_callback($object);

    if ($save_callback($object)) {
      $path = _environments_get_path($object);
      drupal_set_message(t('Task deleted!'), 'status', FALSE);
      $form_state['redirect'] = "{$path}/tasks";
      unset($_GET['destination']);
      return;
    }
  }

  drupal_set_message(t('Error deleting task.'), 'error', FALSE);
  watchdog('environments', 'Error deleting task: @name', array('@name' => $task), WATCHDOG_ERROR);
}

/**
 * Form callback.
 *
 * Displays add/edit task form.
 */
function environments_task_form_edit($form, &$form_state, $object, $task = NULL) {
  // Attach our custom styles.
  $form['#attached']['css'][] = drupal_get_path('module', 'environments') . '/assets/css/environments.admin.css';

  // Store object in form state for AJAX.
  $form_state['object'] = $object;
  $current_task = !empty($form_state['values']['task']) ? $form_state['values']['task'] : '';

  // Load plugin definition.
  ctools_include('plugins');
  $plugin = ctools_get_plugins('ctools', 'export_ui', $object->table);

  // Set better page title.
  $op = isset($task) ? t('Edit Task') : t('Add Task');
  $page_title = t('!op !type @name', array(
    '!op' => $op,
    '!type' => $plugin['title singular proper'],
    '@name' => $object->admin_title,
  ));
  drupal_set_title($page_title, PASS_THROUGH);

  // Prepare task info if editing.
  if (isset($task) && array_key_exists($task, $object->tasks)) {
    // Grab task instance info.
    $task_info = $object->tasks[$task];
    // Load task plugin.
    $task = environments_get_task($task_info['name']);
    // Override with instance info.
    $task = array_merge($task, $task_info);
    // Store task in form state for AJAX.
    $form_state['task'] = $task;
  }

  // Prepare task type options.
  $task_types = array();
  foreach (environments_get_tasks() as $type) {
    $task_types[$type['name']] = $type['admin_title'];
  }

  // Sort tasks alphabetically.
  asort($task_types);

  // Pick a task.
  // Default task can either com from submitted values or from form_state.
  if (isset($form_state['values']['task'])) {
    $default_task = $form_state['values']['task'];
  }
  else {
    if (isset($form_state['task'])) {
      $default_task = $form_state['task']['name'];
    }
    else {
      $default_task = '';
    }
  }

  $form['task'] = array(
    '#name' => 'task',
    '#type' => 'select',
    '#title' => t('Task'),
    '#description' => t('Select a task.'),
    '#required' => TRUE,
    '#empty_option' => t('- None -'),
    '#default_value' => $default_task,
    '#options' => $task_types,
    '#attributes' => array(
      'class' => array('env-task-select'),
    ),
    '#ajax' => array(
      'callback' => 'environments_ajax_task_settings',
      'wrapper' => 'environments-task-settings',
      'method' => 'replace',
      'effect' => 'slide',
    ),
  );

  $form['settings'] = array(
    '#name' => 'settings',
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#tree' => TRUE,
    '#prefix' => '<div id="environments-task-settings" class="env-task-settings">',
    '#suffix' => '</div>',
    '#states' => array(
      'visible' => array(
        'select[name="task"]' => array('empty' => FALSE),
      ),
    ),
  );

  // Build settings form.
  if (isset($form_state['task'])) {
    if ($callback = ctools_plugin_get_function($form_state['task'], 'settings_callback')) {
      // Invoke task's settings form callback.
      $form['settings'] += call_user_func($callback, $form_state);
    }
    else {
      $form['settings'] += array(
        '#description' => t('This task has no settings.'),
      );
    }
  }
  elseif ($current_task) {
    if ($task_info = environments_get_task($current_task)) {
      if ($callback = ctools_plugin_get_function($task_info, 'settings_callback')) {
        // Invoke task's settings form callback.
        $form['settings'] += call_user_func($callback, $form_state);
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
    '#value' => t('Save task'),
  );
  $form['actions']['submit_another'] = array(
    '#type' => 'submit',
    '#value' => t('Save and add another'),
  );

  // Figure out path depending on what type of object we're editing.
  $path = 'admin/config/development/environments';
  if ($object->table === 'environments_bundles') {
    $path .= '-bundles';
  }
  $path .= "/list/{$object->name}/tasks";

  $form['actions']['cancel'] = array(
    '#theme' => 'link',
    '#text' => t('Cancel'),
    '#path' => $path,
    '#options' => array(
      'attributes' => array(),
      'html' => FALSE,
      'absolute' => TRUE,
    ),
  );

  $form['actions']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#limit_validation_errors' => array(),
    '#submit' => array('environments_task_form_edit_submit_delete'),
  );

  $form['#submit'][] = 'environments_task_form_edit_submit';
  $form['#submit'][] = 'environments_task_form_edit_submit_redirect';

  return $form;
}

/**
 * AJAX Callback.
 *
 * Returns task settings form.
 *
 * @see environments_task_form_edit()
 */
function environments_ajax_task_settings($form, &$form_state) {
  return $form['settings'];
}

/**
 * Submit callback.
 *
 * @see environments_task_form_edit()
 */
function environments_task_form_edit_submit($form, &$form_state) {
  $object = $form_state['object'];
  $new_task = $form_state['values']['task'];
  $save_callback = _environments_get_callback($object);

  $new_settings = array_key_exists('settings', $form_state['values'])
    ? $form_state['values']['settings']
    : array();

  // Cast boolean values so that we can store them properly and display them
  // accordingly on summaries.
  foreach ($new_settings as $key => $value) {
    if (isset($form['settings'][$key]) && $form['settings'][$key]['#type'] === 'checkbox') {
      $new_settings[$key] = (bool) $value;
    }
  }

  // Edit.
  if (array_key_exists('task', $form_state)) {
    $id = $form_state['task']['#id'];
    $object->tasks[$id]['name'] = $new_task;
    $object->tasks[$id]['settings'] = $new_settings;

    if ($save_callback($object)) {
      drupal_set_message(t('Task Updated'), 'status', FALSE);
      return;
    }

    drupal_set_message(t('Error updating task'), 'error', FALSE);
    watchdog('environments', 'Error updating task: @name', array('@name' => $id), WATCHDOG_ERROR);
  }
  // Add.
  else {
    if ($task = environments_get_task($new_task)) {
      // Merge settings with defaults.
      $settings = array_merge($task['settings'], $new_settings);

      // Generate unique ID.
      $id = md5($task['name'] . serialize($settings) . time());

      // Find the next highest weight for our new task.
      $weight = -50;
      if ($object->tasks) {
        foreach ($object->tasks as $t) {
          if ($t['#weight'] >= $weight) {
            $weight = ++$t['#weight'];
          }
        }
      }

      // Add new task to object.
      $object->tasks[$id] = array(
        'name' => $task['name'],
        'settings' => $settings,
        '#id' => $id,
        '#weight' => $weight,
      );

      if ($save_callback($object)) {
        drupal_set_message(t('Task Added'), 'status', FALSE);
        return;
      }
    }

    drupal_set_message(t('Error adding task: @name', array('@name' => $task['name'])), 'error', FALSE);
    watchdog('environments', 'Error adding task: @name', array('@name' => $task['name']), WATCHDOG_ERROR);
  }
}

/**
 * Submit callback.
 *
 * @see environments_task_form_edit()
 */
function environments_task_form_edit_submit_redirect($form, &$form_state) {
  $object = $form_state['object'];
  $path = _environments_get_path($object);
  $form_state['redirect'] = "{$path}/tasks";

  if ($form_state['values']['op'] === $form_state['values']['submit_another']) {
    $form_state['redirect'] .= '/add';
    unset($_GET['destination']);
  }
}

/**
 * Submit callback.
 *
 * @see environments_task_form_edit()
 */
function environments_task_form_edit_submit_delete($form, &$form_state) {
  $object = $form_state['object'];
  $task = $form_state['task'];
  $path = _environments_get_path($object);
  $form_state['redirect'] = array(
    "{$path}/tasks/{$task['#id']}/delete",
    array(
      'query' => array(
        'destination' => "{$path}/tasks/{$task['#id']}/edit",
      ),
    ),
  );
  unset($_GET['destination']);
}

/**
 * Helper function to determine the path to an object.
 *
 * @param object $object
 *   Either an Environment or an Environment Bundle object.
 *
 * @return string
 *   The admin path to that object.
 *
 * @see environments_task_form_edit_submit_redirect()
 * @see environments_task_form_edit_submit_delete()
 */
function _environments_get_path($object) {
  $path = 'admin/config/development/environments';

  // Figure out path depending on what type of object we're editing.
  if ($object->table === 'environments_bundles') {
    $path .= '-bundles';
  }

  return "{$path}/list/{$object->name}";
}

/**
 * Helper function to determine an object's save callback.
 *
 * @param object $object
 *   Either an Environment or an Environment Bundle object.
 *
 * @return string
 *   The callback name.
 *
 * @see environments_task_form_submit()
 * @see environments_task_form_edit_submit()
 */
function _environments_get_callback($object) {
  return $object->table === 'environments_bundles'
    ? 'environments_bundles_save'
    : 'environments_save';
}

/**
 * Form callback.
 *
 * Displays the "Site Environment" form.
 *
 * @see environments_menu()
 */
function environments_form_site($form) {
  $environments = environments_load_all();

  // Format options.
  $environments_options = array();
  foreach ($environments as $env) {
    $environments_options[$env->name] = $env->admin_title;
  }

  // Current site environment.
  $form['environment'] = array(
    '#title' => t('Current environment'),
    '#type' => 'select',
    '#options' => $environments_options,
    '#empty_option' => t('- None -'),
    '#empty_value' => '',
    '#default_value' => variable_get('environments_env', ''),
  );

  if (empty($environments_options)) {
    $form['environment']['#description'] = t("You haven't added any environments yet! Click <a href='!url'>here</a> to add one.", array('!url' => url('admin/config/development/environments/add', array('absolute' => TRUE))));
    $form['environment']['#disabled'] = TRUE;
  }

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['advanced']['environments_detailed_summaries'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display detailed summaries'),
    '#description' => t("Enable this option to display bundle/module/etc machine names alongside their human names in task summaries."),
    '#default_value' => variable_get('environments_detailed_summaries', TRUE),
  );

  $form['advanced']['banner'] = array(
    '#type' => 'fieldset',
    '#title' => t('Notification banners'),
    '#description' => t('These settings are only taken into account if the current environment has banner notifications enabled.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['advanced']['banner']['environments_css'] = array(
    '#title' => t('Include default CSS'),
    '#type' => 'checkbox',
    '#description' => t('Disable this if you want to write your own notification banner styles.'),
    '#default_value' => variable_get('environments_css', TRUE),
  );

  $form['advanced']['banner']['environments_js'] = array(
    '#title' => t('Include default JS'),
    '#type' => 'checkbox',
    '#description' => t('Disable this if you want to write your own notification banner scripts.'),
    '#default_value' => variable_get('environments_js', TRUE),
  );

  $form['advanced']['drush'] = array(
    '#type' => 'fieldset',
    '#title' => t('Drush'),
    '#description' => t('Configure Drush integration.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['advanced']['drush']['environments_drush'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to Drush'),
    '#description' => t('Enter the absolute path to your Drush executable, for example: /usr/bin/drush'),
    '#default_value' => variable_get('environments_drush', ''),
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
 * @see environments_form_site()
 */
function environments_form_site_submit($form, &$form_state) {
  // Save simple settings.
  variable_set('environments_css', $form_state['values']['environments_css']);
  variable_set('environments_js', $form_state['values']['environments_js']);
  variable_set('environments_drush', $form_state['values']['environments_drush']);
  variable_set('environments_detailed_summaries', $form_state['values']['environments_detailed_summaries']);

  // Use the same message as system_settings_form_submit().
  drupal_set_message(t('The configuration options have been saved.'), 'status', FALSE);

  // Decide whether we're changing environments or not.
  $new_environment = isset($form_state['values']['environment'])
    ? $form_state['values']['environment']
    : NULL;
  $old_environment = variable_get('environments_env');

  if (isset($new_environment) && $new_environment !== $old_environment) {
    // If we're just removing the environment, do it.
    if (empty($new_environment) && !empty($old_environment)) {
      environments_unset();
      drupal_set_message(t("The site's environment has been unset."), 'status', FALSE);
    }
    else {
      $form_state['redirect'] = "admin/config/system/environments/confirm/{$new_environment}";
      unset($_GET['destination']);
    }
  }
}

/**
 * Form callback; displays the confirmation form for switching environments.
 *
 * @see environments_menu()
 */
function environments_form_site_confirm($form, &$form_state, $env) {
  // Prevent the "options saved" message from appearing here as it's confusing.
  drupal_get_messages('status', TRUE);

  $form['environment'] = array(
    '#type' => 'value',
    '#value' => $env,
  );

  $title = $env;
  if ($environment = environments_load($env)) {
    $title = $environment->admin_title;
  }

  $current = t('None');
  if ($current_environment = variable_get('environments_env')) {
    if ($current_environment_obj = environments_load($current_environment)) {
      $current = $current_environment_obj->admin_title;
    }
  }

  $form['#submit'][] = 'environments_form_site_confirm_submit';

  return confirm_form($form,
    t("Are you sure you want to change your site's environment to: %environment?", array('%environment' => $title)),
    'admin/config/system/environments',
    t("Your current site's environment is: %environment", array('%environment' => $current)),
    t('Change environment'),
    t('Cancel')
  );
}

/**
 * Submit callback; changes site environments.
 *
 * @see environments_form_site_confirm()
 */
function environments_form_site_confirm_submit($form, $form_state) {
  if ($new_environment = $form_state['values']['environment']) {
    environments_set($new_environment);

    $title = $new_environment;
    if ($env = environments_load($new_environment)) {
      $title = $env->admin_title;
    }
    drupal_set_message(t('Environment set to: @environmentironment', array('@environment' => $title)));
  }
  else {
    drupal_set_message(t('An illegal choice has been detected. Please contact the site administrator.'), 'error', FALSE);
  }
}

/**
 * Helper function to cast string to array values and vice versa.
 *
 * This is used in the module_* tasks in order to convert user inputs between
 * select lists and textfields when switching the input mode via AJAX.
 *
 * @param mixed $value
 *   The input variable.
 * @param string $selection
 *   The selection mode, one of "custom" or "list".
 *
 * @return mixed
 *   The variable value casted for the given selection mode.
 */
function _environments_cast_module_names($value, $selection) {
  if ($selection === 'custom' && is_array($value)) {
    $value = implode(' ', $value);
  }
  elseif ($selection === 'list' && is_string($value)) {
    $value = drupal_map_assoc(explode(' ', $value));
  }

  return $value;
}

/**
 * Element validation callback.
 *
 * Fixes module name input values.
 *
 * @see environments_task_features_revert_settings()
 * @see environments_task_module_enable_settings()
 * @see environments_task_module_disable_settings()
 * @see environments_task_module_uninstall_settings()
 */
function _environments_task_module_validate($element, &$form_state, $form) {
  if (isset($form_state['values']['op']) && !is_array($form_state['values']['settings']['modules'])) {
    $form_state['values']['settings']['modules'] = drupal_map_assoc(explode(' ', $form_state['values']['settings']['modules']));
  }
}

/**
 * Element validation callback.
 *
 * Validates submitted variable name and cleans up selection mode variable.
 *
 * @see environments_task_variable_set_settings()
 * @see environments_task_variable_del_settings()
 */
function _environments_task_variable_validate_name($element, &$form_state, $form) {
  $name = $form_state['values']['settings']['name'];
  if (!preg_match('/^[a-z]+[a-z0-9\_]*$/', $name)) {
    form_error($element, t('Invalid variable name.'));
  }

  // Remove helper settings if we're submitting the form.
  if ($form_state['submitted']) {
    unset($form_state['values']['settings']['selection']);
  }
}

/**
 * Element validation callback; type-casts the submitted value.
 *
 * @see environments_task_variable_set_settings()
 */
function _environments_task_variable_set_validate_value($element, &$form_state, $form) {
  $value_type = $form_state['values']['settings']['value_type'];
  $original_value = $value = $form_state['values']['settings']['value'];

  switch ($value_type) {
    case 'boolean':
      $value = (bool) $value;
      break;

    case 'integer':
      $value = (int) $value;
      break;

    case 'float':
      $value = (float) $value;
      break;
  }

  // Update value if we casted it.
  if ($original_value !== $value) {
    form_set_value($element, $value, $form_state);
  }

  // Remove helper settings if we're submitting the form.
  if ($form_state['submitted']) {
    unset($form_state['values']['settings']['getvalue']);
  }
}

/**
 * Helper function to return module-related task settings forms.
 *
 * @param object|NULL $task
 *   The current task, if we're editing one.
 * @param array $form_state
 *   The form_state array.
 *
 * @return array
 *   A Form API form.
 */
function _environments_module_task_settings_form($task, array &$form_state) {
  $form = array();

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
      'callback' => 'environments_ajax_task_settings_modules',
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

  return $form;
}

/**
 * AJAX callback; updates the module selection input mode.
 *
 * @see _environments_module_task_settings_form()
 */
function environments_ajax_task_settings_modules($form, &$form_state) {
  return $form['settings']['modules'];
}

/**
 * Page callback; returns the rendered markup for the Environments Summary page.
 *
 * @see environments_menu()
 */
function environments_page_summary() {
  $build = array();
  drupal_set_title(t('Environments Summary'), PASS_THROUGH);
  $all_environments = environments_load_all();
  $all_tasks = environments_get_tasks();
  $detailed = variable_get('environments_detailed_summaries', TRUE);

  // Determine current Environment.
  if ($env = variable_get('environments_env', '')) {
    if (array_key_exists($env, $all_environments)) {
      $env_string = $all_environments[$env]->admin_title;
    }
    else {
      $env_string = $env;
    }
  }
  else {
    $env_string = t('<none>');
  }

  // Current Environment comes first.
  $build['current'] = array(
    '#prefix' => '<h3>',
    '#suffix' => '</h3>',
    '#markup' => t('Current Environment: %current', array('%current' => $env_string)),
  );

  // Display all environments in a table.
  $header = array(
    t('Name'),
    t('Alias'),
    t('Settings'),
    t('Tasks'),
  );
  $rows = array();
  foreach ($all_environments as $env) {
    // Title.
    $title = $env->admin_title;
    if ($detailed) {
      $title .= " ({$env->name})";
    }

    // Settings.
    $env_settings = array();
    foreach ($env->settings as $name => $value) {
      $name_str = drupal_ucfirst($name);

      if ($name === 'notify') {
        $val_str = $value ? t('Yes') : t('No');
      }
      else {
        $val_str = $value;
      }

      $env_settings[] = "{$name_str}: " . $val_str;
    }
    $settings = array(
      '#theme' => 'item_list',
      '#type' => 'ul',
      '#items' => $env_settings,
    );
    $settings = drupal_render($settings);

    // Tasks.
    $env_tasks = array();
    $i = 1;
    foreach ($env->tasks as $task) {
      // Prevent warnings.
      $plugin_def = isset($all_tasks[$task['name']])
        ? $all_tasks[$task['name']]
        : NULL;

      // If the task has a custom summary callback, use it.
      if ($callback = ctools_plugin_get_function($plugin_def, 'summary_callback')) {
        $settings_summary = (array) call_user_func($callback, $task['settings']);
      }
      // If not, generate a default summary as best we can.
      else {
        $settings_summary = environments_default_summary($task);
      }

      $task_name_str = isset($all_tasks[$task['name']]) ? $all_tasks[$task['name']]['admin_title'] : $task['name'];
      $env_task_info = array(
        '#theme' => 'item_list',
        '#title' => "{$i} - {$task_name_str}",
        '#type' => 'ul',
        '#items' => $settings_summary,
      );
      $env_tasks[] = drupal_render($env_task_info);
      ++$i;
    }
    $tasks = implode('', $env_tasks);

    $rows[] = array(
      $title,
      $env->alias,
      $settings,
      $tasks,
    );
  }

  $build['environments'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#caption' => t('Environments'),
  );

  // Allow others (namely Environments Bundles) to alter this.
  drupal_alter('environments_summary', $build);

  return drupal_render($build);
}
