/* This file is part of the KDE project
   Copyright (C) 2002 Nash Hoogwater <nrhoogwater@wanadoo.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; using
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KWTableTemplate.h"

#include "KWDocument.h"
#include "KWFrameStyle.h"
#include "KoParagStyle.h"

#include <kdebug.h>
#include <klocale.h>
#include <qdom.h>


/******************************************************************/
/* Class: KWTableTemplateCollection                                  */
/******************************************************************/

KWTableTemplateCollection::KWTableTemplateCollection()
{
    m_templateList.setAutoDelete( false );
    m_deletedTemplates.setAutoDelete( true );
    m_lastTemplate = 0L;
}

KWTableTemplateCollection::~KWTableTemplateCollection()
{
    //kdDebug() << "KWTableTemplateCollection::destructor" << endl;
    clear();
}

void KWTableTemplateCollection::clear()
{
    m_templateList.setAutoDelete( true );
    m_templateList.clear();
    m_deletedTemplates.clear();
    m_lastTemplate = 0;
}

KWTableTemplate* KWTableTemplateCollection::findTableTemplate( const QString & _name )
{
    // Caching, to speed things up
    if ( m_lastTemplate && m_lastTemplate->name() == _name )
        return m_lastTemplate;

    QPtrListIterator<KWTableTemplate> templateIt( m_templateList );
    for ( ; templateIt.current(); ++templateIt )
    {
        if ( templateIt.current()->name() == _name ) {
            m_lastTemplate = templateIt.current();
            return m_lastTemplate;
        }
    }

//    if(_name == "Plain") return m_styleList.at(0); // fallback..

    return 0L;
}


KWTableTemplate* KWTableTemplateCollection::addTableTemplate( KWTableTemplate * tt )
{
    // First check for duplicates.
    for ( KWTableTemplate* p = m_templateList.first(); p != 0L; p = m_templateList.next() )
    {
        if ( p->name() == tt->name() ) {
            // Replace existing template
            if ( tt != p )
            {
                *p = *tt;
                delete tt;
            }
            return p;
        }
    }
    m_templateList.append( tt );
    return tt;
}

void KWTableTemplateCollection::removeTableTemplate ( KWTableTemplate *tt )
{
    if( m_templateList.removeRef( tt )) {
        if ( m_lastTemplate == tt )
            m_lastTemplate = 0L;
        // Remember to delete this template when deleting the document
        m_deletedTemplates.append( tt );
    }
}

/******************************************************************/
/* Class: KWTableTemplate                                            */
/******************************************************************/

KWTableTemplate::KWTableTemplate( const QString & name, KWTableStyle * _firstRow, KWTableStyle * _firstCol,
                     KWTableStyle *_lastRow, KWTableStyle *_lastCol, KWTableStyle *_bodyCell,
                     KWTableStyle *_topLeftCorner, KWTableStyle *_topRightCorner,
                     KWTableStyle *_bottomLeftCorner, KWTableStyle *_bottomRightCorner )
{
    m_name = name;
    m_firstRow = _firstRow;
    m_firstCol = _firstCol;
    m_lastRow = _lastRow;
    m_lastCol = _lastCol;
    m_bodyCell = _bodyCell;
    m_topLeftCorner = _topLeftCorner;
    m_topRightCorner = _topRightCorner;
    m_bottomRightCorner = _bottomRightCorner;
    m_bottomLeftCorner = _bottomLeftCorner;
}

KWTableTemplate::KWTableTemplate( QDomElement & parentElem, KWDocument *_doc, int /*docVersion*/ )
{
    m_topRightCorner = 0L;
    m_topLeftCorner = 0L;
    m_bottomRightCorner = 0L;
    m_bottomLeftCorner = 0L;

    QDomElement element = parentElem.namedItem( "NAME" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("value") ) )
        m_name = element.attribute( "value" );

    element = parentElem.namedItem( "BODYCELL" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("name") )
         && ( _doc->tableStyleCollection()->findStyle( element.attribute( "name" ) ) ) )
        m_bodyCell = _doc->tableStyleCollection()->findStyle( element.attribute( "name" ) );
    else {
        KWTableStyle *ts = _doc->tableStyleCollection()->findStyle( "Plain" );
        if (ts) {
            setBodyCell( ts );
        }
        else {
            ts = new KWTableStyle( "Plain", 0L, 0L );
            KWFrameStyle *fs = _doc->frameStyleCollection()->findStyle( "Plain" );
            KoParagStyle *s = _doc->styleCollection()->findStyle( "Standard" );
            if ( fs )
                ts->setFrameStyle( fs );
            else {
                KWFrameStyle * standardFrameStyle = new KWFrameStyle( "Plain" );
                standardFrameStyle->setBackgroundColor(QColor("white"));
                standardFrameStyle->setTopBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
                standardFrameStyle->setRightBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
                standardFrameStyle->setLeftBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
                standardFrameStyle->setBottomBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
                _doc->frameStyleCollection()->addStyle( standardFrameStyle );
                ts->setFrameStyle( fs );
            }

            if ( s )
                ts->setParagraphStyle( s );
            else {
                KoParagStyle * standardStyle = new KoParagStyle( "Standard" );
                standardStyle->format().setFont( _doc->defaultFont() );
                _doc->styleCollection()->addStyle( standardStyle );
                ts->setParagraphStyle( s );
            }
            setBodyCell ( ts );
        }
    }
    element = parentElem.namedItem( "FIRSTROW" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("name") )
         && ( _doc->tableStyleCollection()->findStyle( element.attribute( "name" ) ) ) )
    {
        m_firstRow = _doc->tableStyleCollection()->findStyle( element.attribute( "name" ) );

        if ( element.hasAttribute("topleftcorner") )
            m_topLeftCorner = m_firstRow;
        if ( element.hasAttribute("toprightcorner") )
            m_topRightCorner = m_firstRow;
    }
    else
        m_firstRow = m_bodyCell;

    element = parentElem.namedItem( "FIRSTCOL" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("name") )
         && ( _doc->tableStyleCollection()->findStyle( element.attribute( "name" ) ) ) )
    {
        m_firstCol = _doc->tableStyleCollection()->findStyle( element.attribute( "name" ) );

        if ( element.hasAttribute("topleftcorner") )
            m_topLeftCorner = m_firstCol;
        if ( element.hasAttribute("bottomleftcorner") )
            m_bottomLeftCorner = m_firstCol;
    }
    else
        m_firstCol = m_bodyCell;

    element = parentElem.namedItem( "LASTROW" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("name") )
         && ( _doc->tableStyleCollection()->findStyle( element.attribute( "name" ) ) ) )
    {
        m_lastRow = _doc->tableStyleCollection()->findStyle( element.attribute( "name" ) );

        if ( ( !element.isNull() ) && ( element.hasAttribute("bottomrightcorner") ) )
            m_bottomRightCorner = m_lastRow;
        if ( ( !element.isNull() ) && ( element.hasAttribute("bottomleftcorner") ) )
            m_bottomLeftCorner = m_lastRow;
    }
    else
        m_lastRow = m_bodyCell;

    element = parentElem.namedItem( "LASTCOL" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("name") )
         && ( _doc->tableStyleCollection()->findStyle( element.attribute( "name" ) ) ) )
    {
        m_lastCol = _doc->tableStyleCollection()->findStyle( element.attribute( "name" ) );

        if ( element.hasAttribute("toprightcorner") )
            m_topRightCorner = m_lastCol;
        if ( element.hasAttribute("bottomrightcorner") )
            m_bottomRightCorner = m_lastCol;
    }
    else
        m_lastCol = m_bodyCell;

    if (!m_topRightCorner) m_topRightCorner = m_bodyCell;
    if (!m_topLeftCorner) m_topLeftCorner = m_bodyCell;
    if (!m_bottomRightCorner) m_bottomRightCorner = m_bodyCell;
    if (!m_bottomLeftCorner) m_bottomLeftCorner = m_bodyCell;
}

void KWTableTemplate::operator=( const KWTableTemplate &rhs )
{
    m_name = rhs.m_name;
    m_firstRow = rhs.pFirstRow();
    m_firstCol = rhs.pFirstCol();
    m_lastRow = rhs.pLastRow();
    m_lastCol = rhs.pLastCol();
    m_bodyCell = rhs.pBodyCell();
    m_topLeftCorner = rhs.pTopLeftCorner();
    m_topRightCorner = rhs.pTopRightCorner();
    m_bottomRightCorner = rhs.pBottomRightCorner();
    m_bottomLeftCorner = rhs.pBottomLeftCorner();
}

QString KWTableTemplate::displayName() const
{
    return i18n( "Style name", name().utf8() );
}

// TODO
void KWTableTemplate::saveTableTemplate( QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    element.setAttribute( "value", name() );

    if (m_bodyCell)
    {
        element = doc.createElement( "BODYCELL" );
        parentElem.appendChild( element );
        element.setAttribute( "name", m_bodyCell->name() );
    }
    if (m_firstRow)
    {
        element = doc.createElement( "FIRSTROW" );
        parentElem.appendChild( element );
        element.setAttribute( "name", m_firstRow->name() );
    }
    if (m_firstCol)
    {
        element = doc.createElement( "FIRSTCOL" );
        parentElem.appendChild( element );
        element.setAttribute( "name", m_firstCol->name() );
    }
    if (m_lastRow)
    {
        element = doc.createElement( "LASTROW" );
        parentElem.appendChild( element );
        element.setAttribute( "name", m_lastRow->name() );
    }
    if (m_lastCol)
    {
        element = doc.createElement( "LASTCOL" );
        parentElem.appendChild( element );
        element.setAttribute( "name", m_lastCol->name() );
    }
}

KWTableTemplate *KWTableTemplate::loadTemplate( QDomElement & parentElem, KWDocument *_doc, int docVersion )
{
    return new KWTableTemplate(parentElem, _doc, docVersion);
}
