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

#include "KWTableStyle.h"
#include "KWDocument.h"
#include "KWFrame.h"

#include <kdebug.h>
#include <klocale.h>
#include <qdom.h>

KWTableStyleCollection::KWTableStyleCollection()
    : KoUserStyleCollection( QString::fromLatin1( "table" ) )
{
}

void KWTableStyleCollection::saveOasis( KoGenStyles& mainStyles, KoSavingContext& savingContext ) const
{

}

void KWTableStyleCollection::loadOasisStyles( KoOasisContext& context )
{

}

/******************************************************************/
/* Class: KWTableStyle                                            */
/******************************************************************/

KWTableStyle::KWTableStyle( const QString & name, KoParagStyle * _paragStyle, KWFrameStyle * _frameStyle )
    : KoUserStyle( name )
{
    m_paragStyle = _paragStyle;
    m_frameStyle = _frameStyle;
}

KWTableStyle::KWTableStyle( QDomElement & parentElem, KWDocument *_doc, int /*docVersion*/ )
    : KoUserStyle( QString::null )
{
    QDomElement element = parentElem.namedItem( "NAME" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("value") ) ) {
        m_name = element.attribute( "value" );
        m_displayName = i18n( "Style name", m_name.utf8() );
    } else
        kdWarning() << "No NAME tag in table style!" << endl;

    element = parentElem.namedItem( "PFRAMESTYLE" ).toElement();
    m_frameStyle = 0;
    if ( ( !element.isNull() ) && ( element.hasAttribute("name") ) )
        m_frameStyle = _doc->frameStyleCollection()->findStyle( element.attribute( "name" ) );

    if ( !m_frameStyle ) {
        if ( !_doc->frameStyleCollection()->isEmpty() )
            m_frameStyle = _doc->frameStyleCollection()->frameStyleAt( 0 );
        else { // Isn't possible ( I hope )
            KWFrameStyle * standardFrameStyle = new KWFrameStyle( "Plain" );
            standardFrameStyle->setBackgroundColor(QColor("white"));
            standardFrameStyle->setTopBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setRightBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setLeftBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setBottomBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            _doc->frameStyleCollection()->addStyle( standardFrameStyle );
            m_frameStyle = _doc->frameStyleCollection()->frameStyleAt( 0 );
        }
    }

    element = parentElem.namedItem( "PSTYLE" ).toElement();
    m_paragStyle = 0;
    if ( ( !element.isNull() ) && ( element.hasAttribute("name") ) )
        m_paragStyle = _doc->styleCollection()->findStyle( element.attribute( "name" ) );

    if ( !m_paragStyle ) {
        if ( _doc->styleCollection()->styleList().count()>0 )
            m_paragStyle = _doc->styleCollection()->styleAt( 0 );
        else { // Isn't possible ( I hope )
            KoParagStyle * standardStyle = new KoParagStyle( "Standard" );
            standardStyle->format().setFont( _doc->defaultFont() );
            _doc->styleCollection()->addStyle( standardStyle );
            m_paragStyle = _doc->styleCollection()->styleAt( 0 );
        }
    }
}

void KWTableStyle::operator=( const KWTableStyle &rhs )
{
    KoUserStyle::operator=( rhs );
    m_paragStyle = rhs.paragraphStyle();
    m_frameStyle = rhs.frameStyle();
}

void KWTableStyle::saveTableStyle( QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    element.setAttribute( "value", displayName() );

    if (m_frameStyle)
    {
        element = doc.createElement( "PFRAMESTYLE" );
        parentElem.appendChild( element );
        element.setAttribute( "name", m_frameStyle->name() );
    }
    if (m_paragStyle)
    {
        element = doc.createElement( "PSTYLE" );
        parentElem.appendChild( element );
        element.setAttribute( "name", m_paragStyle->name() );
    }

}

KWTableStyle *KWTableStyle::loadStyle( QDomElement & parentElem, KWDocument *_doc, int docVersion )
{
    return new KWTableStyle( parentElem, _doc, docVersion );
}
