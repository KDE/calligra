// Please add a license header here and change the copyright
// to your name.

#include <koFilterChain.h>
#include <kgenericfactory.h>

#include <pngexport.h>


typedef KGenericFactory<PngExport, KoFilter> PngExportFactory;
K_EXPORT_COMPONENT_FACTORY( libpngexport, PngExportFactory( "pngexport" ) );

PngExport::PngExport(KoFilter *, const char *, const QStringList&) 
    : KoFilter()
{
}

PngExport::~PngExport()
{
}

KoFilter::ConversionStatus
PngExport::convert(const QCString& from, const QCString& to)
{
    // check for proper conversion
    if ( from != "application/x-kchart" || to != "image/png" )
        return KoFilter::NotImplemented;

	

    // Do the conversion stuff here. For notes how to get the input/output
    // locations please refer to koffice/lib/kofficecore/koFilterChain.h

    return KoFilter::NotImplemented; // Change to KoFilter::OK if the conversion
                                     // was successfull
}

#include <pngexport.moc>
