(function ($, Drupal) {
	Drupal.behaviors.env_admin = {
		attach: function (context, settings) {
			$('#edit-task', context).on('change', function () {
//        if ($(this).val() === '') {
//          setTimeout(function() {
//            $('#env-task-settings').slideUp();
//          }, 1000);
//        }
			});
		}
	};
})(jQuery, Drupal);
