#include <document.h>

#include <qdom.h>
#include <kdebug.h>
#include <ustring.h>
#include <word97_generated.h>

Document::Document( const std::string& fileName, QDomDocument& mainDocument, QDomElement& mainFramesetElement )
    : wvWare::LLDocument( fileName ), m_mainDocument( mainDocument ),
      m_mainFramesetElement( mainFramesetElement )
{
}

Document::~Document()
{
}

void Document::paragraphStart( wvWare::SharedPtr<const wvWare::Word97::PAP> pap )
{
    m_paragraph = QString::null;
    m_pap = pap;
}

void Document::paragraphEnd()
{
    writeOutParagraph( "Standard", m_paragraph );
}

void Document::runOfText( const wvWare::UString& text )
{
    m_paragraph += QString( reinterpret_cast<const QChar*>( text.data() ), text.length() );
}

void Document::writeOutParagraph( const QString& name, const QString& text )
{
    QDomElement paragraphElementOut=m_mainDocument.createElement("PARAGRAPH");
    m_mainFramesetElement.appendChild(paragraphElementOut);
    QDomElement textElement=m_mainDocument.createElement("TEXT");
    paragraphElementOut.appendChild(textElement);
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
}  // WriteOutParagraph
