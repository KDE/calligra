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

#include "KWFrameStyle.h"
#include "KWDocument.h"
#include "KWFrame.h"

#include <koGenStyles.h>
#include <koOasisStyles.h>
#include <kooasiscontext.h>

#include <kdebug.h>
#include <klocale.h>
#include <qdom.h>
#include <koxmlns.h>

KWGenericStyle::KWGenericStyle( const QString & name )
    : m_name( name ), m_displayName( i18n( "Style name", name.utf8() ) )
{
}

QString KWGenericStyle::displayName() const
{
    return m_displayName;
}

void KWGenericStyle::setDisplayName( const QString& name )
{
    m_displayName = name;
}

////

KWGenericStyleCollection::KWGenericStyleCollection()
{
    m_styleList.setAutoDelete( false );
    m_deletedStyles.setAutoDelete( true );
    m_lastStyle = 0;
}

KWGenericStyle* KWGenericStyleCollection::findStyle( const QString & _name ) const
{
    // Caching, to speed things up
    if ( m_lastStyle && m_lastStyle->name() == _name )
        return m_lastStyle;

    QPtrListIterator<KWGenericStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        if ( styleIt.current()->name() == _name ) {
            m_lastStyle = styleIt.current();
            return m_lastStyle;
        }
    }

    if( _name == "Plain" && !m_styleList.isEmpty() )
        return m_styleList.getFirst(); // fallback..

    return 0;
}

QString KWGenericStyleCollection::generateUniqueName() const
{
    int count = 1;
    QString name;
    do {
        name = "new" + QString::number( count++ );
    } while ( findStyle( name ) );
    return name;
}

KWGenericStyleCollection::~KWGenericStyleCollection()
{
    clear();
}

void KWGenericStyleCollection::clear()
{
    m_styleList.setAutoDelete( true );
    m_styleList.clear();
    m_styleList.setAutoDelete( false );
    m_deletedStyles.clear();
    m_lastStyle = 0;
}

QStringList KWGenericStyleCollection::displayNameList() const
{
    QStringList lst;
    QPtrListIterator<KWGenericStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
        lst.append( styleIt.current()->displayName() );
    return lst;
}

/******************************************************************/
/* Class: KWFrameStyleCollection                                  */
/******************************************************************/

//necessary to create unique shortcut
int KWFrameStyleCollection::styleNumber = 0;

KWFrameStyleCollection::KWFrameStyleCollection()
{
}

KWFrameStyle* KWFrameStyleCollection::findStyleByShortcut( const QString & _shortCut )
{
    // Caching, to speed things up
    if ( m_lastStyle && m_lastStyle->shortCutName() == _shortCut )
        return m_lastStyle;

    QPtrListIterator<KWGenericStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        KWFrameStyle* style = static_cast<KWFrameStyle *>( styleIt.current() );
        if ( style->shortCutName() == _shortCut ) {
            m_lastStyle = style;
            return m_lastStyle;
        }
    }
    return 0L;
}


KWFrameStyle* KWFrameStyleCollection::findTranslatedFrameStyle( const QString & _name )
{
    // Caching, to speed things up
    if ( m_lastStyle && m_lastStyle->displayName() == _name )
        return m_lastStyle;

    QPtrListIterator<KWGenericStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        KWFrameStyle* style = static_cast<KWFrameStyle *>( styleIt.current() );
        if ( style->displayName() == _name ) {
            m_lastStyle = style;
            return m_lastStyle;
        }
    }

    if( ( _name == "Plain" ) || ( _name == i18n( "Style name", "Plain" ) ) )
        return static_cast<KWFrameStyle *>(m_styleList.getFirst()); // fallback..

    return 0L;
}


KWFrameStyle* KWFrameStyleCollection::addFrameStyleTemplate( KWFrameStyle * sty )
{
    // First check for duplicates.
    for ( KWGenericStyle* p = m_styleList.first(); p != 0L; p = m_styleList.next() )
    {
        if ( p->name() == sty->name() ) {
            if ( p->displayName() == sty->displayName() ) {
                // Replace existing style
                if ( sty != p )
                {
                    *p = *sty;
                    delete sty;
                }
                return static_cast<KWFrameStyle *>( p );
            } else { // internal name conflict, but it's not the same style as far as the user is concerned
                sty->setName( generateUniqueName() );
            }
        }
    }
    m_styleList.append( sty );
    sty->setShortCutName( QString("shortcut_framestyle_%1").arg(styleNumber).latin1());
    styleNumber++;
    return sty;
}

void KWFrameStyleCollection::removeFrameStyleTemplate ( KWFrameStyle *style ) {
    if( m_styleList.removeRef(style)) {
        if ( m_lastStyle == style )
            m_lastStyle = 0L;
        // Remember to delete this style when deleting the document
        m_deletedStyles.append(style);
    }
}

void KWFrameStyleCollection::updateFrameStyleListOrder( const QStringList &list )
{
    QPtrList<KWGenericStyle> orderStyle;
    QStringList lst( list );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
    {
        //kdDebug()<<" style :"<<(*it)<<endl;
        QPtrListIterator<KWGenericStyle> style( m_styleList );
        for ( ; style.current() ; ++style )
        {
            if ( style.current()->name() == *it)
            {
                orderStyle.append( style.current() );
                //kdDebug()<<" found !!!!!!!!!!!!\n";
                break;
            }
        }
    }
    m_styleList.setAutoDelete( false );
    m_styleList.clear();
    m_styleList = orderStyle;
}

void KWFrameStyleCollection::saveOasis( KoGenStyles& mainStyles, KoSavingContext& savingContext ) const
{
    QPtrListIterator<KWGenericStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        KWFrameStyle* style = static_cast<KWFrameStyle *>( styleIt.current() );
        style->saveOasis( mainStyles, savingContext );
    }
}

void KWFrameStyleCollection::loadOasisStyles( KoOasisContext& context )
{
    QValueVector<QDomElement> userStyles = context.oasisStyles().userStyles();
    uint nStyles = userStyles.count();
    if( nStyles ) { // we are going to import at least one style.
        KWFrameStyle *s = findStyle("Standard");
        //kdDebug() << "loadOasisStyleTemplates looking for Standard, to delete it. Found " << s << endl;
        if(s) // delete the standard style.
            removeFrameStyleTemplate(s);
    }
    for (unsigned int item = 0; item < nStyles; item++) {
        QDomElement styleElem = userStyles[item];
	Q_ASSERT( !styleElem.isNull() );

        if ( styleElem.attributeNS( KoXmlNS::style, "family", QString::null ) != "graphic" )
            continue;

        KWFrameStyle *sty = new KWFrameStyle( QString::null );
        // Load the style
        sty->loadOasis( styleElem, context );
        // Style created, now let's try to add it
        sty = addFrameStyleTemplate( sty );

        kdDebug() << " Loaded frame style " << sty->name() << " - now " << count() << " styles" << endl;
    }
}

const QPtrList<KWFrameStyle> KWFrameStyleCollection::frameStyleList() const
{
    // TODO get rid of this method, I think.
    QPtrList<KWFrameStyle> lst;
    QPtrListIterator<KWGenericStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
        lst.append( static_cast<KWFrameStyle *>( styleIt.current() ) );
    return lst;
}

/******************************************************************/
/* Class: KWFrameStyle                                            */
/******************************************************************/

KWFrameStyle::KWFrameStyle( const QString & name )
    : KWGenericStyle( name )
{
    m_shortCut_name = QString::null;
    m_backgroundColor.setColor( Qt::white );
}

KWFrameStyle::KWFrameStyle( const QString & name, KWFrame * frame )
    : KWGenericStyle( name )
{
    m_backgroundColor = frame->backgroundColor();
    m_borderLeft = frame->leftBorder();
    m_borderRight = frame->rightBorder();
    m_borderTop = frame->topBorder();
    m_borderBottom = frame->bottomBorder();
}

KWFrameStyle::KWFrameStyle( QDomElement & parentElem, int /*docVersion=2*/ )
    : KWGenericStyle( QString::null )
{
    QDomElement element = parentElem.namedItem( "NAME" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("value") ) )
        m_name = element.attribute( "value" );

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

void KWFrameStyle::operator=( const KWFrameStyle &rhs )
{
    m_name = rhs.m_name;
    m_displayName = rhs.m_displayName;
    m_backgroundColor = rhs.m_backgroundColor;
    m_shortCut_name = rhs.m_shortCut_name;
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
    element.setAttribute( "value", name() );

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
    if ( nameIsConform )
        (void)mainStyles.lookup( frameStyle, m_name, false );
    else
        (void)mainStyles.lookup( frameStyle, "fr", true /*force numbering*/ );
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
    context.addStyles( &styleElem ); // Load all parents - only because we don't support inheritance.

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
