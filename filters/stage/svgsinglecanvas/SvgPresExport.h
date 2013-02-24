#ifndef SVGPRESEXPORT_H
#define SVGPRESEXPORT_H

#include <KoFilter.h>
#include <QVariantList>

class SvgPresExport : public KoFilter
{
    Q_OBJECT

public:
    SvgPresExport(QObject* parent, const QVariantList&);
    virtual ~SvgPresExport() {}

    virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);

private:
    void saveDocument();
};

#endif
