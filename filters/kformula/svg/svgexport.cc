// Please add a license header here and change the copyright
// to your name.
// Inge Wallin
// Fredrik Edemar

#include <qpicture.h>
#include <qpainter.h>

#include <kapplication.h>
#include <kmessagebox.h>

#include <koFilterChain.h>
#include <koStore.h>
//#include <koStoreDevice.h>
#include <kgenericfactory.h>

#include <kformulacontainer.h>
#include "kformuladocument.h"

#include "svgexport.h"


typedef KGenericFactory<SvgExport, KoFilter> SvgExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkfosvgexport, SvgExportFactory( "svgexport" ) )

SvgExport::SvgExport(KoFilter *, const char *, const QStringList&) 
    : KoFilter()
{
}

SvgExport::~SvgExport()
{
}


KoFilter::ConversionStatus
SvgExport::convert(const QCString& from, const QCString& to)
{
    // Check for proper conversion.
    if ( from != "application/x-kformula" || to != "image/svg+xml" )
        return KoFilter::NotImplemented;

    // Read the contents of the KFormula file
    KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );
    if ( !storeIn ) {
	KMessageBox::error( 0, i18n("Failed to read data." ), 
			    i18n( "SVG Export Error" ) );
	return KoFilter::FileNotFound;
    }

    // Get the XML tree.
    QDomDocument  domIn;
    domIn.setContent( storeIn );
    QDomElement   docNode = domIn.documentElement();

    // Read the document from the XML tree.
    KFormula::DocumentWrapper* wrapper = new KFormula::DocumentWrapper( kapp->config(), 0 );
    KFormula::Document* kformulaDoc = new KFormula::Document;
    wrapper->document( kformulaDoc );
    KFormula::Container* formula = kformulaDoc->createFormula();

    if ( !kformulaDoc->loadXML( domIn ) ) {
        KMessageBox::error( 0, i18n( "Malformed XML data." ), 
			    i18n( "SVG Export Error" ) );
        return KoFilter::WrongFormat;
    }

    // Draw the actual bitmap.
    QPicture  picture;
    QPainter  painter(&picture);
    QRect     rect(QPoint(0, 0), QPoint(500, 400));
    formula->draw( painter, rect, false );
    painter.end();

    // Save the image.
    if ( !picture.save( m_chain->outputFile(), "SVG" ) ) {
        KMessageBox::error( 0, i18n( "Failed to write file." ), 
			    i18n( "SVG Export Error" ) );
    }

    delete formula;
    delete wrapper;
    return KoFilter::OK;
}


#include <svgexport.moc>
