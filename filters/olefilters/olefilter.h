#ifndef OLEFILTER_H
#define OLEFILTER_H

#include <string.h>
#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qarray.h>
#include <qfile.h>
#include <koFilter.h>
#include <koTarStore.h>
#include <klaola.h>
#include <filterbase.h>
#include <wordfilter.h>
#include <excelfilter.h>
#include <myfile.h>


class OLEFilter : public KoFilter {

    Q_OBJECT

public:
    OLEFilter(KoFilter *parent, QString name);
    virtual ~OLEFilter();

    virtual const bool filter(const QCString &fileIn, const QCString &fileOut,
                              const QCString& from, const QCString& to);
    
protected slots:
    void slotSavePic(const char *data, const char *type, const unsigned long size,
                     char **nameOUT);
    void slotPart(const char *nameIN, char **nameOUT);
    void slotGetStream(const long &handle, myFile &stream);
    // Note: might return wrong stream as names are NOT unique!!!
    // (searching only in current dir)
    void slotGetStream(const QString &name, myFile &stream);

private:
    OLEFilter(const OLEFilter &);
    const OLEFilter &operator=(const OLEFilter &);

    void convert(const QString &dirname);
    void connectCommon(FilterBase **myFilter);

    QMap<QString, QString> partMap;
    QArray<unsigned short> storePath;

    myFile olefile;
    int numPic;                      // unique name generation ->
    KLaola *docfile;                 // used to split up the OLE-file
    KoTarStore *store;               // KOffice Storage structure
    bool success;
};
#endif // OLEFILTER_H
