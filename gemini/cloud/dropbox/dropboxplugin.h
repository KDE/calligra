/*
 *
 */

#ifndef DROPBOXPLUGIN_H
#define DROPBOXPLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

class DropboxPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.calligra.DropboxPlugin")

public:
    virtual void registerTypes(const char* uri);
    virtual void initializeEngine(QQmlEngine* engine, const char* uri);
};

#endif // DROPBOXPLUGIN_H
