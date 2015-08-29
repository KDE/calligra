/*
 *
 */

#ifndef DROPBOXPLUGIN_H
#define DROPBOXPLUGIN_H

#include <QDeclarativeExtensionPlugin>

class DropboxPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.calligra.DropboxPlugin")

public:
    virtual void registerTypes(const char* uri);
    virtual void initializeEngine(QDeclarativeEngine* engine, const char* uri);
};

#endif // DROPBOXPLUGIN_H
