<?php

/**
 * @file
 * Export interface plugin for Environments.
 */

$module_path = drupal_get_path('module', 'environments');

/**
 * Define this Export UI plugin.
 */
$plugin = array(
	'schema' => 'environments',
	// As defined in hook_schema().
	'access' => 'administer environments',
	// Define a permission users must have to access these pages.

	// Define the menu item.
	'menu' => array(
		'menu prefix' => 'admin/config/development',
		'menu item' => 'environments',
		'menu title' => 'Environments',
		'menu description' => 'Administer site environments.',
		// Add extra menu items.
		'items' => array(
			// Manage Tasks form.
			'tasks' => array(
				'path' => 'list/%ctools_export_ui/tasks',
				'title' => 'Manage Tasks',
				'page callback' => 'drupal_get_form',
				'page arguments' => array('environments_task_form', 5),
				'access arguments' => array('administer environments'),
				'file' => 'environments.admin.inc',
				'file path' => $module_path,
				'type' => MENU_LOCAL_TASK,
				'weight' => -5,
			),
			// Add new task form.
			'task add' => array(
				'path' => 'list/%ctools_export_ui/tasks/add',
				'title' => 'Add Task',
				'page callback' => 'drupal_get_form',
				'page arguments' => array('environments_task_form_edit', 5),
				'access arguments' => array('administer environments'),
				'file' => 'environments.admin.inc',
				'file path' => $module_path,
				'type' => MENU_LOCAL_ACTION,
			),
			// Edit task form.
			'task edit' => array(
				'path' => 'list/%ctools_export_ui/tasks/%/edit',
				'title' => 'Edit Task',
				'page callback' => 'drupal_get_form',
				'page arguments' => array('environments_task_form_edit', 5, 7),
				'access arguments' => array('administer environments'),
				'file' => 'environments.admin.inc',
				'file path' => $module_path,
			),
			// Delete task form.
			'task delete' => array(
				'path' => 'list/%ctools_export_ui/tasks/%/delete',
				'title' => 'Delete Task',
				'page callback' => 'drupal_get_form',
				'page arguments' => array('environments_task_form_delete', 5, 7),
				'access arguments' => array('administer environments'),
				'file' => 'environments.admin.inc',
				'file path' => $module_path,
			),
		),
	),
	// Define user interface texts.
	'title singular' => t('Environment'),
	'title plural' => t('Environments'),
	'title singular proper' => t('Environment'),
	'title plural proper' => t('Environments'),
	// Define the names of the functions that provide the add/edit forms.
	'form' => array(
		'settings' => 'environments_ctools_export_ui_form',
		'validate' => 'environments_ctools_export_ui_form_validate',
		'submit' => 'environments_ctools_export_ui_form_submit',
	),
	// Override the UI handler.
	'handler' => array(
		'class' => 'environments_environments_ctools_export_ui_handler',
		'parent' => 'ctools_export_ui',
	),
	// Add an extra operation.
	'allowed operations' => array(
		'tasks' => array('title' => t('Manage Tasks')),
	),

);

/**
 * CTools Export UI form.
 */
function environments_ctools_export_ui_form(&$form, &$form_state) {
	// Make environment reference in form_state.
	$form_state['environment'] = &$form_state['item'];

	// Load the configuration form.
	$form = drupal_retrieve_form('environments_form_edit', $form_state);
}

/**
 * Form builder; Form to edit a given environment.
 *
 * @see environments_ctools_export_ui_form()
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
		'#disabled' => $form_state['form type'] === 'edit',
	);

	// Alias.
	$form['alias'] = array(
		'#type' => 'textfield',
		'#title' => t('Alias'),
		'#description' => t('An optional alias to use with Drush. Must contain !num or less characters and use only letters.', array('!num' => ENVIRONMENTS_ALIAS_MAXLENGTH)),
		'#default_value' => !empty($environment->alias) ? $environment->alias : NULL,
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

	// Notify when environment is active?
	$form['settings']['notify'] = array(
		'#type' => 'checkbox',
		'#title' => t('Notify when this environment is active.'),
		'#description' => t('Displays a small notification banner across the the top of the site.'),
		'#default_value' => $environment->settings['notify'],
	);

	return $form;
}

/**
 * Validation handler.
 *
 * @see environments_form_edit()
 */
function environments_ctools_export_ui_form_validate(&$form, &$form_state) {
	$alias = $form_state['values']['alias'];

	// Validate aliases.
	if (!empty($alias)) {
		// First check if alias has required length.
		if (strlen($alias) > ENVIRONMENTS_ALIAS_MAXLENGTH) {
			form_set_error('alias', t('Alias must contain !num or less characters.', array('!num' => ENVIRONMENTS_ALIAS_MAXLENGTH)));
		}
		// Then, check if alias is a valid machine name.
		else {
			if (!ctype_alpha($alias)) {
				form_set_error('alias', t('Alias must contain only letters.'));
			}
			else {
				// Then, check if alias is unique.
				$params = array(
					':alias' => $form_state['values']['alias'],
					':name' => $form_state['values']['name'],
				);

				$result = db_query("SELECT COUNT(*) FROM {environments} WHERE alias = :alias AND name <> :name", $params)
					->fetchField();

				if ((int) $result !== 0) {
					form_set_error('alias', t('This alias is already in use.'));
				}
			}
		}
	}
	// Explicitly set empty aliases to NULL.
	else {
		form_set_value($form['alias'], NULL, $form_state);
	}
}

/**
 * Submit handler.
 *
 * @see environments_form_edit()
 */
function environments_ctools_export_ui_form_submit(&$form, &$form_state) {
	// Edit the reference to $form_state['environment'] which will in turn
	// reference $form_state['item'] which is what CTools is looking for.
	$environment = &$form_state['environment'];
	$environment->admin_title = $form_state['values']['admin_title'];
	$environment->alias = !empty($form_state['values']['alias'])
		? $form_state['values']['alias']
		: NULL;
	$environment->settings = $form_state['values']['settings'];
}
