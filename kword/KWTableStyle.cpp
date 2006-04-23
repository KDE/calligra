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

#include <KoGenStyles.h>
#include <KoOasisStyles.h>
#include <KoOasisContext.h>
#include <KoXmlNS.h>

#include <kdebug.h>
#include <klocale.h>
#include <qdom.h>
//Added by qt3to4:
#include <QList>

KWTableStyleCollection::KWTableStyleCollection()
    : KoUserStyleCollection( QString::fromLatin1( "table" ) )
{
}

void KWTableStyleCollection::saveOasis( KoGenStyles& mainStyles, KoSavingContext& savingContext ) const
{
    if ( !isDefault() ) {
        for ( QList<KoUserStyle *>::const_iterator styleIt = m_styleList.begin(), styleEnd = m_styleList.end() ; styleIt != styleEnd ; ++styleIt )
        {
            KWTableStyle* style = static_cast<KWTableStyle *>( *styleIt );
            style->saveOasis( mainStyles, savingContext );
        }
    }
}

int KWTableStyleCollection::loadOasisStyles( KoOasisContext& context, const KoStyleCollection& paragraphStyles, const KWFrameStyleCollection& frameStyles )
{
    Q3ValueVector<QDomElement> userStyles = context.oasisStyles().userStyles();
    bool defaultStyleDeleted = false;
    int stylesLoaded = 0;
    for (unsigned int item = 0; item < userStyles.count(); item++) {
        QDomElement styleElem = userStyles[item];
	Q_ASSERT( !styleElem.isNull() );

        if ( styleElem.attributeNS( KoXmlNS::style, "family", QString::null ) != "table-cell" )
            continue;

        if ( !defaultStyleDeleted ) {
            KWTableStyle *s = findStyle( defaultStyleName() );
            //kDebug() << "KWTableStyleCollection::loadOasisStyles looking for " << defaultStyleName() << ", to delete it. Found " << s << endl;
            if(s) // delete the standard style.
                removeStyle(s);
            defaultStyleDeleted = true;
        }

        KWTableStyle *sty = new KWTableStyle( QString::null, 0, 0 );
        // Load the style
        sty->loadOasis( styleElem, context, paragraphStyles, frameStyles );
        // Style created, now let's try to add it
        sty = static_cast<KWTableStyle *>( addStyle( sty ) );

        kDebug() << " Loaded table cell style " << sty->name() << " - now " << count() << " styles" << endl;
        ++stylesLoaded;
    }
    return stylesLoaded;
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
        m_displayName = i18nc( "Style name", m_name.toUtf8() );
    } else
        kWarning() << "No NAME tag in table style!" << endl;

    element = parentElem.namedItem( "PFRAMESTYLE" ).toElement();
    m_frameStyle = 0;
    if ( ( !element.isNull() ) && ( element.hasAttribute("name") ) )
        m_frameStyle = _doc->frameStyleCollection()->findStyleByDisplayName( element.attribute( "name" ) );

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
        m_paragStyle = _doc->styleCollection()->findStyleByDisplayName( element.attribute( "name" ) );

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
        element.setAttribute( "name", m_frameStyle->displayName() );
    }
    if (m_paragStyle)
    {
        element = doc.createElement( "PSTYLE" );
        parentElem.appendChild( element );
        element.setAttribute( "name", m_paragStyle->displayName() );
    }

}

KWTableStyle *KWTableStyle::loadStyle( QDomElement & parentElem, KWDocument *_doc, int docVersion )
{
    return new KWTableStyle( parentElem, _doc, docVersion );
}

void KWTableStyle::saveOasis( KoGenStyles& mainStyles, KoSavingContext& /*savingContext*/ ) const
{
    KoGenStyle tableCellStyle( KWDocument::STYLE_TABLE_CELL_USER, "table-cell" );
    tableCellStyle.addAttribute( "style:display-name", displayName() );
    tableCellStyle.addProperty( "koffice:frame-style-name", m_frameStyle->name() );
    tableCellStyle.addProperty( "koffice:paragraph-style-name", m_paragStyle->name() );

    // try to preserve existing internal name, if it looks adequate (no spaces)
    // ## TODO: check XML-Schemacs NCName conformity
    const bool nameIsConform = !m_name.isEmpty() && m_name.find( ' ' ) == -1;
    QString newName;
    if ( nameIsConform )
        newName = mainStyles.lookup( tableCellStyle, m_name, KoGenStyles::DontForceNumbering );
    else
        newName = mainStyles.lookup( tableCellStyle, "tc" );
    const_cast<KWTableStyle*>( this )->m_name = newName;
}

void KWTableStyle::loadOasis( QDomElement & styleElem, KoOasisContext& context, const KoStyleCollection& paragraphStyles, const KWFrameStyleCollection& frameStyles )
{
    // Load name
    m_name = styleElem.attributeNS( KoXmlNS::style, "name", QString::null );
    m_displayName = styleElem.attributeNS( KoXmlNS::style, "display-name", QString::null );
    if ( m_displayName.isEmpty() )
        m_displayName = m_name;
    kDebug() << k_funcinfo << m_name << " " << m_displayName << endl;

    KoStyleStack& styleStack = context.styleStack();
    styleStack.setTypeProperties( "table-cell" );

    styleStack.save();
    context.addStyles( &styleElem, "table-cell" ); // Load all parents - only because we don't support inheritance.

    const QString frameStyleName = styleStack.attributeNS( KoXmlNS::koffice, "frame-style-name" );
    m_frameStyle = frameStyles.findStyle( frameStyleName );
    if ( !m_frameStyle )
        kWarning(32001) << "Frame style " << frameStyleName << " not found!" << endl;

    const QString paragraphStyleName = styleStack.attributeNS( KoXmlNS::koffice, "paragraph-style-name" );
    m_paragStyle = paragraphStyles.findStyle( paragraphStyleName );
    if ( !m_paragStyle )
        kWarning(32001) << "Paragraph style " << paragraphStyleName << " not found!" << endl;

    styleStack.restore();
}
