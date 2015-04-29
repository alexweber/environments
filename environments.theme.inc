<?php

/**
 * @file
 * Theme functions.
 */

/**
 * Implements hook_preprocess_HOOK().
 *
 * Sets current environment message for the notification banner.
 *
 * @see environments_theme()
 */
function environments_preprocess_environments_banner(&$variables) {
  if ($environment = environments_load($variables['environment'])) {
    $variables['environment_message'] = t('Current environment: @name', array('@name' => $environment->admin_title));
    $variables['environment_alias'] = $environment->alias;
  }
}

/**
 * Theme callback.
 *
 * Overrides theme for the "Manage Tasks" form.
 *
 * @see environments_theme()
 * @see environments_task_form()
 */
function theme_environments_task_form($variables) {
  $form = $variables['form'];

  $rows = array();
  foreach (element_children($form['tasks']) as $id) {
    $form['tasks'][$id]['weight']['#attributes']['class'] = array('environments-tasks-order-weight');
    $rows[] = array(
      'data' => array(
        array('class' => array('environments-tasks-order-handle')),
        drupal_render($form['tasks'][$id]['task']),
        drupal_render($form['tasks'][$id]['settings']),
        drupal_render($form['tasks'][$id]['weight']),
        drupal_render($form['tasks'][$id]['op']),
      ),
      'class' => array('draggable'),
    );
  }

  $header = array('', t('Task'), t('Settings'), t('Weight'), t('Operations'));
  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'environments-tasks-order'),
  ));
  $output .= drupal_render_children($form);

  drupal_add_tabledrag('environments-tasks-order', 'order', 'sibling', 'environments-tasks-order-weight');
  return $output;
}
