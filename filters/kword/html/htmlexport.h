#ifndef HTMLEXPORT_H
#define HTMLEXPORT_H

#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qobject.h>

#include <kdebug.h>
#include <koFilter.h>
#include <koTarStore.h>

#include <kword_xml2html.h>


class HTMLExport : public KoFilter {

    Q_OBJECT

public:
    HTMLExport(KoFilter *parent, QString name);
    virtual ~HTMLExport() {}
    /** filtering :) */
    virtual const bool filter(const QCString &fileIn, const QCString &fileOut,
                              const QCString& from, const QCString& to);

};
#endif // HTMLExport_H
