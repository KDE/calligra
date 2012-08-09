#include "presetimageprovider.h"

#include <ui/kis_resource_server_provider.h>

class PresetImageProvider::Private {
public:
    Private()
        : rserver(0)
    {}
    KoResourceServer<KisPaintOpPreset> * rserver;
};

PresetImageProvider::PresetImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

QImage PresetImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QImage image(requestedSize, QImage::Format_ARGB32);
    return image;
}
