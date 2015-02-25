Environments module
---------------------

This module attempts to solve the problem of executing repetive tasks when
switching between different server environments, such as development, staging
or production.

It defines 2 new concepts:

* Environments
* Tasks

------------
Environments
------------

Environments can be managed in: Administration -> Configuration -> Development -> Environments

An environment consists of a name, optional alias, configuration options and tasks.

You can view the current environment in your site's Status Report or in the
settings page: Administration -> Configuration -> System -> Site Environment

You can manually switch your site's active environment at any given time either
in the settings interface or using the Drush command: "drush environments", or
"drush env" for short.

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
