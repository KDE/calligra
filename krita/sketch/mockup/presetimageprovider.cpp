#include "presetimageprovider.h"

#include <KoResourceServerAdapter.h>
#include <ui/kis_resource_server_provider.h>
#include <image/brushengine/kis_paintop_preset.h>

class PresetImageProvider::Private {
public:
    Private()
    {
        rserver = KisResourceServerProvider::instance()->paintOpPresetServer();
    }

    KoResourceServer<KisPaintOpPreset> * rserver;
};

PresetImageProvider::PresetImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

QImage PresetImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QImage image(requestedSize, QImage::Format_ARGB32);
    QList<KisPaintOpPreset*> resources = d->rserver->resources();
    int theID = id.toInt();
    if(theID >= 0 && theID < resources.count())
    {
        image = resources.at(theID)->image().scaled(requestedSize, Qt::KeepAspectRatio);
    }
    return image;
}
