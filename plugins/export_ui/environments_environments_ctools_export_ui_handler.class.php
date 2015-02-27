<?php

/**
 * @file
 * Contains environments_ctools_export_ui_handler.
 */

class environments_environments_ctools_export_ui_handler extends environments_ctools_export_ui_handler {

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
