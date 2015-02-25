(function ($, Drupal) {
	Drupal.behaviors.environments_notification = {
		attach: function (context, settings) {
			var $banner = $('#environments-notify', context);

			if ($banner.length) {
				$banner.find('a.close').click(function () {
					$banner.fadeOut('normal', function () {
						$banner.remove();
					});
				});
			}
		}
	};
})(jQuery, Drupal);
