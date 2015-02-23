<?php

/**
 * @file
 * Contains of env_ctools_export_ui_handler.
 */

class env_ctools_export_ui_handler extends ctools_export_ui {

  /**
   * @inheritdoc
   */
  function build_operations($item) {
    $allowed_operations = parent::build_operations($item);

    // Move our custom "Manage Tasks" operation to the bottom of the list.
    $task = array_key_exists('tasks', $allowed_operations)
      ? $allowed_operations['tasks']
      : NULL;

    if ($task) {
      unset ($allowed_operations['tasks']);
      $allowed_operations['tasks'] = $task;
    }

    return $allowed_operations;
  }

  /**
   * @inheritdoc
   */
  function list_build_row($item, &$form_state, $operations) {
    parent::list_build_row($item, $form_state, $operations);

    // Add a new "alias" field.
    $name = $item->{$this->plugin['export']['key']};
    $data = array(array('data' => check_plain($item->alias), 'class' => array('ctools-export-ui-alias')));

    // Put this field after tile and name.
    array_splice($this->rows[$name]['data'], 2, 0, $data);
  }

  /**
   * @inheritdoc
   */
  function list_table_header() {
    $header = parent::list_table_header();

    // Add a new "Alias" header.
    $data = array(array('data' => t('Alias'), 'class' => array('ctools-export-ui-alias')));

    // Put this field after tile and name.
    array_splice($header, 2, 0, $data);
    return $header;
  }

  /**
   * @inheritdoc
   */
  function list_sort_options() {
    $options = parent::list_sort_options();

    // Add a new "Alias" sort.
    $options['alias'] = t('Alias');

    return $options;
  }
}
