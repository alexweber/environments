<?php

/**
 * @file
 * Admin pages.
 */

/**
 * Form builder; Form to edit a given environment.
 */
function environments_form_edit($form, &$form_state) {
	if (!empty($form_state['environment'])) {
		$environment = $form_state['environment'];
	}
	else {
		$environment = environments_create();
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
			'exists' => 'environments_exists',
		),
		'#required' => TRUE,
		'#default_value' => $environment->name,
	);

	// Alias.
	$form['alias'] = array(
		'#type' => 'textfield',
		'#title' => t('Alias'),
		'#description' => t('An optional alias for this environment to use with Drush. Must contain !num or less characters and use only letters.', array('!num' => ENVIRONMENTS_ALIAS_MAXLENGTH)),
		'#default_value' => $environment->alias,
		'#size' => 8,
		'#maxlength' => ENVIRONMENTS_ALIAS_MAXLENGTH,
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
function environments_form_delete($form, &$form_state, $environment) {
	$form_state['environment'] = &$environment;

	// Deleting an export in code will revert it.
	$op = ($environment->export_type & EXPORT_IN_CODE) ? 'Revert' : 'Delete';

	return confirm_form(
		$form,
		t('Are you sure you want to @action the environment %admin_title?', array(
			'@action' => t(drupal_strtolower($op)),
			'%admin_title' => $environment->admin_title
		)),
		'admin/config/development/environments',
		NULL,
		t($op), t('Cancel')
	);
}

/**
 * Submit handler for deleting an environment.
 */
function environments_form_delete_submit($form, &$form_state) {
	$environment = &$form_state['environment'];
	$op = ($environment->export_type & EXPORT_IN_CODE) ? 'reverted' : 'deleted';

	ctools_include('export');
	ctools_export_crud_delete('environments', $environment);

	drupal_set_message(t('Environment %admin_title was ' . $op . '.', array('%admin_title' => $environment->admin_title)));
	$form_state['redirect'] = 'admin/config/development/environments';
}

/**
 * Form callback.
 *
 * Displays task overview form.
 *
 * @see environments_menu()
 */
function environment_task_form($form, &$form_state, $environment) {
	if (is_string($environment)) {
		$environment = environments_load($environment);
	}

	$form_state['environment'] = $environment;

	// List tasks.
	if (is_object($environment) && !empty($environment->tasks)) {
		$all_tasks = environments_get_tasks();

		$form['tasks'] = array(
			'#tree' => TRUE,
		);

		foreach (element_children($environment->tasks) as $id) {
			$task = $environment->tasks[$id];

			// Prepare settings summary.
			$settings_summary = array();
			foreach ($task['settings'] as $name => $value) {
				if ($value === TRUE) {
					$value = 'true';
				}
				else {
					if ($value === FALSE) {
						$value = 'false';
					}
					else {
						if (is_array($value)) {
							$value = implode(', ', $value);
						}
					}
				}

				$settings_summary[] = "<strong>{$name}:</strong> {$value}";
			}

			ctools_include('export');
			$plugin = ctools_get_export_ui('environments_ctools_export_ui');
			$path = ctools_export_ui_plugin_menu_path($plugin, 'tasks', $environment->name);
			$destination = drupal_get_destination();

			// Prepare operations links
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
 * @see environment_task_form()
 */
function environment_task_form_submit($form, &$form_state) {
	$environment = $form_state['environment'];

	foreach ($form_state['values']['tasks'] as $id => $data) {
		$environment->tasks[$id]['#weight'] = $data['weight'];
	}

	// Sort tasks by new weights.
	uasort($environment->tasks, 'element_sort');

	if (environments_save($environment)) {
		drupal_set_message(t('Task order updated!'), 'status', FALSE);
	}
	else {
		drupal_set_message(t('Error updating task order.'), 'error', FALSE);
		watchdog('environments', 'Error updating task order for environment: @name', array('@name' => $environment->admin_title), WATCHDOG_ERROR);
	}

}

/**
 * Form callback.
 *
 * Displays delete task form.
 */
function environment_task_form_delete($form, &$form_state, $environment, $task) {
	$form_state['environment'] = $environment;
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
 * @see environment_task_form_delete()
 */
function environment_task_form_delete_submit($form, &$form_state) {
	$environment = $form_state['environment'];
	$task = $form_state['task'];

	if (array_key_exists($task, $environment->tasks)) {
		unset($environment->tasks[$task]);
		if (environments_save($environment)) {
			drupal_set_message(t('Task deleted!'), 'status', FALSE);
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
function environment_task_form_edit($form, &$form_state, $environment, $task = NULL) {
	// Store environment in form state for AJAX.
	$form_state['environment'] = $environment;
	$current_task = !empty($form_state['values']['task']) ? $form_state['values']['task'] : '';

	// Prepare task info if editing.
	if (isset($task) && array_key_exists($task, $environment->tasks)) {
		// Grab task instance info.
		$task_info = $environment->tasks[$task];
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
		'#prefix' => '<div id="environments-task-settings">',
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
			$settings_form = call_user_func($callback, $form_state['environment'], $form_state['task']);
			$form['settings'] += $settings_form;
		}
		else {
			$form['settings'] += array(
				'#description' => t('This task has no settings.'),
			);
		}
	}
	else {
		if ($current_task) {
			if ($task_info = environments_get_task($current_task)) {
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
	$path = "admin/config/development/environments/list/{$environment->name}/tasks";
	$form['actions']['cancel'] = array(
		'#markup' => l(t('Cancel'), $path, array('absolute' => TRUE)),
	);

	$form['#submit'][] = 'environment_task_form_edit_submit';
	$form['#submit'][] = 'environment_task_form_edit_submit_redirect';

	return $form;
}

/**
 * AJAX Callback.
 *
 * Returns task settings form.
 */
function environments_ajax_task_settings($form, &$form_state) {
	return $form['settings'];
}

/**
 * Submit callback.
 *
 * @see environment_task_form_edit()
 */
function environment_task_form_edit_submit($form, &$form_state) {
	$environment = $form_state['environment'];
	$new_task = $form_state['values']['task'];
	$new_settings = array_key_exists('settings', $form_state['values'])
		? $form_state['values']['settings']
		: array();

	// Cast boolean values so that we can store them properly and display them
	// accordingly on summaries.
	foreach($new_settings as $key => $value) {
		if (isset($form['settings'][$key]) && $form['settings'][$key]['#type'] === 'checkbox') {
			$new_settings[$key] = (bool) $value;
		}
	}

	// Edit.
	if (array_key_exists('task', $form_state)) {
		$id = $form_state['task']['#id'];
		$environment->tasks[$id]['name'] = $new_task;
		$environment->tasks[$id]['settings'] = $new_settings;

		if (environments_save($environment)) {
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

			// Add new task to environment.
			$environment->tasks[$id] = array(
				'name' => $task['name'],
				'settings' => $settings,
				'#id' => $id,
				'#weight' => 0,
			);

			if (environments_save($environment)) {
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
 * @see environment_task_form_edit()
 */
function environment_task_form_edit_submit_redirect($form, &$form_state) {
	$env = $form_state['environment'];
	$form_state['redirect'] = "admin/config/development/environments/list/{$env->name}/tasks";

	if ($form_state['values']['op'] === t('Save and add another')) {
		$form_state['redirect'] .= '/add';
		unset($_GET['destination']);
	}
}

/**
 * Form callback.
 *
 * Displays the "Site Environment" form.
 *
 * @see environments_menu()
 */
function environments_form_site($form) {
	$environments_options = array();
	$environments = environments_load_all();

	foreach ($environments as $env) {
		$environments_options[$env->name] = $env->admin_title;
	}

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

	$form['advanced']['banner'] = array(
		'#type' => 'fieldset',
		'#title' => t('Notification banners'),
		'#description' => t('These settings are only taken into account if the current environment has banner notifications enabled.'),
		'#collapsible' => FALSE,
	);

	$form['advanced']['banner']['environments_css'] = array(
		'#title' => t('Include default CSS'),
		'#type' => 'checkbox',
		'#description' => t('Disable this if you want to write your own notification banner styles'),
		'#default_value' => variable_get('environments_css', TRUE),
	);

	$form['advanced']['banner']['environments_js'] = array(
		'#title' => t('Include default JS'),
		'#type' => 'checkbox',
		'#description' => t('Disable this if you use a <a href="!url">Zurb Foundation</a> sub-theme.', array('!url' => 'https://www.drupal.org/project/zurb_foundation')),
		'#default_value' => variable_get('environments_js', TRUE),
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
	// Persist CSS/JS settings.
	variable_set('environments_css', $form_state['values']['environments_css']);
	variable_set('environments_js', $form_state['values']['environments_js']);

	// Use the same message as system_settings_form_submit().
	drupal_set_message(t('The configuration options have been saved.'), 'status', FALSE);

	// Decide whether we're changing environments or not.
	$new_environment = isset($form_state['values']['environment'])
		? $form_state['values']['environment']
		: NULL;
	$old_environment = variable_get('environments_env');

	if (isset($new_environment) && $new_environment !== $old_environment) {
		// If we're just removing the environment, do it.
		if (empty($new_environment)) {
			environments_unset();
			drupal_set_message(t('The site\'s environment has been unset.'), 'status', FALSE);
		}
		else {
			unset($_GET['destination']);
			$form_state['redirect'] = "admin/config/system/environments/confirm/{$new_environment}";
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
		'#value' => $env
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
		t('Are you sure you want to change your site\'s environment to: %environment?', array('%environment' => $title)),
		'admin/config/system/environments',
		t('Your current site\'s environment is: %environment', array('%environment' => $current)),
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
 * Returns all available Drupal modules. Used by the "module_enable" and
 * "module_disable" tasks. This is as a lightweight replacement for using
 * Drupal core APIs.
 *
 * @return array
 *   An array of module names, keyed by machine_name.
 */
function _environments_get_modules() {
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
function _environments_get_variables() {
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
