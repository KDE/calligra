#include <document.h>

#include <kdebug.h>
#include <ustring.h>
#include <word97_generated.h>
#include <parser.h>


wvWare::U8 KWordCharacterHandler::hardLineBreak()
{
    kdDebug() << "KWordCharacterHandler::hardLineBreak" << endl;
    return '\n';
}

wvWare::U8 KWordCharacterHandler::nonBreakingHyphen()
{
    kdDebug() << "KWordCharacterHandler::nonBreakingHyphen" << endl;
    return '-'; // normal hyphen for now
}

wvWare::U8 KWordCharacterHandler::nonRequiredHyphen()
{
    kdDebug() << "KWordCharacterHandler::nonRequiredHyphen" << endl;
    return 0xad; // soft hyphen, according to kword.dtd
}


Document::Document( const std::string& fileName, QDomDocument& mainDocument, QDomElement& mainFramesetElement )
    : wvWare::LLDocument( fileName ), m_mainDocument( mainDocument ),
      m_mainFramesetElement( mainFramesetElement ), m_index( 0 )
{
    m_handler = new KWordCharacterHandler;
    parser()->setSpecialCharacterHandler( m_handler );
}

Document::~Document()
{
    delete m_handler;
}

void Document::paragraphStart( wvWare::SharedPtr<const wvWare::Word97::PAP> pap )
{
    m_formats = m_mainDocument.createElement( "FORMATS" );
    m_pap = pap;
}

void Document::paragraphEnd()
{
    writeOutParagraph( "Standard", m_paragraph );
}

void Document::runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp )
{
    m_paragraph += QString( reinterpret_cast<const QChar*>( text.data() ), text.length() );
    if ( chp->fBold || chp->fItalic || chp->kul != 0 ) {
        QDomElement format( m_mainDocument.createElement( "FORMAT" ) );
        format.setAttribute( "id", 1 );
        format.setAttribute( "pos", m_index );
        format.setAttribute( "len", text.length() );

        if ( chp->fBold ) {
            QDomElement weight( m_mainDocument.createElement( "WEIGHT" ) );
            weight.setAttribute( "value", 75 );
            format.appendChild( weight );
        }
        if ( chp->fItalic ) {
            QDomElement italic( m_mainDocument.createElement( "ITALIC" ) );
            italic.setAttribute( "value", 1 );
            format.appendChild( italic );
        }
        if ( chp->kul ) {
            QDomElement underline( m_mainDocument.createElement( "UNDERLINE" ) );
            underline.setAttribute( "styleline", "solid" );
            underline.setAttribute( "value", 1 );
            format.appendChild( underline );
        }
        m_formats.appendChild( format );
    }
    m_index += text.length();
}

void Document::pageBreak()
{
    QDomElement pageBreak = m_mainDocument.createElement( "PAGEBREAKING" );
    pageBreak.setAttribute( "hardFrameBreakAfter", "true" );
    m_oldLayout.appendChild( pageBreak );
}

void Document::writeOutParagraph( const QString& name, const QString& text )
{
    QDomElement paragraphElementOut=m_mainDocument.createElement("PARAGRAPH");
    m_mainFramesetElement.appendChild(paragraphElementOut);
    QDomElement textElement=m_mainDocument.createElement("TEXT");
    paragraphElementOut.appendChild(textElement);
    paragraphElementOut.appendChild( m_formats );
    QDomElement layoutElement=m_mainDocument.createElement("LAYOUT");
    paragraphElementOut.appendChild(layoutElement);

    QDomElement element;
    element=m_mainDocument.createElement("NAME");
    element.setAttribute("value",name);
    layoutElement.appendChild(element);

    QDomElement flowElement;
    flowElement=m_mainDocument.createElement("FLOW");
    QString alignment( "left" );
    if ( m_pap ) {
        if ( m_pap->jc == 1 )
            alignment = "center";
        else if ( m_pap->jc == 2 )
            alignment = "right";
        else if ( m_pap->jc == 3 )
            alignment = "justify";
    }
    flowElement.setAttribute("align",alignment);
    layoutElement.appendChild(flowElement);

    textElement.appendChild(m_mainDocument.createTextNode(text));
    textElement.normalize(); // Put text together (not sure if needed)

    m_paragraph = QString( "" );
    m_index = 0;
    m_oldLayout = layoutElement; // Keep a reference to the old layout for some hacks
}
