import pykrita

@pateEventHandler('_pluginLoaded')
def on_load(plugin):
    if plugin in init.functions:
        # Call registered init functions for the plugin
        init.fire(plugin=plugin)
        del init.functions[plugin]
    return True


@pateEventHandler('_pluginUnloading')
def on_unload(plugin):
    if plugin in unload.functions:
        # Deinitialize plugin
        unload.fire(plugin=plugin)
        del unload.functions[plugin]
    return True


@pateEventHandler('_pykritaLoaded')
def on_pate_loaded():
    kDebug('PYKRITA LOADED')
    return True


@pateEventHandler('_pykritaUnloading')
def on_pate_unloading():
    kDebug('UNLOADING PYKRITA')
    return True

