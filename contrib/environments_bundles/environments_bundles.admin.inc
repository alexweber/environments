<?php

/**
 * @file
 * Administration pages.
 */

/**
 * Form builder; Form to delete a given task bundle.
 */
function environments_bundles_form_delete($form, &$form_state, $bundle) {
  $form_state['bundle'] = &$bundle;

  // Deleting an export in code will revert it.
  $op = ($bundle->export_type & EXPORT_IN_CODE) ? t('Revert') : t('Delete');

  return confirm_form(
    $form,
    t('Are you sure you want to @action the task bundle %admin_title?', array(
      '@action' => drupal_strtolower($op),
      '%admin_title' => $bundle->admin_title,
    )),
    'admin/config/development/environments-bundles',
    NULL,
    $op, t('Cancel')
  );
}

/**
 * Submit handler for deleting a task bundle.
 */
function environments_bundles_form_delete_submit($form, &$form_state) {
  $bundle = &$form_state['bundle'];
  $op = ($bundle->export_type & EXPORT_IN_CODE) ? t('reverted') : t('deleted');

  ctools_include('export');
  ctools_export_crud_delete('environments_bundles', $bundle);

  drupal_set_message(t('Task bundle %admin_title was %op.', array(
    '%admin_title' => $bundle->admin_title,
    '%op' => $op,
  )));
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
  $redirect_path = "admin/config/development/environments-bundles/list/{$bundle->name}";

  if (!is_array($bundle->tasks) || empty($bundle->tasks)) {
    $redirect_path .= '/tasks';
    drupal_set_message(t('This bundle has no tasks!'), 'warning', FALSE);
    drupal_goto(url($redirect_path, array('absolute' => TRUE)));
    exit;
  }

  foreach (element_children($bundle->tasks, TRUE) as $key) {
    $task = $bundle->tasks[$key];
    $tasks_summary[] = $task['name'];
  }

  $tasks = theme_item_list(array(
    'items' => $tasks_summary,
    'title' => '',
    'type' => 'ol',
    'attributes' => array(),
  ));

  return confirm_form($form,
    t('Are you sure you want to execute the task bundle: %name?', array('%name' => $bundle->admin_title)),
    $redirect_path,
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

/**
 * Page callback; returns the task summary for a given bundle.
 *
 * @see environments_bundles_menu()
 * @see environments_bundles_task_summary()
 */
function environments_bundles_ajax_summary($js, $bundle) {
  ctools_include('modal');

  $page_title = t('Task Summary');
  $list_title = t('Bundle: %name', array('%name' => $bundle));
  $output = "<h2>{$list_title}</h2>";
  $items = array();
  $i = 1;

  if ($loaded_bundle = environments_bundles_load($bundle)) {
    foreach ($loaded_bundle->tasks as $key => $task) {
      $identifier = isset($task['settings']['name'])
        ? "({$task['settings']['name']})"
        : '';
      $fieldset_title = $i++ . " - {$task['name']} {$identifier}";

      $item_data = array(
        '#type' => 'fieldset',
        '#title' => $fieldset_title,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#attributes' => array('class' => array('collapsible', 'collapsed')),
      );

      if ($callback = ctools_plugin_get_function($task, 'summary_callback')) {
        $task_summary = (array) call_user_func($callback, $task['settings']);
      }
      else {
        $task_summary = environments_default_summary($task);
      }

      $item_data['items'] = array(
        '#theme' => 'item_list',
        '#title' => '',
        '#type' => 'ul',
        '#attributes' => array(),
        '#items' => $task_summary,
      );

      $items[$key] = drupal_render($item_data);
    }

    $output .= implode('', $items);
  }
  else {
    drupal_set_message(t('Error loading task bundle: @name', array('@name' => $bundle)), 'error', FALSE);
    $output .= theme('status_messages');
  }

  if ($js) {
    ctools_modal_render($page_title, $output);
  }
  else {
    drupal_set_title($page_title);
    return $output;
  }
}
