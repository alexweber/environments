<?php
/**
 * @file
 * Environments notification banner template.
 *
 * Available variables:
 *  - $environment string The current environment name.
 *  - $environment_alias string The current environment alias.
 *  - $environment_message string The notification message.
 */
?>
<div data-alert
     class="environments-notify alert-box <?php print $environment; ?> <?php print $environment_alias; ?>">
  <div class="message"><?php print $environment_message; ?></div>
  <a href="#" class="close">×</a>
</div>
