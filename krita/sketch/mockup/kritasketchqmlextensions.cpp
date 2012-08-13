#include "constants.h"
#include "settings.h"
#include "presetimageprovider.h"
#include "kritasketchqmlextensions.h"

#include <qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

KritaSketchQmlExtensions::KritaSketchQmlExtensions(QObject *parent) :
    QDeclarativeExtensionPlugin(parent)
{
}

void KritaSketchQmlExtensions::registerTypes(const char *uri)
{
    Q_UNUSED(uri)
}

void KritaSketchQmlExtensions::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri)
    engine->addImageProvider(QLatin1String("presetthumb"), new PresetImageProvider);
    engine->rootContext()->setContextProperty( "Constants", new Constants( engine ) );
    engine->rootContext()->setContextProperty( "Settings", new Settings( engine ) );
}

Q_EXPORT_PLUGIN2(kritasketchqmlextensions, KritaSketchQmlExtensions)

#include "kritasketchqmlextensions.moc"
