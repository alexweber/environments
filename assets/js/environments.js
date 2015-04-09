(function ($, Drupal) {
	'use strict';

	Drupal.behaviors.environmentsNotification = {
		attach: function (context) {
			var $banner = $('#environments-notify', context);

			if ($banner.length) {
				$banner.find('a.close').click(function () {
					$banner.fadeOut(300, function () {
						$banner.remove();
					}).attr('href', 'javascript:;');
				});
			}
		}
	};
})(jQuery, Drupal);
