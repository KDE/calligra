/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kwvariable.h"
#include "kwtextframeset.h"
#include "kwtextdocument.h"
#include <koVariable.h>
#include "mailmerge.h"
#include "kwdoc.h"

#include <klocale.h>
#include <kdebug.h>
#include <qdom.h>
#include <qtimer.h>

KWVariableSettings::KWVariableSettings() : KoVariableSettings()
{
}

void KWVariableSettings::changeFootNoteCounter( KoParagCounter _c )
{
    m_footNoteCounter = _c;
}

void KWVariableSettings::changeEndNoteCounter( KoParagCounter _c )
{
    m_endNoteCounter = _c;
}


void KWVariableSettings::save( QDomElement &parentElem )
{
    KoVariableSettings::save( parentElem );
}

void KWVariableSettings::load( QDomElement &elem )
{
    KoVariableSettings::load( elem );
}

KWVariableCollection::KWVariableCollection(KWVariableSettings *_setting)
    : KoVariableCollection(_setting)
{
}


KoVariable *KWVariableCollection::createVariable( int type, int subtype, KoVariableFormatCollection * coll, KoVariableFormat *varFormat,KoTextDocument *textdoc, KoDocument * doc )
{
    KWDocument *m_doc=static_cast<KWDocument*>(doc);
    switch(type) {
    case VT_PGNUM:
        if ( !varFormat )
            varFormat = (subtype == KoPgNumVariable::VST_CURRENT_SECTION) ? coll->format("STRING") : coll->format("NUMBER");
        return new KWPgNumVariable( textdoc, subtype, varFormat, this, m_doc );
    case VT_MAILMERGE:
	return new KWMailMergeVariable( textdoc, QString::null, coll->format("STRING"), this, m_doc );
    case VT_FOOTNOTE:
        if ( !varFormat )
            varFormat =  coll->format("NUMBER");
        return new KWFootNoteVariable( textdoc, varFormat, this, m_doc );
    default:
        return KoVariableCollection::createVariable( type, subtype, coll, varFormat, textdoc, doc );
    }
}

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/
KWPgNumVariable::KWPgNumVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat ,KoVariableCollection *_varColl, KWDocument *doc  )
    : KoPgNumVariable( textdoc, subtype, varFormat ,_varColl ), m_doc(doc)
{
}

void KWPgNumVariable::recalc()
{
    if ( m_subtype == VST_PGNUM_TOTAL )
    {
        m_varValue = QVariant(m_doc->getPages()+m_varColl->variableSetting()->startingPage()-1);
        resize();
    }
    // The other cases are handled by the more dynamic code in KWTextFrameSet::drawFrame()
}

void KWPgNumVariable::setVariableSubType( short int type)
{
    m_subtype=type;
    setVariableFormat((m_subtype == KWPgNumVariable::VST_CURRENT_SECTION) ? m_doc->variableFormatCollection()->format("STRING") : m_doc->variableFormatCollection()->format("NUMBER"));
}


/******************************************************************/
/* Class: KWMailMergeVariable                                  */
/******************************************************************/
KWMailMergeVariable::KWMailMergeVariable( KoTextDocument *textdoc, const QString &name, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KWDocument *doc  )
    : KoMailMergeVariable( textdoc, name, varFormat,_varColl ), m_doc(doc)
{
}


QString KWMailMergeVariable::value() const
{
    return m_doc->getMailMergeDataBase()->getValue( m_varValue.toString() );
}

QString KWMailMergeVariable::text()
{
    // ## should use a format maybe
    QString v = value();
    if ( v == name() )
        return "<" + v + ">";
    return v;
}

void KWMailMergeVariable::recalc()
{
    resize();
}

/////////////

KWFootNoteVariable::KWFootNoteVariable( KoTextDocument *textdoc, KoVariableFormat *varFormat, KoVariableCollection *varColl, KWDocument *doc )
    : KoVariable( textdoc, varFormat, varColl ),
      m_doc(doc),
      m_frameset( 0L ),
      m_numberingType( Auto )
{
    m_varValue = QVariant( 0 );
}

void KWFootNoteVariable::setNumberingType( Numbering _type )
{
    m_numberingType = _type;
    //delete m_varFormat;
    setVariableFormat((_type == Manual) ? m_doc->variableFormatCollection()->format("STRING") : m_doc->variableFormatCollection()->format("NUMBER"));

}


void KWFootNoteVariable::saveVariable( QDomElement &parentElem )
{
    QDomElement footnoteElem = parentElem.ownerDocument().createElement( "FOOTNOTE" );
    parentElem.appendChild( footnoteElem );
    //footnoteElem.setAttribute( "subtype", 0 );
    footnoteElem.setAttribute( "value", m_varValue.toString() );
    footnoteElem.setAttribute( "notetype", m_noteType == FootNote ? "footnote" : "endnote" );
    footnoteElem.setAttribute( "numberingtype", m_numberingType == Auto ? "auto" : "manual" );
    Q_ASSERT( m_frameset );
    footnoteElem.setAttribute( "frameset", m_frameset->getName() );
}

void KWFootNoteVariable::load( QDomElement &elem )
{
    KoVariable::load( elem );
    QDomElement footnoteElem = elem.namedItem( "FOOTNOTE" ).toElement();
    if (!footnoteElem.isNull())
    {
        //m_subtype = footnoteElem.attribute("subtype").toInt();
        QString str = footnoteElem.attribute("notetype").lower();
        if ( str == "footnote" )
            m_noteType = FootNote;
        else if ( str == "endnote" )
            m_noteType = EndNote;
        else
            kdWarning() << "Unknown footnote type: '" << str << "'" << endl;

        str = footnoteElem.attribute("numberingtype").lower();
        if ( str == "auto" )
            m_numberingType = Auto;
        else if ( str == "manual")
            m_numberingType = Manual;
        else
            kdWarning() << "Unknown footnote numbering: '" << str << "'" << endl;

        if ( m_numberingType == Auto )
            m_varValue = QVariant(footnoteElem.attribute("value").toInt());
        else
            m_varValue = QVariant(footnoteElem.attribute("value"));

        str = footnoteElem.attribute("frameset");
        m_doc->addFootNoteRequest( str, this );
    }
}

QString KWFootNoteVariable::text()
{
    if ( m_numberingType == Auto )
    {
        if (m_noteType == FootNote )
            return m_varFormat->convert( QVariant( m_varValue.toInt()+ static_cast<KWVariableSettings*>(m_varColl->variableSetting())->footNoteCounter().startNumber()-1) );
        else
            return m_varFormat->convert( QVariant( m_varValue.toInt()+ static_cast<KWVariableSettings*>(m_varColl->variableSetting())->endNoteCounter().startNumber()-1) );
    }
    else
        return m_varFormat->convert( m_varValue );
}

void KWFootNoteVariable::drawCustomItem( QPainter* p, int x, int y, int /*cx*/, int /*cy*/, int /*cw*/, int /*ch*/, const QColorGroup& cg, bool selected, const int _offset ) // TODO s/const int/int/
{
    KoTextFormat * fmt = format();
    KoZoomHandler * zh = textDocument()->paintingZoomHandler();

    // Force drawing as "superscript" - hmm, the formatting will use too big font metrics though.
    QFont font( fmt->screenFont( zh ) );
    int pointSize = ( ( font.pointSize() * 2 ) / 3 );
    font.setPointSize( pointSize );

    int offset = _offset;
    if ( offset == 0 )
    {
        int h = zh->layoutUnitToPixelY( /*_y HACK,*/ height );
        offset = -( h - QFontMetrics(font).height() );
    }

    QColor textColor( fmt->color() );
    drawCustomItemHelper( p, x, y, cg, selected, offset, fmt, font, textColor );
}

void KWFootNoteVariable::move( int x, int y )
{
    KoVariable::move( x, y );

    Q_ASSERT( m_frameset );
    if (!m_frameset )
        return;
    Q_ASSERT( !m_frameset->isDeleted() );
    if ( m_frameset->isDeleted() )
        return;

    // Find out the position of the footnote variable in document coordinates.
    int paragy = paragraph()->rect().y();
    KWTextFrameSet * fs = static_cast<KWTextDocument *>(textDocument())->textFrameSet();
    KoPoint dPoint;
    KWFrame* containingFrame = fs->internalToDocument( QPoint( x, paragy + y + height ), dPoint );
    if ( containingFrame )
    {
        // Ok, the (bottom of the) footnote variable is at dPoint.
        KWFrame* footNoteFrame = m_frameset->frame( 0 );
        int framePage = footNoteFrame->pageNum();
        int varPage = containingFrame->pageNum();
        if ( framePage != varPage )
        {
            kdDebug(32001) << "Footnote var at page " << varPage << ", footnote frame at page " << framePage << " -> recalcFrames()" << endl;
            fs->kWordDocument()->recalcFrames( QMIN( varPage, framePage ), -1 );
        }

        // TODO handle the case where dPoint is too far down to be on top of the associated footnote.... somehow
    } else
    {
        // This can happen if the page hasn't been created yet
        //kdDebug(32001) << "KWFootNoteVariable::move internalToDocument returned 0L for " << x << ", " << y+paragy << endl;
    }
}

void KWFootNoteVariable::setDeleted( bool del )
{
    kdDebug() << "KWFootNoteVariable::setDeleted " << del << endl;
    if ( del )
    {
        Q_ASSERT( m_frameset );
        if ( m_frameset ) {
            m_frameset->deleteAllFrames(); // Important, because we don't want to save it!
            m_frameset->setVisible( false );
        }
    }
    else
    {
        Q_ASSERT( m_frameset );
        if ( m_frameset ) {
            kdDebug() << "Making frameset " << m_frameset << " visible" << endl;
            m_frameset->setVisible( true );
            m_frameset->createInitialFrame( 0 ); // Page number shouldn't matter (see recalcFrames below).
            Q_ASSERT( m_frameset->isVisible() );
        }
    }

    // hmm, maybe compress all the stuff below and do only once
    // (e.g. when deleting multiple footnotes)?
    // (but we can't really delay it with a sst, the formatMore after undo/redo
    // needs this to be done already, I think). Bah.

    // Re-number footnote variables
    KWTextFrameSet * textfs = static_cast<KWTextDocument *>(textDocument())->textFrameSet();
    textfs->renumberFootNotes();

    m_doc->recalcFrames();

    if (!del)
        m_frameset->layout(); // format its text, so that it resizes the frame

    // Does this compress? Probably not.
    QTimer::singleShot( 0, m_doc, SLOT( slotRepaintAllViews() ) );
}
