#ifndef OLEFILTER_H
#define OLEFILTER_H

#include <string.h>
#include <qobject.h>
#include <qstring.h>
#include <qlist.h>
#include <qarray.h>
#include <qfile.h>
#include <klaola.h>
#include <filterbase.h>
#include <wordfilter.h>
#include <excelfilter.h>
#include <myfile.h>


class OLEFilter : public QObject {

    Q_OBJECT

public:

    enum IN { Word, Excel, PowerPoint };
    enum OUT { KWord, KSpread, KPresenter };

    OLEFilter(const myFile &in, const IN i, const OUT o);
    ~OLEFilter();

    const bool isOk() { return success; }
    const bool filter();            // manages the filtering process
    const QString part() { return myFilter->part(); } // returns the file

protected slots:
    void slotSavePic(const char *data, const char *type, const unsigned long size,
                     char **nameOUT);
    void slotGetStream(const long &handle, myFile &stream);
    // Note: might return wrong stream as names are NOT unique!!!
    // (searching only in current dir)
    void slotGetStream(const QString &name, myFile &stream);

private:
    OLEFilter(const OLEFilter &);
    const OLEFilter &operator=(const OLEFilter &);

    void convert();
    void connectCommon();

    myFile fileIn;                   // will hopefully be changed, soon (to filename only)
    IN in;                           // which file type?
    OUT out;                         // ditto
    FilterBase *myFilter;
    int numPic;                      // unique name generation ->
    KLaola *docfile;                 // used to split up the OLE-file
    bool success;                    // everything ok?
};
#endif // OLEFILTER_H
