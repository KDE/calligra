#include <mswordimport.h>

#include <qdom.h>
#include <qfontinfo.h>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <koFilterChain.h>
#include <koGlobal.h>

#include <document.h>

typedef KGenericFactory<MSWordImport, KoFilter> MSWordImportFactory;
K_EXPORT_COMPONENT_FACTORY( libmswordimport, MSWordImportFactory( "mswordimport" ) );

MSWordImport::MSWordImport( KoFilter *, const char *, const QStringList& ) : KoFilter()
{
}

MSWordImport::~MSWordImport()
{
}

KoFilter::ConversionStatus MSWordImport::convert( const QCString& from, const QCString& to )
{
    // check for proper conversion
    if ( to != "application/x-kword" || from != "application/msword" )
        return KoFilter::NotImplemented;

    kdDebug() << "######################## MSWordImport::convert ########################" << endl;

    QDomDocument mainDocument;
    QDomElement mainFramesetElement;
    prepareDocument( mainDocument, mainFramesetElement );

    Document document( m_chain->inputFile().ascii(), mainDocument, mainFramesetElement );
    document.parse();

    KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
    if ( !out ) {
        kdError(30502) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    QCString cstr = mainDocument.toCString();
    // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
    out->writeBlock( cstr, cstr.length() );

    kdDebug() << "######################## MSWordImport::convert ########################" << endl;
    return KoFilter::OK;
}

void MSWordImport::prepareDocument( QDomDocument& mainDocument, QDomElement& mainFramesetElement )
{
    // TODO: other paper formats
    KoFormat paperFormat = PG_DIN_A4;
    KoOrientation paperOrientation = PG_PORTRAIT;

    mainDocument.appendChild( mainDocument.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement elementDoc;
    elementDoc=mainDocument.createElement("DOC");
    elementDoc.setAttribute("editor","KWord's MS Word Import Filter");
    elementDoc.setAttribute("mime","application/x-kword");
    // TODO: We claim to be syntax version 2, but we should verify that it is also true.
    elementDoc.setAttribute("syntaxVersion",2);
    mainDocument.appendChild(elementDoc);

    QDomElement element;
    element=mainDocument.createElement("ATTRIBUTES");
    element.setAttribute("processing",0);
    element.setAttribute("standardpage",1);
    element.setAttribute("hasHeader",0);
    element.setAttribute("hasFooter",0);
    element.setAttribute("unit","mm");
    elementDoc.appendChild(element);

    QDomElement elementPaper=mainDocument.createElement("PAPER");
    elementPaper.setAttribute("format",paperFormat);
    elementPaper.setAttribute("width" ,KoPageFormat::width (paperFormat,paperOrientation) * 72.0 / 25.4);
    elementPaper.setAttribute("height",KoPageFormat::height(paperFormat,paperOrientation) * 72.0 / 25.4);
    elementPaper.setAttribute("orientation",PG_PORTRAIT);
    elementPaper.setAttribute("columns",1);
    elementPaper.setAttribute("columnspacing",2);
    elementPaper.setAttribute("hType",0);
    elementPaper.setAttribute("fType",0);
    elementPaper.setAttribute("spHeadBody",9);
    elementPaper.setAttribute("spFootBody",9);
    elementPaper.setAttribute("zoom",100);
    elementDoc.appendChild(elementPaper);

    element=mainDocument.createElement("PAPERBORDERS");
    element.setAttribute("left",28);
    element.setAttribute("top",42);
    element.setAttribute("right",28);
    element.setAttribute("bottom",42);
    elementPaper.appendChild(element);

    QDomElement framesetsPluralElementOut=mainDocument.createElement("FRAMESETS");
    mainDocument.documentElement().appendChild(framesetsPluralElementOut);

    mainFramesetElement=mainDocument.createElement("FRAMESET");
    mainFramesetElement.setAttribute("frameType",1);
    mainFramesetElement.setAttribute("frameInfo",0);
    mainFramesetElement.setAttribute("autoCreateNewFrame",1);
    mainFramesetElement.setAttribute("removable",0);
    // TODO: "name" attribute (needs I18N)
    framesetsPluralElementOut.appendChild(mainFramesetElement);

    QDomElement frameElementOut=mainDocument.createElement("FRAME");
    frameElementOut.setAttribute("left",28);
    frameElementOut.setAttribute("top",42);
    frameElementOut.setAttribute("bottom",566);
    frameElementOut.setAttribute("right",798);
    frameElementOut.setAttribute("runaround",1);
    mainFramesetElement.appendChild(frameElementOut);

    QDomElement elementStylesPlural=mainDocument.createElement("STYLES");
    elementDoc.appendChild(elementStylesPlural);

    QDomElement elementStyleStandard=mainDocument.createElement("STYLE");
    elementStylesPlural.appendChild(elementStyleStandard);

    element=mainDocument.createElement("NAME");
    element.setAttribute("value","Standard");
    elementStyleStandard.appendChild(element);

    element=mainDocument.createElement("FOLLOWING");
    element.setAttribute("name","Standard");
    elementStyleStandard.appendChild(element);

    QDomElement elementFormat=mainDocument.createElement("FORMAT");
    elementStyleStandard.appendChild(elementFormat);

    // Use QFontInfo, as it does not give back -1 as point size.
    QFontInfo defaultFontInfo(KoGlobal::defaultFont());

    element=mainDocument.createElement("FONT");
    element.setAttribute("name",defaultFontInfo.family());
    elementFormat.appendChild(element);

    element=mainDocument.createElement("SIZE");
    element.setAttribute("value",defaultFontInfo.pointSize());
    elementFormat.appendChild(element);

    element=mainDocument.createElement("ITALIC");
    element.setAttribute("value",0);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("WEIGHT");
    element.setAttribute("value",50);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("UNDERLINE");
    element.setAttribute("value",0);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("STRIKEOUT");
    element.setAttribute("value",0);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("VERTALIGN");
    element.setAttribute("value",0);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("COLOR");
    element.setAttribute("red",  0);
    element.setAttribute("green",0);
    element.setAttribute("blue", 0);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("TEXTBACKGROUNDCOLOR");
    element.setAttribute("red",  255);
    element.setAttribute("green",255);
    element.setAttribute("blue", 255);
    elementFormat.appendChild(element);
}

#include <mswordimport.moc>
