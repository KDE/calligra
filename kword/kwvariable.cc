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
#include <koVariable.h>
#include "mailmerge.h"
#include "kwdoc.h"

#include <klocale.h>
#include <kdebug.h>


KWVariableCollection::KWVariableCollection()
    : KoVariableCollection()
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
        return new KWFootNoteVariable( textdoc, (NoteType)subtype /*TODO*/, coll->format("NUMBER"), this );
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
        m_pgNum = m_doc->getPages()+m_varColl->variableSetting()->startingPage()-1;
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
    return m_doc->getMailMergeDataBase()->getValue( m_name );
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

KWFootNoteVariable::KWFootNoteVariable( KoTextDocument *textdoc, NoteType noteType, KoVariableFormat *varFormat, KoVariableCollection *varColl )
    : KoVariable( textdoc, varFormat, varColl ), m_num( 0 ), m_noteType( noteType ), m_frameset( 0L )
{
}

void KWFootNoteVariable::saveVariable( QDomElement &parentElem )
{
    QDomElement pgNumElem = parentElem.ownerDocument().createElement( "FOOTNOTE" );
    parentElem.appendChild( pgNumElem );
    pgNumElem.setAttribute( "subtype", 0 ); // the only kind currently
    pgNumElem.setAttribute( "value", m_num );
}

void KWFootNoteVariable::load( QDomElement &elem )
{
    KoVariable::load( elem );
    QDomElement footnoteElem = elem.namedItem( "FOOTNOTE" ).toElement();
    if (!footnoteElem.isNull())
    {
        //m_subtype = footnoteElem.attribute("subtype").toInt();
        m_num = footnoteElem.attribute("value").toInt();
    }
}

QString KWFootNoteVariable::text()
{
    KoVariableNumberFormat * format = dynamic_cast<KoVariableNumberFormat *>( m_varFormat );
    if ( format )
        return format->convert( m_num );
    return QString::null;
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
