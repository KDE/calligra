#ifndef ASCIIEXPORT_H
#define ASCIIEXPORT_H

#include <qstring.h>
#include <qcstring.h>
#include <qfile.h>
#include <qobject.h>

#include <kdebug.h>
#include <koFilter.h>
#include <koTarStore.h>


class ASCIIExport : public KoFilter {

    Q_OBJECT

public:
    ASCIIExport(KoFilter *parent, QString name);
    virtual ~ASCIIExport() {}
    /** filtering :) */
    virtual const bool filter(const QCString &fileIn, const QCString &fileOut,
                              const QCString& from, const QCString& to);

};
#endif // ASCIIExport_H
