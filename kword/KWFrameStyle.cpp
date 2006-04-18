/* This file is part of the KDE project
   Copyright (C) 2002 Nash Hoogwater <nrhoogwater@wanadoo.nl>
                 2005 David Faure <faure@kde.org>

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

#include "KWFrameStyle.h"
#include "KWDocument.h"
#include "KWFrame.h"

#include <KoGenStyles.h>
#include <KoOasisStyles.h>
#include <KoOasisContext.h>
#include <KoXmlNS.h>

#include <kdebug.h>
#include <klocale.h>
#include <qdom.h>
//Added by qt3to4:
#include <Q3ValueList>

KWFrameStyleCollection::KWFrameStyleCollection()
    : KoUserStyleCollection( QString::fromLatin1( "frame" ) )
{
}

void KWFrameStyleCollection::saveOasis( KoGenStyles& mainStyles, KoSavingContext& savingContext ) const
{
    if ( !isDefault() ) {
        for ( Q3ValueList<KoUserStyle *>::const_iterator styleIt = m_styleList.begin(), styleEnd = m_styleList.end() ; styleIt != styleEnd ; ++styleIt )
        {
            KWFrameStyle* style = static_cast<KWFrameStyle *>( *styleIt );
            style->saveOasis( mainStyles, savingContext );
        }
    }
}

int KWFrameStyleCollection::loadOasisStyles( KoOasisContext& context )
{
    Q3ValueVector<QDomElement> userStyles = context.oasisStyles().userStyles();
    bool defaultStyleDeleted = false;
    int stylesLoaded = 0;
    for (unsigned int item = 0; item < userStyles.count(); item++) {
        QDomElement styleElem = userStyles[item];
	Q_ASSERT( !styleElem.isNull() );

        if ( styleElem.attributeNS( KoXmlNS::style, "family", QString::null ) != "graphic" )
            continue;

        if ( !defaultStyleDeleted ) {
            KWFrameStyle *s = findStyle( defaultStyleName() );
            //kDebug() << "KWFrameStyleCollection::loadOasisStyles looking for " << defaultStyleName() << ", to delete it. Found " << s << endl;
            if(s) // delete the standard style.
                removeStyle(s);
            defaultStyleDeleted = true;
        }

        KWFrameStyle *sty = new KWFrameStyle( QString::null );
        // Load the style
        sty->loadOasis( styleElem, context );
        // Style created, now let's try to add it
        sty = static_cast<KWFrameStyle *>( addStyle( sty ) );

        kDebug() << " Loaded frame style " << sty->name() << " - now " << count() << " styles" << endl;
        ++stylesLoaded;
    }
    return stylesLoaded;
}

/******************************************************************/
/* Class: KWFrameStyle                                            */
/******************************************************************/

KWFrameStyle::KWFrameStyle( const QString & name )
    : KoUserStyle( name )
{
    m_backgroundColor.setColor( Qt::white );
}

KWFrameStyle::KWFrameStyle( const QString & name, KWFrame * frame )
    : KoUserStyle( name )
{
    m_backgroundColor = frame->backgroundColor();
    m_borderLeft = frame->leftBorder();
    m_borderRight = frame->rightBorder();
    m_borderTop = frame->topBorder();
    m_borderBottom = frame->bottomBorder();
}

KWFrameStyle::KWFrameStyle( QDomElement & parentElem, int /*docVersion=2*/ )
    : KoUserStyle( QString::null )
{
    QDomElement element = parentElem.namedItem( "NAME" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("value") ) ) {
        m_name = element.attribute( "value" );
        m_displayName = i18n( "Style name", m_name.toUtf8() );
    } else
        kWarning() << "No NAME tag in frame style!" << endl;

    element = parentElem.namedItem( "LEFTBORDER" ).toElement();
    if ( !element.isNull() )
        m_borderLeft = KoBorder::loadBorder( element );
    else
        m_borderLeft.setPenWidth( 0 );

    element = parentElem.namedItem( "RIGHTBORDER" ).toElement();
    if ( !element.isNull() )
        m_borderRight = KoBorder::loadBorder( element );
    else
        m_borderRight.setPenWidth( 0 );

    element = parentElem.namedItem( "TOPBORDER" ).toElement();
    if ( !element.isNull() )
        m_borderTop = KoBorder::loadBorder( element );
    else
        m_borderTop.setPenWidth( 0 );

    element = parentElem.namedItem( "BOTTOMBORDER" ).toElement();
    if ( !element.isNull() )
        m_borderBottom = KoBorder::loadBorder( element );
    else
        m_borderBottom.setPenWidth( 0 );

    QColor c("white");
    if ( parentElem.hasAttribute("red") )
        c.setRgb(
            KWDocument::getAttribute( parentElem, "red", 0 ),
            KWDocument::getAttribute( parentElem, "green", 0 ),
            KWDocument::getAttribute( parentElem, "blue", 0 ) );

    m_backgroundColor = QBrush( c );
}

KWFrameStyle::KWFrameStyle( const KWFrameStyle &rhs )
    : KoUserStyle( QString::null )
{
    operator=( rhs );
}

void KWFrameStyle::operator=( const KWFrameStyle &rhs )
{
    KoUserStyle::operator=( rhs );
    m_backgroundColor = rhs.m_backgroundColor;
    m_borderLeft = rhs.m_borderLeft;
    m_borderRight = rhs.m_borderRight;
    m_borderTop = rhs.m_borderTop;
    m_borderBottom = rhs.m_borderBottom;
}

int KWFrameStyle::compare( const KWFrameStyle & frameStyle ) const
{
    int flags = 0;
    if ( m_borderLeft != frameStyle.m_borderLeft
         || m_borderRight != frameStyle.m_borderRight
         || m_borderTop != frameStyle.m_borderTop
         || m_borderBottom != frameStyle.m_borderBottom )
        flags |= Borders;
    if ( m_backgroundColor.color() != frameStyle.m_backgroundColor.color() )
        flags |= Background;

    return flags;
}


void KWFrameStyle::saveFrameStyle( QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    element.setAttribute( "value", displayName() );

    if ( m_borderLeft.width() > 0 )
    {
        element = doc.createElement( "LEFTBORDER" );
        parentElem.appendChild( element );
        m_borderLeft.save( element );
    }
    if ( m_borderRight.width() > 0 )
    {
        element = doc.createElement( "RIGHTBORDER" );
        parentElem.appendChild( element );
        m_borderRight.save( element );
    }
    if ( m_borderTop.width() > 0 )
    {
        element = doc.createElement( "TOPBORDER" );
        parentElem.appendChild( element );
        m_borderTop.save( element );
    }
    if ( m_borderBottom.width() > 0 )
    {
        element = doc.createElement( "BOTTOMBORDER" );
        parentElem.appendChild( element );
        m_borderBottom.save( element );
    }

    if(m_backgroundColor.color().isValid())
    {
        parentElem.setAttribute( "red", m_backgroundColor.color().red() );
        parentElem.setAttribute( "green", m_backgroundColor.color().green() );
        parentElem.setAttribute( "blue", m_backgroundColor.color().blue() );
    }
}

KWFrameStyle *KWFrameStyle::loadStyle( QDomElement & parentElem, int docVersion )
{
    return new KWFrameStyle( parentElem, docVersion );
}

void KWFrameStyle::saveOasis( KoGenStyles& mainStyles, KoSavingContext& savingContext ) const
{
    Q_UNUSED( savingContext );

    KoGenStyle frameStyle( KWDocument::STYLE_FRAME_USER, "graphic" );
    frameStyle.addAttribute( "style:display-name", displayName() );

    // Borders (taken from KWFrame::saveBorderProperties)
    if (  ( m_borderLeft == m_borderRight )
          && ( m_borderLeft == m_borderTop )
          && ( m_borderLeft == m_borderBottom ) )
    {
        frameStyle.addProperty( "fo:border", m_borderLeft.saveFoBorder() );
    }
    else
    {
        frameStyle.addProperty( "fo:border-left", m_borderLeft.saveFoBorder() );
        frameStyle.addProperty( "fo:border-right", m_borderRight.saveFoBorder() );
        frameStyle.addProperty( "fo:border-top", m_borderTop.saveFoBorder() );
        frameStyle.addProperty( "fo:border-bottom", m_borderBottom.saveFoBorder() );
    }

    // Background (taken from KWFrame::saveBorderProperties)
    if ( m_backgroundColor.style() == Qt::NoBrush )
        frameStyle.addProperty( "fo:background-color", "transparent" );
    else if ( m_backgroundColor.color().isValid() )
        frameStyle.addProperty( "fo:background-color", m_backgroundColor.color().name() );

    // try to preserve existing internal name, if it looks adequate (no spaces)
    // ## TODO: check XML-Schemacs NCName conformity
    const bool nameIsConform = !m_name.isEmpty() && m_name.find( ' ' ) == -1;
    QString newName = m_name;
    if ( nameIsConform )
        newName = mainStyles.lookup( frameStyle, m_name, KoGenStyles::DontForceNumbering );
    else
        newName = mainStyles.lookup( frameStyle, "fr" );
    const_cast<KWFrameStyle*>( this )->m_name = newName;
}

void KWFrameStyle::loadOasis( QDomElement & styleElem, KoOasisContext& context )
{
    // Load name
    m_name = styleElem.attributeNS( KoXmlNS::style, "name", QString::null );
    m_displayName = styleElem.attributeNS( KoXmlNS::style, "display-name", QString::null );
    if ( m_displayName.isEmpty() )
        m_displayName = m_name;

    KoStyleStack& styleStack = context.styleStack();
    styleStack.setTypeProperties( "graphic" );

    styleStack.save();
    context.addStyles( &styleElem, "graphic" ); // Load all parents - only because we don't support inheritance.

    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "background-color" ) ) {
        QString color = styleStack.attributeNS( KoXmlNS::fo, "background-color" );
        if ( color == "transparent" )
            m_backgroundColor = QBrush( QColor(), Qt::NoBrush );
        else
            m_backgroundColor = QBrush( QColor( color ) /*, brush style is a dead feature, ignored */ );
    }

    m_borderLeft.loadFoBorder( styleStack.attributeNS( KoXmlNS::fo, "border", "left") );
    m_borderRight.loadFoBorder( styleStack.attributeNS( KoXmlNS::fo, "border", "right") );
    m_borderTop.loadFoBorder( styleStack.attributeNS( KoXmlNS::fo, "border", "top") );
    m_borderBottom.loadFoBorder( styleStack.attributeNS( KoXmlNS::fo, "border", "bottom") );

    styleStack.restore();
}
