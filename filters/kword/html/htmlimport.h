#ifndef HTMLIMPORT_H
#define HTMLIMPORT_H

#include <qstring.h>
#include <qfile.h>
#include <qobject.h>

#include <kdebug.h>
#include <koFilter.h>
#include <koTarStore.h>


class HTMLImport : public KoFilter {

    Q_OBJECT

public:
    HTMLImport(KoFilter *parent, QString name);
    virtual ~HTMLImport() {}
    /** filtering :) */
    virtual const bool filter(const QCString &fileIn, const QCString &fileOut,
                              const QCString& from, const QCString& to);

};
#endif // HTMLIMPORT_H
