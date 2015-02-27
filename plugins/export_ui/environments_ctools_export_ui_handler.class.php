<?php

/**
 * @file
 * Contains environments_ctools_export_ui_handler.
 */

class environments_ctools_export_ui_handler extends ctools_export_ui {

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
}
