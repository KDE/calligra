Deferred plugin loading

Generally, plugins are loaded by searching for all plugins
of a certain type. Some plugins come in two parts, the 
"real" plugin and a deferred plugin. The real plugin
is loaded and then on demand loads the deferred plugin.

The deferred plugin is of the calligradeferredplugin type
and will be searched by looking for a plugin of a certain
name of this type.
