/*
 *
 */

#ifndef DROPBOXPLUGIN_H
#define DROPBOXPLUGIN_H

#include <QQmlExtensionPlugin>

class DropboxPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.calligra.DropboxPlugin")

public:
    void registerTypes(const char* uri) override;
    void initializeEngine(QQmlEngine* engine, const char* uri) override;
};

#endif // DROPBOXPLUGIN_H
