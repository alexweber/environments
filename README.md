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

The environment object has a name, optional alias, settings and any number of tasks. All defined environments can be
managed in: `Administration -> Configuration -> Development -> Environments`.

### Current Environment

Having defined one or more environments, you can now switch your site between them.

You can view the current environment in your site's Status Report or in the settings page:
`Administration -> Configuration -> System -> Site Environment`

Only one environment can be active at any given time and, when an environment becomes active, it's tasks are executed
in order.

You can manually switch your site's active environment at any given time either in the Admin UI described above or by
using the handy __Drush__ command: `drush environments`, or `drush env` for short:

```bash
# View documentation.
drush environments --help

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

The included "Environments Bundles" module enables _Task Bundles_, which are nothing more than one or more tasks,
grouped together.

Bundles are virtually identical to _Environments_ except for 2 major differences:

* Bundles don't have an "active" state like Environments
* Unlike Environments, you can execute all of a Bundle's tasks on demand (with Environments you'd have to make that
Environment active to have it's tasks execute)

Task Bundles may be executed in the Admin UI described above or by using the handy __Drush__ command:
`drush environments-bundle`, or `drush envb` for short:

```bash
# View documentation.
drush environments-bundle --help

# Execute "foo" bundle.
drush envb foo
```

The Environments Bundles module also enables a new task:

* Execute Task Bundle

Yo dawg, I heard you like bundles, so this task can be added to Environments as well as Task Bundles, so you can have a
bundle that executes another bundle (but not itself because that would be bundle inception).

## Drush

In order to use the task that executes a Drush command, you must first configure the path to Drush in the settings form:
 `Administration -> Configuration -> System -> Site Environment`

You must insert the absolute path to the executable and, if it contains spaces you must manually escape them, for
example: `/Applications/Dev\ Desktop/drush/drush`

The following documentation page from the Migrate module has pretty good instructions for getting a very similar Drush
integration working (this one is totally ripped off theirs actually!):
[https://www.drupal.org/node/1958170](https://www.drupal.org/node/1958170)

All you have to do is replace the variable name `migrate_drush_path` in the instructions with ours `environments_drush`.
In fact, if you already have Migrate configured, we will borrow from their config to initialize ours when you enable the
module so you won't have to do anything!

## Suggested Usage

The optimal way of working with Environments, particularly when maintaining multiple sites, each with their own somewhat
yet not entirely similar set of modules, is to leverage Task Bundles to create smaller, more manageable sets of tasks
targetted at a particular functionality, module or module suite:

		AdvAgg Production Bundle
		 	* Enable AdvAgg + submodules
		 	* Enable JS Compression using JSMin extension

		AdvAgg Staging Bundle
		 	* Enable AdvAgg + submodules
		 	* Enable JS Compression using JSMin+

		AdvAgg Development Bundle
		 	* Disable AdvAgg + submodules

The above example is very simple but it demonstrates reasonably well how you may apply different settings to different
environments in a real-life situation.

Grouping your tasks into targetted bundles also allows them to be more easily re-used amongst other projects where
there might be small differences (ie: no CDN) and that alone means for example that the "Production" environment of a
site with a CDN couldn't be shared with a site that didn't use it.

However, using bundles we can simple add the bundles we want to our environments and manage tasks in a much more
streamlined manner:

		Production Environment Tasks
			* Execute "AdvAgg Production Bundle"
			* Execute "CDN Production Bundle"
			* Execute "Disable Debug Bundle"
			* Execute "Enable Caching Bundle"

		Development Environment Tasks
			* Execute "AdvAgg Development Bundle"
			* Execute "CDN Development Bundle"
			* Execute "Enable Debug Bundle"
			* Execute "Disable Caching Bundle"

## Environments API

If you are familiar with CTools Plugins, creating a new Environments Task should be pretty straight-forwards!

`@TODO write this for now check out the existing tasks! :)`

## Videos

* [Environments Intro/Overview](https://vimeo.com/121030324)
