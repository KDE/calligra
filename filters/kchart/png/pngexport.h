// Please add a license header here and change the copyright
// to your name.
//
// Copyright Inge Wallin

#ifndef __PNGEXPORT_H__
#define __PNGEXPORT_H__

#include <koFilter.h>

class PngExport : public KoFilter
{
    Q_OBJECT

public:
    PngExport(KoFilter *parent, const char *name, const QStringList&);
    virtual ~PngExport();

    virtual KoFilter::ConversionStatus convert(const QCString& from, const QCString& to);
};

#endif // __PNGEXPORT_H__

