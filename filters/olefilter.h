#ifndef OLEFILTER_H
#define OLEFILTER_H

#include <qobject.h>
#include "klaola.h"
#include "ktar.h"
#include <wordfilter.h>
#include <myfile.h>

class OLEFilter : public QObject {

    Q_OBJECT

public:

    enum IN { Word, Excel, Powerpoint };
    enum OUT { KWord, KSpread, KPresenter };

    OLEFilter(const myFile &in, const QString &nameOut, const IN i, const OUT o);
    ~OLEFilter();

    const bool filter();            // manages the filtering process

protected slots:
    void slotSavePic(const char *data, const char *nameOUT);
    void slotPart(const char *nameIN, const char *nameOUT);

private:
    void convert(const QString &name);

    myFile fileIn;
    QString fileOutName;
    IN in;
    OUT out;
    KLaola *docfile;
    KTar *fileOut;
    bool success;
    //WordFilter *wordFilter;
};
#endif // OLEFILTER_H
