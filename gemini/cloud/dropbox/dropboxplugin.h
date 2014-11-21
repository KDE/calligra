/*
 *
 */

#ifndef DROPBOXPLUGIN_H
#define DROPBOXPLUGIN_H

#include <QDeclarativeExtensionPlugin>

class DropboxPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char* uri);
    virtual void initializeEngine(QDeclarativeEngine* engine, const char* uri);
};

Q_EXPORT_PLUGIN2(calligrageminidropboxplugin, DropboxPlugin)

#endif // DROPBOXPLUGIN_H
