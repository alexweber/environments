(function ($, Drupal) {
	Drupal.behaviors.environments_notification = {
		attach: function (context, settings) {
			var $banner = $('#environments-notify', context);

			if ($banner.length) {
				$banner.find('a.close').click(function () {
					$banner.fadeOut(300, function () {
						$banner.remove();
					});
				});
			}
		}
	};
})(jQuery, Drupal);
