#ifndef KRITASKETCHQMLEXTENSIONS_H
#define KRITASKETCHQMLEXTENSIONS_H

#include <QDeclarativeExtensionPlugin>
class KritaSketchQmlExtensions : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    explicit KritaSketchQmlExtensions(QObject *parent = 0);
    void registerTypes(const char *uri);
    void initializeEngine(QDeclarativeEngine *engine, const char *uri);
};

#endif // KRITASKETCHQMLEXTENSIONS_H
