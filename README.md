# Environments module

This module attempts to solve the problem of executing repetive tasks when switching between different server
environments, such as development, staging or production.

It inroduces 3 new concepts:

* Environments
* Tasks
* Task Bundles

## Environments

An environment is a Drupal object (a CTools Exportable to be more specific) which represents a server environment, such
as "production", "staging", "development" and so on.

The environment object has a name, optional alias (for Drush), settings and any number of tasks. All defined environments can be
managed in: `Administration -> Configuration -> Development -> Environments`.

### Current Environment

Having defined one or more environments, you can now switch your site between them (and also explicitly un-set your site's environment).

You can view the current environment in your site's Status Report or in the settings page:
`Administration -> Configuration -> System -> Site Environment`

Only one environment can be active at any given time and, when an environment becomes active, it's tasks are executed
in order.

You can manually switch your site's active environment at any given time either in the Admin UI described above or by
using the handy __Drush__ command: `drush environment`, or `drush env` for short:

```bash
# View documentation.
drush help environment

# View all defined environments.
drush env

# Switch to the "dev" environment.
drush env dev
```

## Tasks

Environment Tasks are operations such as enabling or disabling a module or changing a variable's value, that are
executed when an environment becomes active.

Once you've created an environment at `Administration -> Configuration -> Development -> Environments`, you can
configure it's tasks by going to it's "Manage Tasks" page.

It's important to note that you can drag the tasks around to re-arrange them and that they will run in the _exact_ order
in which they appear.

The following tasks are available out of the box:

* Clear caches
* Delete variable
* Disable module(s)
* Enable module(s)
* Execute callback
* Execute Drush command
* Execute Rules component
* Import Migrations
* Revert Feature(s)
* Roll-back Migrations
* Set variable value
* Uninstall module(s)

## Task Bundles

The included "Environments Bundles" module enables _Task Bundles_, which are nothing more than a grouping of one or more tasks.

Bundles are both architecturally and conceptually virtually identical to _Environments_ except for 2 major differences:

* Bundles don't have an "active" state like Environments
* Unlike Environments, you can execute all of a Bundle's tasks on demand (with Environments you'd have to make that
Environment active to have it's tasks execute).

Environments Bundles can be managed at: `Administration -> Configuration -> Development -> Environments -> Environments Bundles`.

Task Bundles may be executed in the Admin UI described above or by using the handy __Drush__ command:
`drush environment-bundle`, or `drush envb` for short:

```bash
# View documentation.
drush help environment-bundle

# Execute "foo" bundle.
drush envb foo
```

The Environments Bundles module also enables a new task:

* Execute Task Bundle

This task can be added to _Environments_ as well as _Task Bundles_, so you can have a bundle that executes another bundle (but not itself because that would be bundle inception).

## Drush

In order to use the task that executes a Drush command, you must first configure the path to Drush in the settings form:
 `Administration -> Configuration -> System -> Site Environment`

You must insert the absolute path to the executable and, if it contains spaces you must manually escape them, for
example: `/Applications/Dev\ Desktop/drush/drush`

The following documentation page from the Migrate module has pretty good instructions for getting a very similar Drush
integration working (from which we):
[https://www.drupal.org/node/1958170](https://www.drupal.org/node/1958170)

All you have to do is replace the variable name `migrate_drush_path` in the instructions with ours `environments_drush`.
In fact, if you already have Migrate configured, we will borrow from their config to initialize ours when you enable the
module so you won't have to do anything!

**PRO Tip:** You can override the path to Drush per server using `$conf['environments_drush']` in your settings.php

## Suggested Usage

The optimal way of working with Environments, particularly when maintaining multiple sites, each with their own somewhat
yet not entirely similar set of modules, is to leverage Task Bundles to create smaller, more manageable sets of tasks
targetted at a particular functionality, module or module suite. For example:

		AdvAgg Production Bundle
		 	* Enable AdvAgg + submodules
		 	* Enable JS Compression using JSMin extension

		AdvAgg Development Bundle
		 	* Disable AdvAgg + submodules

The above example is very simple but it demonstrates reasonably well how you may apply different settings to different
environments in a real-life situation.

Grouping your tasks into targetted bundles also allows them to be more easily re-used amongst other projects where
there might be small differences (ie: no CDN) and that alone means for example that the "Production" environment of a
site with a CDN couldn't be shared with a site that didn't use it.

However, using bundles we can simply add the bundles we want to our environments and manage tasks in a much more
streamlined manner:

		Production Environment Tasks
			* Execute "AdvAgg Production Bundle"
			* Execute "CDN Production Bundle"
			* Execute "Disable Debug Bundle"
			* Execute "Enable Caching Bundle"

		Staging Environment Tasks
			* Execute "AdvAgg Production Bundle"
			* Execute "Enable Caching Bundle"
			* Enable JS Compression using JSMin+

		Development Environment Tasks
			* Execute "AdvAgg Development Bundle"
			* Execute "CDN Development Bundle"
			* Execute "Enable Debug Bundle"
			* Execute "Disable Caching Bundle"

**PRO Tip:** See the last task in the Staging tasks above? Instead of creating an "AdvAgg Staging" bundle which would be identical to the production one except for the JS Compression, we execute the production bundle and then add a new task immediately after it to override just that one setting.

## Environments API

If you are familiar with CTools Plugins, creating a new Environments Task should be pretty straight-forwards:

```php
$plugin = array(
  // Title for Admin UI.
  'admin_title' => t('My Cool Task'),

  // Batch API callback to execute. You can also define this using the
  // alternative method below.
  'task_callback' => 'foo_task_callback',
  // Batch API callback to execute. This is an alternative implementation that
  // gives you more control over where your callback lives and what it's called.
  // Which callback to use is automatically determined by CTools.
//  'task_callback' => array(
//    'file' => '',
//    'path' => '',
//    'function' => '',
//  ),

  // Define any settings your task might implement.
  // It's ok to have no settings. If, however, you *do* have some, make sure
  // you create a settings_callback to allow these values to be changed.
  'settings' => array(
    'modules' => array(),
    'enable_dependencies' => TRUE,
  ),

  // Optional Form callback for settings form. You can also define this using
  // a similar alternative method as with the batch callbacks.
  'settings_callback' => 'environments_task_module_enable_settings',

  // Optional Form callback for summary text. You can also define this using
  // a similar alternative method as with the batch callbacks.
  'summary_callback' => 'environments_task_module_enable_summary',
);
```

The existing tasks are all well-documented too so check them out.

## Videos

* [Environments Intro/Overview](https://vimeo.com/121030324)
