/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "document.h"
#include "conversion.h"
#include "texthandler.h"

#include <koGlobal.h>
#include <kdebug.h>

#include <styles.h>
#include <ustring.h>
#include <word97_generated.h>
#include <parser.h>
#include <parserfactory.h>


Document::Document( const std::string& fileName, QDomDocument& mainDocument, QDomElement& mainFramesetElement )
    : m_mainDocument( mainDocument ), m_mainFramesetElement( mainFramesetElement ),
      m_replacementHandler( new KWordReplacementHandler ),
      m_parser( wvWare::ParserFactory::createParser( fileName ) )
{
    if ( m_parser ) // 0 in case of major error (e.g. unsupported format)
    {
        m_textHandler = new KWordTextHandler( m_parser );
        m_parser->setSubDocumentHandler( this );
        m_parser->setTextHandler( m_textHandler );
        m_parser->setInlineReplacementHandler( m_replacementHandler );
        prepareDocument();
        processStyles();
    }
}

Document::~Document()
{
    delete m_replacementHandler;
}

void Document::prepareDocument()
{
    const wvWare::Word97::DOP& dop = m_parser->dop();

    QDomElement elementDoc = m_mainDocument.documentElement();

    QDomElement element;
    element = m_mainDocument.createElement("ATTRIBUTES");
    element.setAttribute("processing",0); // WP
    element.setAttribute("hasHeader",0); // TODO
    element.setAttribute("hasFooter",0); // TODO
    element.setAttribute("unit","mm"); // How to figure out the unit to use?

    element.setAttribute("tabStopValue", (double)dop.dxaTab / 20.0 );
    elementDoc.appendChild(element);

    // FOOTNOTESETTING: use nfcFtnRef for the type of counter
    // Hmm there's nfcFtnRef2 too.
}

void Document::processStyles()
{
    QDomElement stylesElem = m_mainDocument.createElement( "STYLES" );
    m_mainDocument.documentElement().appendChild( stylesElem );

    m_textHandler->setFrameSetElement( stylesElem ); /// ### naming!
    const wvWare::StyleSheet& styles = m_parser->styleSheet();
    unsigned int count = styles.size();
    //kdDebug() << k_funcinfo << "styles count=" << count << endl;
    for ( unsigned int i = 0; i < count ; ++i )
    {
        const wvWare::Style* style = styles.styleByIndex( i );
        Q_ASSERT( style );
        //kdDebug() << k_funcinfo << "style " << i << " " << style << endl;
        if ( style && style->type() == wvWare::Style::sgcPara )
        {
            QDomElement styleElem = m_mainDocument.createElement("STYLE");
            stylesElem.appendChild( styleElem );

            QConstString name = Conversion::string( style->name() );
            QDomElement element = m_mainDocument.createElement("NAME");
            element.setAttribute( "value", name.string() );
            styleElem.appendChild( element );

            //kdDebug() << k_funcinfo << "Style " << i << ": " << name.string() << endl;

            const wvWare::Style* followingStyle = styles.styleByID( style->followingStyle() );
            if ( followingStyle && followingStyle != style )
            {
                QConstString followingName = Conversion::string( followingStyle->name() );
                element = m_mainDocument.createElement("FOLLOWING");
                element.setAttribute( "name", followingName.string() );
                styleElem.appendChild( element );
            }

            m_textHandler->paragLayoutBegin(); // new style, reset some vars

            // It's important to do that one first, for m_shadowTextFound
            m_textHandler->writeFormat( styleElem, &style->chp(), 0L /*all of it, no ref chp*/, 0, 0 );

            m_textHandler->writeLayout( styleElem, style->paragraphProperties(), style );
        }
        // KWord doesn't support character styles yet
    }
}

bool Document::parse()
{
    if ( m_parser )
        return m_parser->parse();
    return false;
}

void Document::startBody()
{
    kdDebug() << k_funcinfo << endl;
    m_textHandler->setFrameSetElement( m_mainFramesetElement );
    connect( m_textHandler, SIGNAL( firstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> ) ),
             this, SLOT( slotFirstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> ) ) );
}

void Document::endBody()
{
    kdDebug() << k_funcinfo << endl;
    disconnect( m_textHandler, SIGNAL( firstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> ) ),
             this, SLOT( slotFirstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> ) ) );
}

void Document::slotFirstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> sep )
{
    kdDebug() << k_funcinfo << endl;
    QDomElement elementDoc = m_mainDocument.documentElement();

    QDomElement elementPaper = m_mainDocument.createElement("PAPER");
    bool landscape = (sep->dmOrientPage == 2);
    double width = (double)sep->xaPage / 20.0;
    double height = (double)sep->yaPage / 20.0;
    elementPaper.setAttribute("width", width);
    elementPaper.setAttribute("height", height);

    // guessFormat takes millimeters
    width = POINT_TO_MM( width );
    height = POINT_TO_MM( height );
    KoFormat paperFormat = KoPageFormat::guessFormat( landscape ? height : width, landscape ? width : height );
    elementPaper.setAttribute("format",paperFormat);

    elementPaper.setAttribute("orientation", landscape ? PG_LANDSCAPE : PG_PORTRAIT );
    elementPaper.setAttribute("columns", sep->ccolM1 + 1 );
    elementPaper.setAttribute("columnspacing", (double)sep->dxaColumns / 20.0);
    elementPaper.setAttribute("hType",0); // TODO
    elementPaper.setAttribute("fType",0); // TODO
    elementPaper.setAttribute("spHeadBody", (double)sep->dyaHdrTop / 20.0);
    elementPaper.setAttribute("spFootBody", (double)sep->dyaHdrBottom / 20.0);
    // elementPaper.setAttribute("zoom",100); // not a doc property in kword
    elementDoc.appendChild(elementPaper);

    QDomElement element = m_mainDocument.createElement("PAPERBORDERS");
    element.setAttribute("left", (double)sep->dxaLeft / 20.0);
    element.setAttribute("top",(double)sep->dyaTop / 20.0);
    element.setAttribute("right", (double)sep->dxaRight / 20.0);
    element.setAttribute("bottom", (double)sep->dyaBottom / 20.0);
    elementPaper.appendChild(element);

    // TODO apply brcTop/brcLeft etc. to the main FRAME
    // TODO use sep->fEndNote to set the 'use endnotes or footnotes' flag
}

#include "document.moc"
