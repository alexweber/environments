Environments module
---------------------

This module attempts to solve the problem of executing repetive tasks when
switching between different server environments, such as development, staging
or production.

It defines 3 new concepts:

* Environments
* Tasks
* Task Bundles

------------
Environments
------------

Environments can be managed in:

		Administration -> Configuration -> Development -> Environments

An environment consists of a name, optional alias, configuration options and tasks.

You can view the current environment in your site's Status Report or in the
settings page:

		Administration -> Configuration -> System -> Site Environment

You can manually switch your site's active environment at any given time either
in the settings interface or using the Drush command: "drush environments", or
"drush env" for short:

		drush environments --help
		drush env
		drush env dev

When an environment becomes active, it's tasks are executed in order.

------------
Tasks
------------

Environment Tasks are operations such as enabling or disabling a module that you
can execute when an environment becomes active.

Once you've created an environment you can configure it's tasks by going to it's
"Manage Tasks" page.

It's important to note that these tasks will run in the order that they appear
and that you can drag them around to re-arrange them.

------------
Task Bundles
------------

A "Task Bundle" is a group of tasks, bundled as a single unit. They work in a
similar manner as Environments, in that you can import/export them and manage
their tasks.

A Task Bundle may also be executed on its own, on demand, either using the UI
or using the "environments-bundle" Drush command:

		drush environments-bundle --help
		drush env-exec bundle_name

Finally, a Task Bundle can also be added to an Environment as a task itself, so
that it can be run and managed alongside other tasks.

------------
Drush
------------

In order to add a task that executes Drush commands, you must first configure
the path to Drush in the settings form:

		Administration -> Configuration -> System -> Site Environment

You must insert the absolute path to the executable and, if it contains spaces
you must manually escape them, for example:

		/Applications/Dev\ Desktop/drush/drush

The following documentation page from the Migrate module has pretty good
instructions for getting a very similar Drush integration working (this one is
based on theirs actually!):

 		https://www.drupal.org/node/1958170

All you have to do is replace the variable name "migrate_drush_path" in the
instructions with ours: "environments_drush". In fact, if you already have
Migrate configured, we will borrow from their config to initialize ours when
you enable the module.

------------
Suggested Usage
------------

The optimal way of working with Environments, particularly when maintaining
multiple sites, each with their own similar yet kinda different set of modules,
is to leverage Task Bundles to create smaller, more manageable sets of tasks
targetted at a particular functionality, module or module suite:

		AdvAgg Production Bundle
		 	* Enable AdvAgg + submodules
		 	* Enable JS Compression using JSMin extension

		AdvAgg Staging Bundle
		 	* Enable AdvAgg + submodules
		 	* Enable JS Compression using JSMin+

		AdvAgg Development Bundle
		 	* Disable AdvAgg + submodules

The above example is very simple but it demonstrates reasonably well how you
may apply different settings to different environments in a real-life situation.

Grouping your tasks into targetted bundles also allows them to be more easily
re-used amongst other projects where there might be small differences (ie: no
CDN) and that alone means for example that the "Production" environment of a
site with a CDN couldn't be shared with a site that didn't use it.

However, using bundles we can simple add the bundles we want to our environments
and manage tasks in a much more streamlined manner:

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
