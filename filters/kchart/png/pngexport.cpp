// Please add a license header here and change the copyright
// to your name.

#include <qpixmap.h>
#include <qpainter.h>

#include <kmessagebox.h>

#include <koFilterChain.h>
#include <koStore.h>
//#include <koStoreDevice.h>
#include <kgenericfactory.h>

#include "kchart_part.h"

#include "pngexport.h"
#include <exportsizedia.h>

typedef KGenericFactory<PngExport, KoFilter> PngExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkchartpngexport, PngExportFactory( "pngexport" ) )

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
    // Check for proper conversion.
    if ( from != "application/x-kchart" || to != "image/png" )
        return KoFilter::NotImplemented;

    // Read the contents of the KChart file
    KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );
    if ( !storeIn ) {
	KMessageBox::error( 0, i18n("Failed to read data." ), 
			    i18n( "PNG Export Error" ) );
	return KoFilter::FileNotFound;
    }

    // Get the XML tree.
    QDomDocument  domIn;
    domIn.setContent( storeIn );
    QDomElement   docNode = domIn.documentElement();

    // Read the document from the XML tree.
    KChart::KChartPart  kchartDoc;
    if ( !kchartDoc.loadXML(0, domIn) ) {
        KMessageBox::error( 0, i18n( "Malformed XML data." ), 
			    i18n( "PNG Export Error" ) );
        return KoFilter::WrongFormat;
    }

    // Draw the actual bitmap.
    ExportSizeDia  *exportDialog = new ExportSizeDia(500, 400, 
						   0, "exportdialog");
    if (exportDialog->exec()) {
	int  width  = exportDialog->width();
	int  height = exportDialog->height();

	kdDebug() << "PNG Export: size = [" << width << "," << height << "]" << endl;
	QPixmap   pixmap(width, height);
	QPainter  painter(&pixmap);
	kchartDoc.paintContent(painter, pixmap.rect(), false);

	// Save the image.
	if ( !pixmap.save( m_chain->outputFile(), "PNG" ) ) {
	    KMessageBox::error( 0, i18n( "Failed to write file." ), 
				i18n( "PNG Export Error" ) );
	}
    }
    else {
	kdDebug() << "PNG Export: Couldn't run export dialog." << endl;
    }
    delete  exportDialog;
    return KoFilter::OK;
}


#include "pngexport.moc"

