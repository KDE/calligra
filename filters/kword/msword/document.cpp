#include <document.h>

#include <kdebug.h>
#include <ustring.h>
#include <word97_generated.h>
#include <parser.h>
#include <parserfactory.h>


wvWare::U8 KWordCharacterHandler::hardLineBreak()
{
    return '\n';
}

wvWare::U8 KWordCharacterHandler::nonBreakingHyphen()
{
    return '-'; // normal hyphen for now
}

wvWare::U8 KWordCharacterHandler::nonRequiredHyphen()
{
    return 0xad; // soft hyphen, according to kword.dtd
}


Document::Document( const std::string& fileName, QDomDocument& mainDocument, QDomElement& mainFramesetElement )
    : m_mainDocument( mainDocument ), m_mainFramesetElement( mainFramesetElement ), m_index( 0 ),
      m_charHandler( new KWordCharacterHandler ), m_parser( wvWare::ParserFactory::createParser( fileName ) )
{
    if ( m_parser ) {
        m_parser->setSpecialCharacterHandler( m_charHandler );
        m_parser->setBodyTextHandler( this );
    }
}

Document::~Document()
{
    delete m_charHandler;
}

bool Document::parse()
{
    if ( m_parser )
        return m_parser->parse();
    return false;
}

void Document::paragraphStart( wvWare::SharedPtr<const wvWare::Word97::PAP> pap )
{
    m_formats = m_mainDocument.createElement( "FORMATS" );
    m_pap = pap;
}

void Document::paragraphEnd()
{
    // TODO: get style name (and properties) from pap.istd, in paragraphStart
    writeOutParagraph( "Standard", m_paragraph );
}

void Document::runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp )
{
    m_paragraph += QString( reinterpret_cast<const QChar*>( text.data() ), text.length() );

    QDomElement format( m_mainDocument.createElement( "FORMAT" ) );
    format.setAttribute( "id", 1 );
    format.setAttribute( "pos", m_index );
    format.setAttribute( "len", text.length() );

    // TODO: change the if()s below, to add attributes if different from paragraph format
    // (not if different from 'plain text')
    // This is also why the code below seems to test stuff twice ;)

    // TODO: COLOR  .... see chp.ico where to put the conversion code? here or in wv2?
    // TODO: FONT

    QDomElement fontSize( m_mainDocument.createElement( "SIZE" ) );
    fontSize.setAttribute( "value", (int)(chp->hps / 2) ); // hps is in half points
    format.appendChild( fontSize );

    if ( chp->fBold ) {
        QDomElement weight( m_mainDocument.createElement( "WEIGHT" ) );
        weight.setAttribute( "value", chp->fBold ? 75 : 50 );
        format.appendChild( weight );
    }
    if ( chp->fItalic ) {
        QDomElement italic( m_mainDocument.createElement( "ITALIC" ) );
        italic.setAttribute( "value", chp->fItalic ? 1 : 0 );
        format.appendChild( italic );
    }
    if ( chp->kul ) {
        QDomElement underline( m_mainDocument.createElement( "UNDERLINE" ) );
        QString val = (chp->kul == 0) ? "0" : "1";
        switch ( chp->kul ) {
        case 3: // double
            underline.setAttribute( "styleline", "solid" );
            val = "double";
            break;
        case 6: // thick
            underline.setAttribute( "styleline", "solid" );
            val = "single-bold";
            break;
        case 7:
            underline.setAttribute( "styleline", "dash" );
            break;
        case 4: // dotted
        case 8: // dot (not used, says the docu)
            underline.setAttribute( "styleline", "dot" );
            break;
        case 9:
            underline.setAttribute( "styleline", "dashdot" );
            break;
        case 10:
            underline.setAttribute( "styleline", "dashdotdot" );
            break;
        case 1: // single
        case 2: // by word - TODO in kword
        case 5: // hidden - WTH is that?
        case 11: // wave - not in kword
        default:
            underline.setAttribute( "styleline", "solid" );
        };
        underline.setAttribute( "value", val );
        format.appendChild( underline );
    }
    if ( chp->fStrike || chp->fDStrike ) {
        QDomElement strikeout( m_mainDocument.createElement( "STRIKEOUT" ) );
        if ( chp->fDStrike ) // double strikethrough
        {
            strikeout.setAttribute( "value", "double" );
            strikeout.setAttribute( "styleline", "solid" );
        }
        else if ( chp->fStrike )
        {
            strikeout.setAttribute( "value", "single" );
            strikeout.setAttribute( "styleline", "solid" );
        }
        else
            strikeout.setAttribute( "value", "0" );
        format.appendChild( strikeout );
    }

    if ( chp->iss ) { // superscript/subscript
        QDomElement vertAlign( m_mainDocument.createElement( "VERTALIGN" ) );
        // Obviously the values are reversed between the two file formats :)
        int kwordVAlign = (chp->iss==1 ? 2 : chp->iss==2 ? 1 : 0);
        vertAlign.setAttribute( "value", kwordVAlign );
        format.appendChild( vertAlign );
    }
    // TODO: TEXTBACKGROUNDCOLOR - what's the msword name for it? Can't find it in the CHP.
    // ## Problem with fShadow. Char property in MSWord, parag property in KWord at the moment....

    if ( !format.firstChild().isNull() ) // Don't save an empty format tag
        m_formats.appendChild( format );

    m_index += text.length();
}

void Document::pageBreak()
{
    // Check if PAGEBREAKING already exists (e.g. due to linesTogether)
    QDomElement pageBreak = m_oldLayout.namedItem( "PAGEBREAKING" ).toElement();
    if ( pageBreak.isNull() )
    {
        pageBreak = m_mainDocument.createElement( "PAGEBREAKING" );
        m_oldLayout.appendChild( pageBreak );
    }
    pageBreak.setAttribute( "hardFrameBreakAfter", "true" );
}

void Document::writeOutParagraph( const QString& styleName, const QString& text )
{
    QDomElement paragraphElementOut=m_mainDocument.createElement("PARAGRAPH");
    m_mainFramesetElement.appendChild(paragraphElementOut);
    QDomElement textElement=m_mainDocument.createElement("TEXT");
    paragraphElementOut.appendChild(textElement);
    paragraphElementOut.appendChild( m_formats );
    QDomElement layoutElement=m_mainDocument.createElement("LAYOUT");
    paragraphElementOut.appendChild(layoutElement);

    QDomElement nameElement = m_mainDocument.createElement("NAME");
    nameElement.setAttribute("value", styleName);
    layoutElement.appendChild(nameElement);

    if ( m_pap ) {
        QDomElement flowElement = m_mainDocument.createElement("FLOW");
        QString alignment( "left" );
        if ( m_pap->jc == 1 )
            alignment = "center";
        else if ( m_pap->jc == 2 )
            alignment = "right";
        else if ( m_pap->jc == 3 )
            alignment = "justify";
        flowElement.setAttribute("align",alignment);
        layoutElement.appendChild(flowElement);

        // TODO: INDENTS dxaRight dxaLeft dxaLeft1 - in which unit are those?
        // TODO: OFFSETS dyaBefore dyaAfter
        // TODO: LINESPACING lspd
        if ( m_pap->fKeep || m_pap->fKeepFollow || m_pap->fPageBreakBefore )
        {
            QDomElement pageBreak = m_mainDocument.createElement( "PAGEBREAKING" );
            pageBreak.setAttribute("linesTogether", m_pap->fKeep ? "true" : "false");
            pageBreak.setAttribute("hardFrameBreak", m_pap->fPageBreakBefore ? "true" : "false");
            pageBreak.setAttribute("keepWithNext", m_pap->fKeepFollow ? "true" : "false");
            layoutElement.appendChild( pageBreak );
        }

        // TODO: LEFTBORDER|RIGHTBORDER|TOPBORDER|BOTTOMBORDER  - see pap.brcl brcTop brcLeft brcBottom brcRight

        // TODO: COUNTER
        // TODO: FORMAT - unless it all comes from the style
        // TODO: SHADOW [it comes from the text runs...]
        // TODO: TABULATORs itbdMac? (why "Mac"?) rgdxaTab[] rgtbd[]
    }

    textElement.appendChild(m_mainDocument.createTextNode(text));
    //textElement.normalize(); // Put text together (not sure if needed)
    // DF: I don't think so, you created only one text node ;)

    m_paragraph = QString( "" );
    m_index = 0;
    m_oldLayout = layoutElement; // Keep a reference to the old layout for some hacks
}
