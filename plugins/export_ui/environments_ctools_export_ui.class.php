<?php

/**
 * @file
 * Contains environments_ctools_export_ui.
 */

/**
 * Class environments_ctools_export_ui; contains common overrides for our
 * exportables.
 */
abstract class environments_ctools_export_ui extends ctools_export_ui {

	/**
	 * @inheritdoc
	 */
	function get_page_title($op, $item = NULL) {
		if (empty($this->plugin['strings']['title'][$op])) {
			return;
		}

		// Replace %title that might be there with the exportable title.
		$title = $this->plugin['strings']['title'][$op];
		if (!empty($item)) {
			$title = (str_replace('%title', check_plain($item->admin_title), $title));
		}

		return $title;
	}

	/**
	 * @inheritdoc
	 */
	function edit_save_form($form_state) {
		$item = &$form_state['item'];
		$title = $item->admin_title;

		$result = ctools_export_crud_save($this->plugin['schema'], $item);

		if ($result) {
			$message = str_replace('%title', check_plain($title), $this->plugin['strings']['confirmation'][$form_state['op']]['success']);
			drupal_set_message($message);
		}
		else {
			$message = str_replace('%title', check_plain($title), $this->plugin['strings']['confirmation'][$form_state['op']]['fail']);
			drupal_set_message($message, 'error');
		}
	}

	/**
	 * @inheritdoc
	 */
	function delete_form_submit(&$form_state) {
		$item = $form_state['item'];

		ctools_export_crud_delete($this->plugin['schema'], $item);
		$message = str_replace('%title', check_plain($item->admin_title), $this->plugin['strings']['confirmation'][$form_state['op']]['success']);
		drupal_set_message($message);
	}

}
