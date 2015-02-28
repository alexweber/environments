<?php

/**
 * @file
 * Administration pages.
 */

/**
 * Form builder; Form to edit a given task bundle.
 */
function environments_bundles_form_edit($form, &$form_state) {
	if (!empty($form_state['bundle'])) {
		$bundle = $form_state['bundle'];
	}
	else {
		$bundle = environments_bundles_create();
	}

	// Title
	$form['admin_title'] = array(
		'#type' => 'textfield',
		'#maxlength' => '255',
		'#title' => t('Title'),
		'#description' => t('A human-readable title for this task bundle.'),
		'#required' => TRUE,
		'#default_value' => $bundle->admin_title,
	);

	// Machine name.
	$form['name'] = array(
		'#type' => 'machine_name',
		'#maxlength' => '255',
		'#machine_name' => array(
			'source' => array('admin_title'),
			'exists' => 'environments_bundles_exists',
		),
		'#required' => TRUE,
		'#default_value' => $bundle->name,
		'#disabled' => $form_state['form type'] === 'edit',
	);

	// Description.
	$form['description'] = array(
		'#type' => 'textarea',
		'#title' => t('Description'),
		'#default_value' => $bundle->description,
		'#description' => t('Insert an optional description that will appear in the Admin UI.'),
	);

	return $form;
}

/**
 * Form builder; Form to delete a given task bundle.
 */
function environments_bundles_form_delete($form, &$form_state, $bundle) {
	$form_state['bundle'] = &$bundle;

	// Deleting an export in code will revert it.
	$op = ($bundle->export_type & EXPORT_IN_CODE) ? 'Revert' : 'Delete';

	return confirm_form(
		$form,
		t('Are you sure you want to @action the task bundle %admin_title?', array(
			'@action' => t(drupal_strtolower($op)),
			'%admin_title' => $bundle->admin_title
		)),
		'admin/config/development/environments-bundles',
		NULL,
		t($op), t('Cancel')
	);
}

/**
 * Submit handler for deleting a task bundle.
 */
function environments_bundles_form_delete_submit($form, &$form_state) {
	$bundle = &$form_state['bundle'];
	$op = ($bundle->export_type & EXPORT_IN_CODE) ? 'reverted' : 'deleted';

	ctools_include('export');
	ctools_export_crud_delete('environments_bundles', $bundle);

	drupal_set_message(t('Task bundle %admin_title was ' . $op . '.', array('%admin_title' => $bundle->admin_title)));
	$form_state['redirect'] = 'admin/config/development/environments-bundles';
}

/**
 * Page callback.
 *
 * @see environments_ctools_export_ui_form_bundles()
 */
function environments_bundles_exec_form($form, &$form_state, $bundle) {
	$tasks_summary = array();
	$form_state['bundle'] = $bundle;

	if (is_array($bundle->tasks)) {
		foreach (element_children($bundle->tasks, TRUE) as $key) {
			$task = $bundle->tasks[$key];
			$tasks_summary[] = $task['name'];
		}
	}

	if (!empty($tasks_summary)) {
		$tasks = theme_item_list(array(
			'items' => $tasks_summary,
			'title' => '',
			'type' => 'ol',
			'attributes' => array(),
		));
	}
	else {
		$tasks = ' <em>' . t('None') . '</em>';
	}

	return confirm_form($form,
		t('Are you sure you want to execute the task bundle: %name?', array('%name' => $bundle->admin_title)),
		"admin/config/development/environments-bundles/list/{$bundle->name}",
		t('The following tasks will run:') . $tasks,
		t('Execute'),
		t('Cancel')
	);
}

/**
 * Submit handler for executing a task bundle.
 */
function environments_bundles_exec_form_submit($form, &$form_state) {
	$bundle = &$form_state['bundle'];
	environments_bundles_exec($bundle);
}
