#include "kivio_common.h"
#include "kivio_text_style.h"
#include <qdom.h>
#include <qpainter.h>
#include <KoGlobal.h>

KivioTextStyle::KivioTextStyle()
{
    m_text = "";
    m_color = QColor(0,0,0);
    m_hTextAlign = Qt::AlignHCenter;
    m_vTextAlign = Qt::AlignVCenter;
    m_isHtml = false;
    //m_font = QFont("times",12);
    m_font = KoGlobal::defaultFont();
}

KivioTextStyle::~KivioTextStyle()
{
}

void KivioTextStyle::copyInto( KivioTextStyle *pTarget )
{
    pTarget->m_text = m_text;
    pTarget->m_color = m_color;
    pTarget->m_hTextAlign = m_hTextAlign;
    pTarget->m_vTextAlign = m_vTextAlign;
    pTarget->m_isHtml = m_isHtml;
    pTarget->m_font = m_font;
}

QDomElement KivioTextStyle::saveXML( QDomDocument &doc )
{
    QDomElement textE = doc.createElement("KivioTextStyle");

    XmlWriteString( textE, "text", m_text );
    XmlWriteInt( textE, "isHtml", m_isHtml );
    XmlWriteInt( textE, "hTextAlign", m_hTextAlign );
    XmlWriteInt( textE, "vTextAlign", m_vTextAlign );

    // Text font & color
    QDomElement innerTextE = doc.createElement("Font");
    XmlWriteColor( innerTextE, "color",   m_color );
    XmlWriteString( innerTextE, "family", m_font.family() );
    XmlWriteInt( innerTextE, "size",      m_font.pointSize() );
    XmlWriteInt( innerTextE, "bold",      m_font.bold() );
    XmlWriteInt( innerTextE, "italic",    m_font.italic() );
    XmlWriteInt( innerTextE, "underline", m_font.underline() );
    XmlWriteInt( innerTextE, "strikeOut", m_font.strikeOut() );
    XmlWriteInt( innerTextE, "fixedPitch", m_font.fixedPitch() );

    textE.appendChild( innerTextE );

    return textE;
}

bool KivioTextStyle::loadXML( const QDomElement &e )
{
    m_text = XmlReadString( e, "text", "" );
    m_isHtml = (bool)XmlReadInt( e, "isHtml", (int)false );

    m_hTextAlign = XmlReadInt( e, "hTextAlign", Qt::AlignHCenter );
    m_vTextAlign = XmlReadInt( e, "vTextAlign", Qt::AlignVCenter );

    // Search for the font
    QDomNode innerNode = e.firstChild();
    while( !innerNode.isNull() )
    {
        QString innerName = innerNode.nodeName();
        QDomElement innerE = innerNode.toElement();

        if( innerName == "Font" )
        {
            m_font.setFamily( XmlReadString(innerE, "family", "times") );
            m_font.setPointSize( XmlReadInt(innerE, "size", 12 ) );
            m_font.setBold( (bool)XmlReadInt( innerE, "bold", 12 ) );
            m_font.setItalic( (bool)XmlReadInt( innerE, "italic", 12 ) );
            m_font.setUnderline( (bool)XmlReadInt( innerE, "underline", 12 ) );
            m_font.setStrikeOut( (bool)XmlReadInt( innerE, "strikeOut", 12 ) );
            m_font.setFixedPitch( (bool)XmlReadInt( innerE, "fixedPitch", false ) );
            m_color = XmlReadColor( innerE, "color", QColor(0,0,0) );
        }

        innerNode = innerNode.nextSibling();
    }

    return true;
}
