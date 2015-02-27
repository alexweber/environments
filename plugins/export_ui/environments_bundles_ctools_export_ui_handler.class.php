<?php

/**
 * @file
 * Contains environments_bundles_ctools_export_ui_handler.
 */

class environments_bundles_ctools_export_ui_handler extends environments_ctools_export_ui_handler {

	/**
	 * @inheritdoc
	 */
	function list_build_row($item, &$form_state, $operations) {
		parent::list_build_row($item, $form_state, $operations);

		// Add a new "description" field.
		$name = $item->{$this->plugin['export']['key']};
		$data = array(array('data' => check_plain($item->description), 'class' => array('ctools-export-ui-description')));

		// Put this field after tile and name.
		array_splice($this->rows[$name]['data'], 2, 0, $data);
	}

	/**
	 * @inheritdoc
	 */
	function list_table_header() {
		$header = parent::list_table_header();

		// Add a new "Description" header.
		$data = array(array('data' => t('Description'), 'class' => array('ctools-export-ui-description')));

		// Put this field after tile and name.
		array_splice($header, 2, 0, $data);
		return $header;
	}

}
