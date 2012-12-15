#ifndef SAVECOVERIMAGE_H
#define SAVECOVERIMAGE_H

#include <QObject>
#include <QHash>
#include "flake_export.h"

class KoStore;

class FLAKE_EXPORT SaveCoverImage
{
public:
    SaveCoverImage();
    bool saveCoveImageData(KoStore *store);
    //bool hasCoverImage();
    void setCoverData(QString path);

private:
    int m_coverChanged;
    static QByteArray m_coverData;
    static QString m_coveMimType;

};

#endif // SAVECOVERIMAGE_H
