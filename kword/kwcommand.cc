/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#include "kwdoc.h"
#include "kwcommand.h"
#include "kwtextframeset.h"
#include "kwgroupmanager.h"
#include <qrichtext_p.h>
#include <kdebug.h>

// This is automatically called by KCommandHistory's redo action when redo is activated
void KWTextCommand::execute()
{
    m_textfs->redo();
}

// This is automatically called by KCommandHistory's undo action when undo is activated
void KWTextCommand::unexecute()
{
    m_textfs->undo();
}


KWTextDeleteCommand::KWTextDeleteCommand(
    QTextDocument *d, int i, int idx, const QArray<QTextStringChar> &str,
    const QValueList< QVector<QStyleSheetItem> > &os,
    const QValueList<KWParagLayout> &oldParagLayouts )
    : QTextDeleteCommand( d, i, idx, str, os, QValueList<QStyleSheetItem::ListStyle>(), QArray<int>() )
{
    // Note that we don't pass aligns and liststyles to QTextDeleteCommand.
    // We'll handle them here, as part of the rest, since they are in the paraglayouts
    m_oldParagLayouts = oldParagLayouts;
}

QTextCursor * KWTextDeleteCommand::execute( QTextCursor *c )
{
    QTextCursor * cr = QTextDeleteCommand::execute(c);
    //Can't see anything special to do here
    return cr;
}

QTextCursor * KWTextDeleteCommand::unexecute( QTextCursor *c )
{
    // Keep a ref to the first parag before changing anything
    QTextParag *s = doc ? doc->paragAt( id ) : parag;
    // Let QRichText undo what it can
    QTextCursor * cr = QTextDeleteCommand::unexecute(c);
    // Now restore the parag layouts (i.e. KWord specific stuff)
    QValueList<KWParagLayout>::Iterator lit = m_oldParagLayouts.begin();
    int i = 0;
    QTextParag *p = s;
    while ( p ) {
        if ( lit != m_oldParagLayouts.end() )
            static_cast<KWTextParag*>(p)->setParagLayout( *lit );
        else
            break;
        //if ( s == cr->parag() )
        //    break;
        p = p->next();
        ++i;
        ++lit;
    }
    return cr;
}

KWTextParagCommand::KWTextParagCommand( QTextDocument *d, int fParag, int lParag,
                                        const QValueList<KWParagLayout> &oldParagLayouts,
                                        KWParagLayout newParagLayout,
                                        Flags flags,
                                        QStyleSheetItem::Margin margin )
    : QTextCommand( d ), firstParag( fParag ), lastParag( lParag ), m_oldParagLayouts( oldParagLayouts ),
      m_newParagLayout( newParagLayout ), m_flags( flags ), m_margin( margin )
{
}

QTextCursor * KWTextParagCommand::execute( QTextCursor *c )
{
    //kdDebug() << "KWTextParagCommand::execute" << endl;
    KWTextParag *p = static_cast<KWTextParag *>(doc->paragAt( firstParag ));
    if ( !p )
    {
        kdDebug() << "KWTextParagCommand::execute paragraph " << firstParag << "not found" << endl;
        return c;
    }
     QList<KoTabulator>tmp;
     QListIterator<KoTabulator> it(tmp) ;
    while ( p ) {
        switch ( m_flags ) {
            case All:
                p->setParagLayout( m_newParagLayout );
                break;
            case Alignment:
                p->setAlignment( m_newParagLayout.alignment );
                break;
            case Counter:
                p->setCounter( m_newParagLayout.counter );
                break;
            case Margin:
                p->setMargin( static_cast<QStyleSheetItem::Margin>(m_margin), m_newParagLayout.margins[m_margin] );
                break;
	    case LineSpacing:
	        p->setLineSpacing( m_newParagLayout.lineSpacing );
                break;
	    case Borders:
          	p->setLeftBorder(m_newParagLayout.leftBorder);
	        p->setRightBorder(m_newParagLayout.rightBorder);
	        p->setBottomBorder(m_newParagLayout.bottomBorder);
	        p->setTopBorder(m_newParagLayout.topBorder);
	        break;
            case Tabulator:
                tmp.clear();
                it=m_newParagLayout.m_tabList ;
                for ( it.toFirst(); it.current(); ++it ) {
                    KoTabulator *t = new KoTabulator;
                    t->type = it.current()->type;
                    t->mmPos = it.current()->mmPos;
                    t->inchPos = it.current()->inchPos;
                    t->ptPos = it.current()->ptPos;
                    tmp.append( t );
                }
                p->setTabList(&tmp);
                break;
            default:
                kdDebug() << "Houston we have a problem" << endl;
                break;
        }
        if ( p->paragId() == lastParag )
            break;
        p = static_cast<KWTextParag *>(p->next());
    }
    //kdDebug() << "KWTextParagCommand::execute done" << endl;
    return c;
}

QTextCursor * KWTextParagCommand::unexecute( QTextCursor *c )
{
    KWTextParag *p = static_cast<KWTextParag *>(doc->paragAt( firstParag ));
    if ( !p )
    {
        kdDebug() << "KWTextParagCommand::unexecute paragraph " << firstParag << "not found" << endl;
        return c;
    }
    QValueList<KWParagLayout>::Iterator lit = m_oldParagLayouts.begin();
    while ( p ) {
        if ( lit == m_oldParagLayouts.end() )
        {
            kdDebug() << "KWTextParagCommand::unexecute m_oldParagLayouts not big enough!" << endl;
            break;
        }
        QList<KoTabulator> tmp;
        QListIterator<KoTabulator> it(tmp);
        switch ( m_flags ) {
            case All:
                p->setParagLayout( *lit );
                break;
            case Alignment:
                p->setAlignment( (*lit).alignment );
                break;
            case Counter:
                //kdDebug() << "KWTextParagCommand::unexecute restoring counter " << (*lit).counter.counterType << endl;
                p->setCounter( (*lit).counter );
                break;
            case Margin:
                p->setMargin( static_cast<QStyleSheetItem::Margin>(m_margin), (*lit).margins[m_margin] );
                break;
            case LineSpacing:
                p->setLineSpacing( (*lit).lineSpacing );
                break;
	    case Borders:
          	p->setLeftBorder((*lit).leftBorder);
	        p->setRightBorder((*lit).rightBorder);
	        p->setBottomBorder((*lit).bottomBorder);
	        p->setTopBorder((*lit).topBorder);
	        break;
            case Tabulator:
                tmp.clear();
                it= (*lit).m_tabList ;
                for ( it.toFirst(); it.current(); ++it ) {
                    KoTabulator *t = new KoTabulator;
                    t->type = it.current()->type;
                    t->mmPos = it.current()->mmPos;
                    t->inchPos = it.current()->inchPos;
                    t->ptPos = it.current()->ptPos;
                    tmp.append( t );
                }
                p->setTabList(&tmp);
                break;
            default:
                kdDebug() << "KWTextParagCommand::unexecute unhandled flag " << m_flags << endl;
        }
        if ( p->paragId() == lastParag )
            break;
        p = static_cast<KWTextParag *>(p->next());
        ++lit;
    }
    return c;
}

//////////

KWParagFormatCommand::KWParagFormatCommand( QTextDocument *d, int fParag, int lParag,
                                                          const QValueList<QTextFormat *> &oldFormats,
                                                          QTextFormat * newFormat )
    : QTextCommand( d ), firstParag( fParag ), lastParag( lParag ), m_oldFormats( oldFormats ),
      m_newFormat( newFormat )
{
    QValueList<QTextFormat *>::Iterator lit = m_oldFormats.begin();
    for ( ; lit != m_oldFormats.end() ; ++lit )
        (*lit)->addRef();
}

KWParagFormatCommand::~KWParagFormatCommand()
{
    QValueList<QTextFormat *>::Iterator lit = m_oldFormats.begin();
    for ( ; lit != m_oldFormats.end() ; ++lit )
        (*lit)->removeRef();
}

QTextCursor * KWParagFormatCommand::execute( QTextCursor *c )
{
    KWTextParag *p = static_cast<KWTextParag *>(doc->paragAt( firstParag ));
    if ( !p )
    {
        kdDebug() << "KWTextParagCommand::execute paragraph " << firstParag << "not found" << endl;
        return c;
    }
    while ( p ) {
        p->setFormat( m_newFormat );
        p->invalidate(0);
        if ( p->paragId() == lastParag )
            break;
        p = static_cast<KWTextParag *>(p->next());
    }
    return c;
}

QTextCursor * KWParagFormatCommand::unexecute( QTextCursor *c )
{
    QTextParag *p = doc->paragAt( firstParag );
    if ( !p )
    {
        kdDebug() << "KWParagFormatCommand::unexecute paragraph " << firstParag << "not found" << endl;
        return c;
    }
    QValueList<QTextFormat *>::Iterator lit = m_oldFormats.begin();
    while ( p ) {
        if ( lit == m_oldFormats.end() )
        {
            kdDebug() << "KWParagFormatCommand::unexecute m_oldFormats not big enough!" << endl;
            break;
        }
        p->setFormat( (*lit) );
        if ( p->paragId() == lastParag )
            break;
        p = p->next();
        ++lit;
    }
    return c;
}

KWPasteCommand::KWPasteCommand( QTextDocument *d, int parag, int idx,
                                const QCString & data )
    : QTextCommand( d ), m_parag( parag ), m_idx( idx ), m_data( data )
{
}

QTextCursor * KWPasteCommand::execute( QTextCursor *c )
{
    QTextParag *firstParag = doc->paragAt( m_parag );
    if ( !firstParag ) {
        qWarning( "can't locate parag at %d, last parag: %d", m_parag, doc->lastParag()->paragId() );
        return 0;
    }
    cursor.setParag( firstParag );
    cursor.setIndex( m_idx );
    QDomDocument domDoc;
    domDoc.setContent( m_data );
    QDomElement elem = domDoc.documentElement();
    // We iterate twice over the list of paragraphs.
    // First time to gather the text,
    // second time to apply the character & paragraph formatting
    QString text;

    QDomNodeList listParagraphs = elem.elementsByTagName ( "PARAGRAPH" );
    uint count =listParagraphs.count();
    for (unsigned int item = 0; item < count; item++)
    {
        QDomElement paragElem = listParagraphs.item( item ).toElement();
        QString s = paragElem.namedItem( "TEXT" ).toElement().text();
        if ( !text.isEmpty() )
            text += '\n';
        text += s;
    }

    cursor.insert( text, true );

    // Move cursor to the end
    c->setParag( firstParag );
    c->setIndex( m_idx );
    for ( int i = 0; i < (int)text.length(); ++i )
        c->gotoRight();

    KWTextParag * parag = static_cast<KWTextParag *>(firstParag);
    for (unsigned int item = 0; item < count; item++)
    {
        QDomElement paragElem = listParagraphs.item( item ).toElement();
        // First line (if appending to non-empty line) : apply offset to formatting, don't apply parag layout
        if ( item == 0 && m_idx > 0 )
        {
            // First load the default format, but only apply it to our new chars
            QDomElement layout = paragElem.namedItem( "LAYOUT" ).toElement();
            if ( !layout.isNull() )
            {
                QDomElement formatElem = layout.namedItem( "FORMAT" ).toElement();
                if ( !formatElem.isNull() )
                {
                    QTextFormat f = parag->loadFormat( formatElem, 0L );
                    QTextFormat * defaultFormat = doc->formatCollection()->format( &f );
                    // Last paragraph (i.e. only one in all) : some of the text might be from before the paste
                    int endIndex = (item == count-1) ? c->index() : parag->string()->length() - 1;
                    parag->setFormat( m_idx, endIndex - m_idx, defaultFormat, TRUE );
                }
            }

            parag->loadFormatting( paragElem, m_idx );
        }
        else
        {
            parag->loadLayout( paragElem );
            // Last paragraph: some of the text might be from before the paste
            int len = (item == count-1) ? c->index() : parag->string()->length();
            // Apply default format
            parag->setFormat( 0, len, parag->paragFormat(), TRUE );
            parag->loadFormatting( paragElem );
        }
        parag->format();
        parag->setChanged( TRUE );
        parag = static_cast<KWTextParag *>(parag->next());
    }
    m_lastParag = c->parag()->paragId();
    m_lastIndex = c->index();
    return c;
}

QTextCursor * KWPasteCommand::unexecute( QTextCursor *c )
{
    QTextParag *firstParag = doc->paragAt( m_parag );
    if ( !firstParag ) {
        qWarning( "can't locate parag at %d, last parag: %d", m_parag, doc->lastParag()->paragId() );
        return 0;
    }
    cursor.setParag( firstParag );
    cursor.setIndex( m_idx );
    doc->setSelectionStart( QTextDocument::Temp, &cursor );

    QTextParag *lastParag = doc->paragAt( m_lastParag );
    if ( !lastParag ) {
        qWarning( "can't locate parag at %d, last parag: %d", m_lastParag, doc->lastParag()->paragId() );
        return 0;
    }
    cursor.setParag( lastParag );
    cursor.setIndex( m_lastIndex );
    doc->setSelectionEnd( QTextDocument::Temp, &cursor );
    doc->removeSelectedText( QTextDocument::Temp, c /* sets c to the correct position */ );
    return c;
}

////////////////////////// Frame commands ////////////////////////////////

KWFrameBorderCommand::KWFrameBorderCommand( const QString &name,KWDocument *_doc,QList<FrameIndex> &_listFrameIndex, QList<FrameBorderTypeStruct> &_frameTypeBorder,const Border & _newBorder):
    KCommand(name),
    m_IndexFrame(_listFrameIndex),
    m_oldBorderFrameType(_frameTypeBorder),
    m_newBorder( _newBorder),
    m_pDoc(_doc)
{
}

void KWFrameBorderCommand::execute()
{
    FrameIndex *tmp;
    for ( tmp=m_IndexFrame.first(); tmp != 0; tmp=m_IndexFrame.next() )
    {
        KWFrameSet *frameSet =m_pDoc->getFrameSet(tmp->m_iFrameSetIndex);
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        FrameBorderTypeStruct *tmpFrameStruct=m_oldBorderFrameType.at(m_IndexFrame.find(tmp));
        switch( tmpFrameStruct->m_EFrameType)
        {
            case  FBLeft:
                frame->setLeftBorder(m_newBorder);
                break;
            case FBRight:
                frame->setRightBorder(m_newBorder);
                break;
            case FBTop:
                frame->setTopBorder(m_newBorder);
                break;
            case FBBottom:
                frame->setBottomBorder(m_newBorder);
                break;
            default:
                break;
        }
    }
    m_pDoc->repaintAllViews();
}

void KWFrameBorderCommand::unexecute()
{
    FrameIndex *tmp;
    for ( tmp=m_IndexFrame.first(); tmp != 0; tmp=m_IndexFrame.next() )
    {
        KWFrameSet *frameSet =m_pDoc->getFrameSet(tmp->m_iFrameSetIndex);
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        FrameBorderTypeStruct *tmpFrameStruct=m_oldBorderFrameType.at(m_IndexFrame.find(tmp));
        switch(tmpFrameStruct->m_EFrameType)
        {
            case  FBLeft:
                frame->setLeftBorder(tmpFrameStruct->m_OldBorder);
                break;
            case FBRight:
                frame->setRightBorder(tmpFrameStruct->m_OldBorder);
                break;
            case FBTop:
                frame->setTopBorder(tmpFrameStruct->m_OldBorder);
                break;
            case FBBottom:
                frame->setBottomBorder(tmpFrameStruct->m_OldBorder);
                break;
            default:
                break;
        }
    }
    //update frames
    m_pDoc->repaintAllViews();
}

KWFrameBackGroundColorCommand::KWFrameBackGroundColorCommand( const QString &name,KWDocument *_doc,QList<FrameIndex> &_listFrameIndex, QList<QBrush> &_oldBrush,const QBrush & _newColor ):
    KCommand(name),
    m_IndexFrame(_listFrameIndex),
    m_oldBackGroundColor(_oldBrush),
    m_newColor( _newColor),
    m_pDoc(_doc)
{
}

void KWFrameBackGroundColorCommand::execute()
{
    FrameIndex *tmp;
    for ( tmp=m_IndexFrame.first(); tmp != 0; tmp=m_IndexFrame.next() )
    {
        KWFrameSet *frameSet =m_pDoc->getFrameSet(tmp->m_iFrameSetIndex);
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        frame->setBackgroundColor(m_newColor);
    }
    //update frame
    m_pDoc->repaintAllViews();
}

void KWFrameBackGroundColorCommand::unexecute()
{
    FrameIndex *tmp;
    for ( tmp=m_IndexFrame.first(); tmp != 0; tmp=m_IndexFrame.next() )
    {
        KWFrameSet *frameSet =m_pDoc->getFrameSet(tmp->m_iFrameSetIndex);
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        QBrush *tmpFrameStruct=m_oldBackGroundColor.at(m_IndexFrame.find(tmp));
        frame->setBackgroundColor(*tmpFrameStruct);
    }

    //update frames
    m_pDoc->repaintAllViews();
}


KWFrameResizeCommand::KWFrameResizeCommand( const QString &name,KWDocument *_doc,FrameIndex _frameIndex,FrameResizeStruct _frameResize ) :
    KCommand(name),
    m_IndexFrame(_frameIndex),
    m_FrameResize(_frameResize),
    m_pDoc(_doc)
{
}

void KWFrameResizeCommand::execute()
{
    KWFrameSet *frameSet =m_pDoc->getFrameSet(m_IndexFrame.m_iFrameSetIndex);
    KWFrame *frame=frameSet->getFrame(m_IndexFrame.m_iFrameIndex);
    frame->setCoords(m_FrameResize.sizeOfEnd.left(),m_FrameResize.sizeOfEnd.top(),m_FrameResize.sizeOfEnd.right(),m_FrameResize.sizeOfEnd.bottom());

    KWGroupManager *grpMgr = frame->getFrameSet()->getGroupManager();
    if (grpMgr) {
        grpMgr->recalcCols();
        grpMgr->recalcRows();
        grpMgr->updateTempHeaders();
        //repaintTableHeaders( grpMgr );
    }

    frame->setSelected(true);
    m_pDoc->repaintAllViews();
}

void KWFrameResizeCommand::unexecute()
{
    KWFrameSet *frameSet =m_pDoc->getFrameSet(m_IndexFrame.m_iFrameSetIndex);
    KWFrame *frame=frameSet->getFrame(m_IndexFrame.m_iFrameIndex);
    frame->setCoords(m_FrameResize.sizeOfBegin.left(),m_FrameResize.sizeOfBegin.top(),m_FrameResize.sizeOfBegin.right(),m_FrameResize.sizeOfBegin.bottom());
    KWGroupManager *grpMgr = frame->getFrameSet()->getGroupManager();
    if (grpMgr) {
        grpMgr->recalcCols();
        grpMgr->recalcRows();
        grpMgr->updateTempHeaders();
        //repaintTableHeaders( grpMgr );
    }
    frame->setSelected(true);
    //update frames
    m_pDoc->repaintAllViews();
}


KWFrameMoveCommand::KWFrameMoveCommand( const QString &name,KWDocument *_doc,QList<FrameIndex> &_frameIndex,QList<FrameResizeStruct>&_frameMove  ) :
    KCommand(name),
    m_IndexFrame(_frameIndex),
    m_frameMove(_frameMove),
    m_pDoc(_doc)
{
}

void KWFrameMoveCommand::execute()
{
    FrameIndex *tmp;
    for ( tmp=m_IndexFrame.first(); tmp != 0; tmp=m_IndexFrame.next() )
    {
        KWFrameSet *frameSet =m_pDoc->getFrameSet(tmp->m_iFrameSetIndex);
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        FrameResizeStruct *tmpFrameMove=m_frameMove.at(m_IndexFrame.find(tmp));
        frame->setCoords(tmpFrameMove->sizeOfEnd.left(),tmpFrameMove->sizeOfEnd.top(),tmpFrameMove->sizeOfEnd.right(),tmpFrameMove->sizeOfEnd.bottom());
        frame->setSelected(true);
    }

    m_pDoc->repaintAllViews();
}

void KWFrameMoveCommand::unexecute()
{
    FrameIndex *tmp;
    for ( tmp=m_IndexFrame.first(); tmp != 0; tmp=m_IndexFrame.next() )
    {
        KWFrameSet *frameSet =m_pDoc->getFrameSet(tmp->m_iFrameSetIndex);
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        FrameResizeStruct *tmpFrameMove=m_frameMove.at(m_IndexFrame.find(tmp));
        frame->setCoords(tmpFrameMove->sizeOfBegin.left(),tmpFrameMove->sizeOfBegin.top(),tmpFrameMove->sizeOfBegin.right(),tmpFrameMove->sizeOfBegin.bottom());
        frame->setSelected(true);
    }

    m_pDoc->repaintAllViews();
}
