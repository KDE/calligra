#include <document.h>

#include <kdebug.h>
#include <ustring.h>
#include <word97_generated.h>
#include <parser.h>
#include <parserfactory.h>
#include <qfont.h>
#include <qfontinfo.h>


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

    QColor color = colorForNumber( chp->ico, -1 );
    QDomElement colorElem( m_mainDocument.createElement( "COLOR" ) );
    colorElem.setAttribute( "red", color.red() );
    colorElem.setAttribute( "blue", color.blue() );
    colorElem.setAttribute( "green", color.green() );
    format.appendChild( colorElem );

    // Font name
    // TBD: We only use the Ascii font code. We should work out how/when to use the FE and Other font codes.
    QString fontName = getFont( chp->ftcAscii );

    if ( !fontName.isEmpty() )
    {
        QDomElement fontElem( m_mainDocument.createElement( "FONT" ) );
        fontElem.setAttribute( "name", fontName );
        format.appendChild( fontElem );
    }

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

//#define FONT_DEBUG

// Return the name of a font. We have to convert the Microsoft font names to
// something that might just be present under X11.
QString Document::getFont(unsigned fc) const
{
    Q_ASSERT( m_parser );
    if ( !m_parser )
        return QString::null;
    const wvWare::Word97::FFN* ffn = m_parser->font( fc );
    Q_ASSERT( ffn );
    if ( !ffn )
        return QString::null;

    QConstString fontName( (QChar *)ffn->xszFfn.data(), ffn->xszFfn.length() );
    QString font = fontName.string();

#ifdef FONT_DEBUG
    kdDebug() << "MS-FONT: " << font << endl;
#endif

    static const unsigned ENTRIES = 6;
    static const char* const fuzzyLookup[ENTRIES][2] =
    {
        // MS contains      X11 font family
        // substring.       non-AA name.
        { "times",          "times" },
        { "courier",        "courier" },
        { "andale",         "monotype" },
        { "monotype.com",   "monotype" },
        { "georgia",        "times" },
        { "helvetica",      "helvetica" }
    };

    // When Xft is available, Qt will do a good job of looking up our local
    // equivalent of the MS font. But, we want to work even without Xft.
    // So, first, we do a fuzzy match of some common MS font names.
    unsigned i;

    for (i = 0; i < ENTRIES; i++)
    {
        // The loop will leave unchanged any MS font name not fuzzy-matched.
        if (font.find(fuzzyLookup[i][0], 0, FALSE) != -1)
        {
            font = fuzzyLookup[i][1];
            break;
        }
    }

#ifdef FONT_DEBUG
    kdDebug() << "FUZZY-FONT: " << font << endl;
#endif

    // Use Qt to look up our canonical equivalent of the font name.
    QFont xFont( font );
    QFontInfo info( xFont );

#ifdef FONT_DEBUG
    kdDebug() << "QT-FONT: " << info.family() << endl;
#endif

    return info.family();
}

QColor Document::colorForNumber(int number, int defaultcolor, bool defaultWhite)
{
    switch(number)
    {
	case 0:
	    if(defaultWhite)
		return QColor("white");
	case 1:
	    return QColor("black");
	case 2:
	    return QColor("blue");
	case 3:
	    return QColor("cyan");
	case 4:
	    return QColor("green");
	case 5:
	    return QColor("magenta");
	case 6:
	    return QColor("red");
	case 7:
	    return QColor("yellow");
	case 8:
	    return QColor("white");
	case 9:
	    return QColor("darkBlue");
	case 10:
	    return QColor("darkCyan");
	case 11:
	    return QColor("darkGreen");
	case 12:
	    return QColor("darkMagenta");
	case 13:
	    return QColor("darkRed");
	case 14:
	    return QColor("darkYellow");
	case 15:
	    return QColor("darkGray");
	case 16:
	    return QColor("lightGray");

	default:
	    if(defaultcolor == -1)
		return QColor("black");
	    else
		return colorForNumber(defaultcolor, -1);
    }
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
