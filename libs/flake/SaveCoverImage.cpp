#include "SaveCoverImage.h"
#include <KoStoreDevice.h>

#include <QBuffer>
#include <QFile>
#include <kdebug.h>

SaveCoverImage::SaveCoverImage()
{
}

bool SaveCoverImage::saveCoveImageData(KoStore *store)
{
    QString path = "/home/moji/index2.jpg";
    QFile file (path);
    if (!file.open(QIODevice::ReadOnly)) {
        kDebug(31000) << "Couldn't to open" << path;
        return false;
    }
    QByteArray imageData = file.readAll();

    QString coverPath = "cover.jpg";
    if (!store->open("Author profile/" + coverPath)) {
        kDebug(31000) << "Couldn't to open" << "Author profile/" + coverPath;
        return false;
    }
    KoStoreDevice device(store);
    device.write(imageData, imageData.size());
    store->close();
    return true;
}
