// Please add a license header here and change the copyright
// to your name.

#ifndef MSWORDIMPORT_H
#define MSWORDIMPORT_H

#include <koFilter.h>

class QDomDocument;
class QDomElement;

class MSWordImport : public KoFilter
{
    Q_OBJECT
public:
    MSWordImport( KoFilter* parent, const char* name, const QStringList& );
    virtual ~MSWordImport();

    virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

private:
    void prepareDocument( QDomDocument& mainDocument, QDomElement& mainFramesetElement );
};

#endif // MSWORDIMPORT_H
