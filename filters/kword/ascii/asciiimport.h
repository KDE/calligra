#ifndef ASCIIIMPORT_H
#define ASCIIIMPORT_H

#include <qstring.h>
//#include <qtextstream.h>
#include <qfile.h>
#include <qobject.h>

#include <kdebug.h>
#include <koFilter.h>
#include <koTarStore.h>


class ASCIIImport : public KoFilter {

    Q_OBJECT

public:
    ASCIIImport(KoFilter *parent, QString name);
    virtual ~ASCIIImport() {}
    /** filtering :) */
    virtual const bool filter(const QCString &fileIn, const QCString &fileOut,
                              const QCString& from, const QCString& to);

};
#endif // ASCIIIMPORT_H
