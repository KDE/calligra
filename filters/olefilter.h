#ifndef OLEFILTER_H
#define OLEFILTER_H

#include <string.h>
#include <qobject.h>
#include <qlist.h>
#include <qmap.h>
#include <qstring.h>
#include "klaola.h"
#include "ktar.h"
#include <filterbase.h>
#include <wordfilter.h>
#include <myfile.h>


class OLEFilter : public QObject {

    Q_OBJECT

public:

    enum IN { Word, Excel, PowerPoint };
    enum OUT { KWord, KSpread, KPresenter };

    OLEFilter(const myFile &in, const QString &nameOut, const IN i, const OUT o);
    ~OLEFilter();

    const bool filter();            // manages the filtering process

protected slots:
    void slotSavePic(const char *data, const char *type, const unsigned int size,
                     char **nameOUT);
    void slotPart(const char *nameIN, const char *type, char **nameOUT);
    void slotGetStream(const long &handle, myFile &stream);
    // Note: might return wrong stream as names are NOT unique!!!
    // (searching only in current dir)
    void slotGetStream(const QString &name, myFile &stream);

private:
    void convert(const QString &dirname);   // do the real work, recursively
    void connectCommon(FilterBase **myFilter);

    myFile fileIn;                   // will hopefully be changed, soon (to filename only)
    QString fileOutName;             // path+name of the output file
    QString dir;                     // name of the main dir in the tgz-file
    QMap<QString, QString> partMap;  // all part-names in the OLE-file
                                     // "mapped" to the real ones (in the KTar-struct)
    IN in;                           // which file type?
    OUT out;                         // ditto
    int numPic;                      // unique name generation ->
    int numPart;                     // number for file (partX.yyy)
    KLaola *docfile;                 // used to split up the OLE-file
    KTar *fileOut;                   // used to create output-file
    bool success;                    // everything ok?
};
#endif // OLEFILTER_H
