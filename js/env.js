(function ($, Drupal) {
  Drupal.behaviors.env_notification = {
    attach: function(context, settings) {
      var $banner = $('#env-notify', context);

      if ($banner.length) {
        $banner.find('a.close').click(function() {
          $banner.fadeOut('normal', function() {
            $banner.remove();
          });
        });
      }
    }
  };
})(jQuery, Drupal);
