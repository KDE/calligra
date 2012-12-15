#include "SaveCoverImage.h"
#include <KoStoreDevice.h>

#include <QBuffer>
#include <QFile>
#include <kdebug.h>
#include <kmimetype.h>

QByteArray SaveCoverImage::m_coverData;
QString SaveCoverImage::m_coveMimType;

SaveCoverImage::SaveCoverImage()
{
}

bool SaveCoverImage::saveCoveImageData(KoStore *store)
{
    if (m_coverData.isEmpty())
        return true;

    if (!store->open("Author profile/cover." + m_coveMimType)) {
        kDebug(31000) << "Couldn't to open" << "Author profile/cover."<<m_coveMimType;
        return false;
    }

    KoStoreDevice device(store);
    device.write(m_coverData, m_coverData.size());
    store->close();

    kDebug(31000) <<"############" <<m_coverChanged;
    return true;
}

void SaveCoverImage::setCoverData(QString path)
{
    kDebug(31000) << "#### Set Cover data";
    QFile file (path);
    if (!file.open(QIODevice::ReadOnly)) {
        kDebug(31000) << "Couldn't to open" << path;
    }
    QByteArray data = file.readAll();
    QString mimtype = path.right(3);
    m_coverData = data;
    kDebug(31000) << "####" <<mimtype;
    m_coverChanged = 2;
    file.close();
}


