// Please add a license header here and change the copyright
// to your name.
//
// Copyright Inge Wallin
// Copyright Fredrik Edemar

#ifndef __SVGEXPORT_H__
#define __SVGEXPORT_H__

#include <koFilter.h>

class SvgExport : public KoFilter
{
    Q_OBJECT

public:
    SvgExport(KoFilter *parent, const char *name, const QStringList&);
    virtual ~SvgExport();

    virtual KoFilter::ConversionStatus convert(const QCString& from, const QCString& to);
};

#endif // __SVGEXPORT_H__
