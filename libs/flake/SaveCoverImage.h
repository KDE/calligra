#ifndef SAVECOVERIMAGE_H
#define SAVECOVERIMAGE_H

#include <QObject>

class KoStore;

class SaveCoverImage
{
public:
    SaveCoverImage();
    bool saveCoveImageData(KoStore *store);
    //bool hasCoverImage();
};

#endif // SAVECOVERIMAGE_H
