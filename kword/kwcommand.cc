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
#include "kwview.h"
#include "kwcommand.h"
#include "kwtextframeset.h"
#include "kwtableframeset.h"
#include "kwanchor.h"

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
    const CustomItemsMap & customItemsMap,
    const QValueList<KWParagLayout> &oldParagLayouts )
    : QTextDeleteCommand( d, i, idx, str,
                          QValueList< QVector<QStyleSheetItem> >(),
                          QValueList<QStyleSheetItem::ListStyle>(),
                          QArray<int>() ),
      m_oldParagLayouts( oldParagLayouts ),
      m_customItemsMap( customItemsMap )
{
    // Note that we don't pass aligns and liststyles to QTextDeleteCommand.
    // We'll handle them here, as part of the rest, since they are in the paraglayouts
}

QTextCursor * KWTextDeleteCommand::execute( QTextCursor *c )
{
    QTextParag *s = doc ? doc->paragAt( id ) : parag;
    if ( !s ) {
        qWarning( "can't locate parag at %d, last parag: %d", id, doc->lastParag()->paragId() );
        return 0;
    }
    cursor.setParag( s );
    cursor.setIndex( index );
    int len = text.size();
    // Detach from custom items. They are already in the map, and we don't
    // want them to be deleted
    for ( int i = 0; i < len; ++i )
    {
        QTextStringChar * ch = cursor.parag()->at( cursor.index() );
        if ( ch->isCustom() )
        {
            static_cast<KWTextCustomItem *>( ch->customItem() )->setDeleted( true );
            ch->loseCustomItem();
        }
        cursor.gotoRight();
    }

    return QTextDeleteCommand::execute(c);
}

QTextCursor * KWTextDeleteCommand::unexecute( QTextCursor *c )
{
    // Keep a ref to the first parag before changing anything
    QTextParag *s = doc ? doc->paragAt( id ) : parag;
    if ( !s ) {
        qWarning( "can't locate parag at %d, last parag: %d", id, doc->lastParag()->paragId() );
        return 0;
    }
    cursor.setParag( s );
    cursor.setIndex( index );
    // Let QRichText undo what it can
    QTextCursor * cr = QTextDeleteCommand::unexecute(c);
    // Set any custom item that we had
    m_customItemsMap.insertItems( cursor, text.size() );

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
                                        int flags,
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
        kdWarning() << "KWTextParagCommand::execute paragraph " << firstParag << "not found" << endl;
        return c;
    }
    while ( p ) {
        if ( ( m_flags & KWParagLayout::Margins ) && m_margin != (QStyleSheetItem::Margin)-1 ) // all
            p->setMargin( static_cast<QStyleSheetItem::Margin>(m_margin), m_newParagLayout.margins[m_margin] );
        else
        {
            p->setParagLayout( m_newParagLayout, m_flags );
        }
        if ( p->paragId() == lastParag )
            break;
        p = static_cast<KWTextParag *>(p->next());
    }
    //kdDebug() << "KWTextParagCommand::execute done" << endl;
    // Set cursor to end of selection. Like in QTextFormatCommand::[un]execute...
    c->setParag( p );
    c->setIndex( p->length()-1 );
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
        if ( m_flags & KWParagLayout::Margins && m_margin != (QStyleSheetItem::Margin)-1 ) // just one
            p->setMargin( static_cast<QStyleSheetItem::Margin>(m_margin), (*lit).margins[m_margin] );
        else
        {
            p->setParagLayout( *lit, m_flags );
        }
        if ( p->paragId() == lastParag )
            break;
        p = static_cast<KWTextParag *>(p->next());
        ++lit;
    }
    // Set cursor to end of selection. Like in QTextFormatCommand::[un]execute...
    c->setParag( p );
    c->setIndex( p->length()-1 );
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

KWPasteTextCommand::KWPasteTextCommand( QTextDocument *d, int parag, int idx,
                                const QCString & data )
    : QTextCommand( d ), m_parag( parag ), m_idx( idx ), m_data( data )
{
}

QTextCursor * KWPasteTextCommand::execute( QTextCursor *c )
{
    QTextParag *firstParag = doc->paragAt( m_parag );
    if ( !firstParag ) {
        qWarning( "can't locate parag at %d, last parag: %d", m_parag, doc->lastParag()->paragId() );
        return 0;
    }
    //kdDebug() << "KWPasteTextCommand::execute m_parag=" << m_parag << " m_idx=" << m_idx
    //          << " firstParag=" << firstParag << " " << firstParag->paragId() << endl;
    cursor.setParag( firstParag );
    cursor.setIndex( m_idx );
    QDomDocument domDoc;
    domDoc.setContent( m_data );
    QDomElement elem = domDoc.documentElement();
    // We iterate twice over the list of paragraphs.
    // First time to gather the text,
    // second time to apply the character & paragraph formatting
    QString text;

    QValueList<QDomElement> listParagraphs;
    QDomElement paragraph = elem.firstChild().toElement();
    bool first = true;
    for ( ; !paragraph.isNull() ; paragraph = paragraph.nextSibling().toElement() )
    {
        if ( paragraph.tagName() == "PARAGRAPH" )
        {
            QString s = paragraph.namedItem( "TEXT" ).toElement().text();
            if ( !first )
                text += '\n';
            else
                first = false;
            text += s;
            listParagraphs.append( paragraph );
        }
    }
    //kdDebug() << "KWPasteTextCommand::execute Inserting text: '" << text << "'" << endl;
    KWTextDocument * textdoc = static_cast<KWTextDocument *>(c->parag()->document());
    KWTextFrameSet * textFs = textdoc->textFrameSet();
    textFs->unzoom();

    cursor.insert( text, true );

    // Move cursor to the end
    c->setParag( firstParag );
    c->setIndex( m_idx );
    for ( int i = 0; i < (int)text.length(); ++i )
        c->gotoRight();

    // Redo the parag lookup because if firstParag was empty, insert() has
    // shifted it down (side effect of splitAndInsertEmptyParag)
    firstParag = doc->paragAt( m_parag );
    KWTextParag * parag = static_cast<KWTextParag *>(firstParag);
    //kdDebug() << "KWPasteTextCommand::execute starting at parag " << parag << " " << parag->paragId() << endl;
    uint count = listParagraphs.count();
    QValueList<QDomElement>::Iterator it = listParagraphs.begin();
    QValueList<QDomElement>::Iterator end = listParagraphs.end();
    for ( uint item = 0 ; it != end ; ++it, ++item )
    {
        if (!parag)
        {
            kdWarning() << "KWPasteTextCommand: parag==0L ! KWord bug, please report." << endl;
            break;
        }
        QDomElement paragElem = *it;
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
                    QTextFormat f = parag->loadFormat( formatElem, 0L, QFont() );
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
            if ( item == 0 ) // This paragraph existed, store its parag layout
                m_oldParagLayout = parag->paragLayout();
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
        //kdDebug() << "KWPasteTextCommand::execute going to next parag: " << parag << endl;
    }
    // In case loadFormatting queued any image request
    KWDocument * doc = textFs->kWordDocument();
    doc->processImageRequests();

    // In case of any inline frameset
    doc->pasteFrames( elem, 0 );

    textFs->zoom( false );

    m_lastParag = c->parag()->paragId();
    m_lastIndex = c->index();
    return c;
}

QTextCursor * KWPasteTextCommand::unexecute( QTextCursor *c )
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

    if ( m_idx == 0 )
        static_cast<KWTextParag *>( firstParag )->setParagLayout( m_oldParagLayout );
    return c;
}

////////////////////////// Frame commands ////////////////////////////////

FrameIndex::FrameIndex( KWFrame *frame )
{
    m_pFrameSet=frame->getFrameSet();
    m_iFrameIndex=m_pFrameSet->getFrameFromPtr(frame);
}

KWFrameBorderCommand::KWFrameBorderCommand( const QString &name, QList<FrameIndex> &_listFrameIndex, QList<FrameBorderTypeStruct> &_frameTypeBorder,const Border & _newBorder):
    KCommand(name),
    m_indexFrame(_listFrameIndex),
    m_oldBorderFrameType(_frameTypeBorder),
    m_newBorder( _newBorder)
{
}

void KWFrameBorderCommand::execute()
{
    FrameIndex *tmp;
    KWDocument *doc = 0L;
    for ( tmp=m_indexFrame.first(); tmp != 0; tmp=m_indexFrame.next() )
    {
        KWFrameSet *frameSet =tmp->m_pFrameSet;
        doc = frameSet->kWordDocument();
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        FrameBorderTypeStruct *tmpFrameStruct=m_oldBorderFrameType.at(m_indexFrame.find(tmp));
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
    if ( doc )
    {
        doc->refreshFrameBorderButton();
        doc->repaintAllViews();
    }
}

void KWFrameBorderCommand::unexecute()
{
    FrameIndex *tmp;
    KWDocument *doc = 0L;
    for ( tmp=m_indexFrame.first(); tmp != 0; tmp=m_indexFrame.next() )
    {
        KWFrameSet *frameSet =tmp->m_pFrameSet;
        doc = frameSet->kWordDocument();
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        FrameBorderTypeStruct *tmpFrameStruct=m_oldBorderFrameType.at(m_indexFrame.find(tmp));
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
    if ( doc )
    {
        doc->refreshFrameBorderButton();
        //update frames
        doc->repaintAllViews();
    }
}

KWFrameBackGroundColorCommand::KWFrameBackGroundColorCommand( const QString &name, QList<FrameIndex> &_listFrameIndex, QList<QBrush> &_oldBrush,const QBrush & _newColor ):
    KCommand(name),
    m_indexFrame(_listFrameIndex),
    m_oldBackGroundColor(_oldBrush),
    m_newColor( _newColor)
{
}

void KWFrameBackGroundColorCommand::execute()
{
    FrameIndex *tmp;
    KWDocument * doc = 0L;
    for ( tmp=m_indexFrame.first(); tmp != 0; tmp=m_indexFrame.next() )
    {
        KWFrameSet *frameSet =tmp->m_pFrameSet;
        doc = frameSet->kWordDocument();
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        frame->setBackgroundColor(m_newColor);
    }
    //update frame
    if ( doc )
        doc->repaintAllViews();
}

void KWFrameBackGroundColorCommand::unexecute()
{
    FrameIndex *tmp;
    KWDocument * doc = 0L;
    for ( tmp=m_indexFrame.first(); tmp != 0; tmp=m_indexFrame.next() )
    {
        KWFrameSet *frameSet =tmp->m_pFrameSet;
        doc = frameSet->kWordDocument();
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        QBrush *tmpFrameStruct=m_oldBackGroundColor.at(m_indexFrame.find(tmp));
        frame->setBackgroundColor(*tmpFrameStruct);
    }

    //update frames
    if ( doc )
        doc->repaintAllViews();
}


KWFrameResizeCommand::KWFrameResizeCommand( const QString &name, FrameIndex _frameIndex, FrameResizeStruct _frameResize ) :
    KCommand(name),
    m_indexFrame(_frameIndex),
    m_FrameResize(_frameResize)
{
}

void KWFrameResizeCommand::execute()
{
    KWFrameSet *frameSet = m_indexFrame.m_pFrameSet;
    ASSERT( frameSet );
    KWFrame *frame = frameSet->getFrame(m_indexFrame.m_iFrameIndex);
    ASSERT( frame );
    frame->setCoords(m_FrameResize.sizeOfEnd.left(),m_FrameResize.sizeOfEnd.top(),m_FrameResize.sizeOfEnd.right(),m_FrameResize.sizeOfEnd.bottom());

    KWTableFrameSet *table = frame->getFrameSet()->getGroupManager();
    if (table) {
        KWTableFrameSet::Cell *cell=dynamic_cast<KWTableFrameSet::Cell *>(frame->getFrameSet());
        if(cell)
        {
            table->recalcCols(cell->m_col,cell->m_row);
            table->recalcRows(cell->m_col,cell->m_row);
        }
        else
        {
           table->recalcCols();
           table->recalcRows();
        }
        table->updateTempHeaders();
        table->refreshSelectedCell();
        //repaintTableHeaders( table );
    }
    KWDocument * doc = frameSet->kWordDocument();

    if(frameSet->isAHeader() || frameSet->isAFooter())
        doc->recalcFrames();

    if(frame->isSelected())
        frame->updateResizeHandles();

    doc->frameChanged( frame );
}

void KWFrameResizeCommand::unexecute()
{
    KWFrameSet *frameSet =m_indexFrame.m_pFrameSet;
    KWFrame *frame=frameSet->getFrame(m_indexFrame.m_iFrameIndex);
    frame->setCoords(m_FrameResize.sizeOfBegin.left(),m_FrameResize.sizeOfBegin.top(),m_FrameResize.sizeOfBegin.right(),m_FrameResize.sizeOfBegin.bottom());
    KWTableFrameSet *table = frame->getFrameSet()->getGroupManager();
    if (table) {
        KWTableFrameSet::Cell *cell=dynamic_cast<KWTableFrameSet::Cell *>(frame->getFrameSet());
        if(cell)
        {
            table->recalcCols(cell->m_col,cell->m_row);
            table->recalcRows(cell->m_col,cell->m_row);
        }
        else
        {
           table->recalcCols();
           table->recalcRows();
        }
        table->updateTempHeaders();
        table->refreshSelectedCell();
        //repaintTableHeaders( table );
    }
    KWDocument * doc = frameSet->kWordDocument();
    if(frameSet->isAHeader() || frameSet->isAFooter())
        doc->recalcFrames();
    if(frame->isSelected())
        frame->updateResizeHandles();
    //update frames
    doc->frameChanged( frame );
}


KWFrameMoveCommand::KWFrameMoveCommand( const QString &name, QList<FrameIndex> &_frameIndex, QList<FrameResizeStruct>&_frameMove  ) :
    KCommand(name),
    m_indexFrame(_frameIndex),
    m_frameMove(_frameMove)
{
}

void KWFrameMoveCommand::execute()
{
    bool needRelayout = false;
    FrameIndex *tmp;
    KWDocument * doc = 0L;
    for ( tmp=m_indexFrame.first(); tmp != 0; tmp=m_indexFrame.next() )
    {
        KWFrameSet *frameSet = tmp->m_pFrameSet;
        doc = frameSet->kWordDocument();
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        FrameResizeStruct *tmpFrameMove=m_frameMove.at(m_indexFrame.find(tmp));
        KWTableFrameSet *table=frameSet->getGroupManager();
        if(table)
        {
            table->moveBy(tmpFrameMove->sizeOfEnd.left()-tmpFrameMove->sizeOfBegin.left(),tmpFrameMove->sizeOfEnd.top()-tmpFrameMove->sizeOfBegin.top());
        }
        else
            frame->setCoords(tmpFrameMove->sizeOfEnd.left(),tmpFrameMove->sizeOfEnd.top(),tmpFrameMove->sizeOfEnd.right(),tmpFrameMove->sizeOfEnd.bottom());

        if(frame->isSelected())
            frame->updateResizeHandles();

        needRelayout = needRelayout || ( frame->runAround() != KWFrame::RA_NO );
    }
    if ( doc )
    {
        doc->updateAllFrames();
        if ( needRelayout )
            doc->layout();
        doc->repaintAllViews();
    }
}

void KWFrameMoveCommand::unexecute()
{
    bool needRelayout = false;
    FrameIndex *tmp;
    KWDocument * doc = 0L;
    for ( tmp=m_indexFrame.first(); tmp != 0; tmp=m_indexFrame.next() )
    {
        KWFrameSet *frameSet =tmp->m_pFrameSet;
        doc = frameSet->kWordDocument();
        KWFrame *frame=frameSet->getFrame(tmp->m_iFrameIndex);
        FrameResizeStruct *tmpFrameMove=m_frameMove.at(m_indexFrame.find(tmp));
        KWTableFrameSet *table=frameSet->getGroupManager();
        if(table)
        {
            table->moveBy(tmpFrameMove->sizeOfBegin.left()-tmpFrameMove->sizeOfEnd.left(),tmpFrameMove->sizeOfBegin.top()-tmpFrameMove->sizeOfEnd.top());
        }
        else
            frame->setCoords(tmpFrameMove->sizeOfBegin.left(),tmpFrameMove->sizeOfBegin.top(),tmpFrameMove->sizeOfBegin.right(),tmpFrameMove->sizeOfBegin.bottom());

        if(frame->isSelected())
            frame->updateResizeHandles();
        needRelayout = needRelayout || ( frame->runAround() != KWFrame::RA_NO );
    }

    if ( doc )
    {
        doc->updateAllFrames();
        if ( needRelayout )
            doc->layout();
        doc->repaintAllViews();
    }
}



KWFrameSetFloatingCommand::KWFrameSetFloatingCommand( const QString &name, KWFrameSet *frameset, bool floating ) :
    KCommand(name),
    m_pFrameSet( frameset ),
    m_bFloating( floating )
{
}

void KWFrameSetFloatingCommand::execute()
{
    kdDebug() << "KWFrameSetFloatingCommand::execute" << endl;
    if ( m_bFloating )
    {
        // Make frame(set) floating
        m_pFrameSet->setFloating();
        // ## We might want to store a list of anchors in the command, and reuse them
        // in execute/unexecute. Currently setFixed forgets the anchors and setFloating recreates new ones...
    }
    else
    {
        // Make frame(set) non-floating
        m_pFrameSet->setFixed();
    }
    m_pFrameSet->kWordDocument()->updateAllFrames();
    m_pFrameSet->kWordDocument()->repaintAllViews();
}

void KWFrameSetFloatingCommand::unexecute()
{
    kdDebug() << "KWFrameSetFloatingCommand::unexecute" << endl;
    if ( !m_bFloating )
    {
        // Make frame(set) floating again
        m_pFrameSet->setFloating();
    }
    else
    {
        // Make frame(set) non-floating again
        m_pFrameSet->setFixed();
    }
    m_pFrameSet->kWordDocument()->updateAllFrames();
    m_pFrameSet->kWordDocument()->repaintAllViews();
}

KWPageLayoutCommand::KWPageLayoutCommand( const QString &name,KWDocument *_doc,pageLayout &_oldLayout, pageLayout &_newLayout  ) :
    KCommand(name),
    m_pDoc(_doc),
    m_OldLayout(_oldLayout),
    m_NewLayout(_newLayout)
{
}

void KWPageLayoutCommand::execute()
{
    m_pDoc->setPageLayout( m_NewLayout._pgLayout,m_NewLayout._cl, m_NewLayout._hf );
    m_pDoc->updateRuler();
    m_pDoc->updateResizeHandles();
    m_pDoc->repaintAllViews();
}

void KWPageLayoutCommand::unexecute()
{
    m_pDoc->setPageLayout( m_OldLayout._pgLayout,m_OldLayout._cl, m_OldLayout._hf);
    m_pDoc->updateRuler();
    m_pDoc->updateResizeHandles();
    m_pDoc->repaintAllViews();
}


KWDeleteFrameCommand::KWDeleteFrameCommand( const QString &name, KWFrame * frame ):
    KCommand(name),
    m_frameIndex( frame ),
    m_copyFrame( frame->getCopy() )
{
}

void KWDeleteFrameCommand::execute()
{
    KWFrameSet *frameSet = m_frameIndex.m_pFrameSet;
    ASSERT( frameSet );

    KWFrame *frame = frameSet->getFrame( m_frameIndex.m_iFrameIndex );
    ASSERT( frame );

    frameSet->delFrame( m_frameIndex.m_iFrameIndex );
    //when you delete a frame frame pointer is deleted
    //so used frameChanged with a null pointer.
    frameSet->kWordDocument()->frameChanged( 0L );
    frameSet->kWordDocument()->refreshDocStructure( frameSet->type() );
}

void KWDeleteFrameCommand::unexecute()
{
    KWFrameSet *frameSet = m_frameIndex.m_pFrameSet;
    KWFrame * frame = m_copyFrame->getCopy();
    frame->setFrameSet( frameSet );
    frameSet->addFrame( frame );

    KWTextFrameSet * textfs = dynamic_cast<KWTextFrameSet *>( frameSet );
    if ( textfs )
        textfs->formatMore();

    frameSet->kWordDocument()->frameChanged( frame );
    frameSet->kWordDocument()->refreshDocStructure(frameSet->type());
}

KWCreateFrameCommand::KWCreateFrameCommand( const QString &name, KWFrame * frame ) :
    KWDeleteFrameCommand( name, frame )
{}


KWUngroupTableCommand::KWUngroupTableCommand( const QString &name, KWTableFrameSet * _table ):
    KCommand(name),
    m_pTable(_table)
{
    m_ListFrame.clear();
    for ( unsigned int i = 0; i < m_pTable->getNumCells(); i++ ) {
        m_ListFrame.append(m_pTable->getCell( i ));
    }
}

void KWUngroupTableCommand::execute()
{
    KWDocument * doc = m_pTable->kWordDocument();
    for ( unsigned int i = 0; i < m_pTable->getNumCells(); i++ ) {
        m_pTable->getCell( i )->setGroupManager( 0L );
        doc->addFrameSet(m_pTable->getCell( i ));
    }
    m_pTable->ungroup();
    doc->removeFrameSet(m_pTable);

    //when you ungroup a table
    // you must remove table item in docstruct
    // create items in text item in docstruct

    int refresh=0;
    refresh |=Tables;
    refresh |=TextFrames;
    doc->refreshDocStructure(refresh);

    doc->updateAllFrames();
    doc->repaintAllViews();
}

void KWUngroupTableCommand::unexecute()
{
    ASSERT(m_pTable);
    m_pTable->group();
    KWDocument * doc = m_pTable->kWordDocument();
    KWFrameSet *tmp;
    for ( tmp=m_ListFrame.first(); tmp != 0; tmp=m_ListFrame.next() )
    {
        tmp->setGroupManager(m_pTable);
        doc->removeFrameSet(tmp);
        KWTableFrameSet::Cell *cell=static_cast<KWTableFrameSet::Cell *>(tmp);
        ASSERT(cell);
        m_pTable->addCell( cell );
    }
    doc->addFrameSet(m_pTable);

    int refresh=0;
    refresh |=Tables;
    refresh |=TextFrames;
    doc->refreshDocStructure(refresh);


    doc->updateAllFrames();
    doc->repaintAllViews();
}


KWDeleteTableCommand::KWDeleteTableCommand( const QString &name, KWTableFrameSet * _table ):
    KCommand(name),
    m_pTable(_table)
{
    ASSERT(m_pTable);
}

void KWDeleteTableCommand::execute()
{
    kdDebug() << "KWDeleteTableCommand::execute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    doc->removeFrameSet(m_pTable);
    doc->refreshDocStructure((int)Tables);
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}

void KWDeleteTableCommand::unexecute()
{
    kdDebug() << "KWDeleteTableCommand::unexecute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    doc->addFrameSet(m_pTable);
    doc->refreshDocStructure((int)Tables);
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}


KWInsertColumnCommand::KWInsertColumnCommand( const QString &name, KWTableFrameSet * _table, int _col ):
    KCommand(name),
    m_pTable(_table),
    m_colPos(_col)
{
    ASSERT(m_pTable);
    m_ListFrameSet.clear();
}

void KWInsertColumnCommand::execute()
{
    kdDebug() << "KWInsertColumnCommand::execute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    m_pTable->insertCol( m_colPos,m_ListFrameSet);
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}

void KWInsertColumnCommand::unexecute()
{
    kdDebug() << "KWInsertColumnCommand::unexecute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    if(m_ListFrameSet.isEmpty())
    {
        for ( unsigned int i = 0; i < m_pTable->getNumCells(); i++ ) {
            KWTableFrameSet::Cell *cell=static_cast<KWTableFrameSet::Cell *>(m_pTable->getCell( i ));
            if(cell->m_col==m_colPos)
                m_ListFrameSet.append(cell);
        }
    }
    doc->terminateEditing(m_pTable);
    doc->frameSelectedChanged();
    m_pTable->deleteCol( m_colPos);
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}



KWInsertRowCommand::KWInsertRowCommand( const QString &name, KWTableFrameSet * _table, int _row ):
    KCommand(name),
    m_pTable(_table),
    m_rowPos(_row)
{
    ASSERT(m_pTable);
    m_ListFrameSet.clear();
}

void KWInsertRowCommand::execute()
{
    kdDebug() << "KWInsertRowCommand::execute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    m_pTable->insertRow( m_rowPos,m_ListFrameSet);
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}

void KWInsertRowCommand::unexecute()
{
    kdDebug() << "KWInsertRowCommand::unexecute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    if(m_ListFrameSet.isEmpty())
    {
        for ( unsigned int i = 0; i < m_pTable->getNumCells(); i++ ) {
            KWTableFrameSet::Cell *cell=static_cast<KWTableFrameSet::Cell *>(m_pTable->getCell( i ));
            if(cell->m_row==m_rowPos)
                m_ListFrameSet.append(cell);
        }
    }
    doc->terminateEditing(m_pTable);
    m_pTable->deleteRow( m_rowPos);
    doc->frameSelectedChanged();
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}




KWRemoveRowCommand::KWRemoveRowCommand( const QString &name, KWTableFrameSet * _table, int _row ):
    KCommand(name),
    m_pTable(_table),
    m_rowPos(_row)
{
    ASSERT(m_pTable);
}

void KWRemoveRowCommand::execute()
{
    kdDebug() << "KWRemoveRowCommand::execute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    doc->terminateEditing(m_pTable);

    m_ListFrameSet.clear();
    m_copyFrame.clear();
    for ( unsigned int i = 0; i < m_pTable->getNumCells(); i++ )
    {
        KWTableFrameSet::Cell *cell=static_cast<KWTableFrameSet::Cell *>(m_pTable->getCell( i ));
        if(cell->m_row==m_rowPos)
        {
            m_ListFrameSet.append(cell);
            m_copyFrame.append(cell->getFrame(0)->getCopy());
        }
    }

    m_pTable->deleteRow( m_rowPos);
    doc->frameSelectedChanged();
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}

void KWRemoveRowCommand::unexecute()
{
    kdDebug() << "KWRemoveRowCommand::unexecute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    m_pTable->insertRow( m_rowPos,m_ListFrameSet,m_copyFrame);
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}


KWRemoveColumnCommand::KWRemoveColumnCommand( const QString &name, KWTableFrameSet * _table, int _col ):
    KCommand(name),
    m_pTable(_table),
    m_colPos(_col)
{
    ASSERT(m_pTable);
}

void KWRemoveColumnCommand::execute()
{
    kdDebug() << "KWRemoveColumnCommand::execute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    doc->terminateEditing(m_pTable);

    m_ListFrameSet.clear();
    m_copyFrame.clear();
    for ( unsigned int i = 0; i < m_pTable->getNumCells(); i++ )
    {
        KWTableFrameSet::Cell *cell=static_cast<KWTableFrameSet::Cell *>(m_pTable->getCell( i ));
        if(cell->m_col==m_colPos)
        {
            m_ListFrameSet.append(cell);
            m_copyFrame.append(cell->getFrame(0)->getCopy());
        }
    }

    m_pTable->deleteCol( m_colPos);
    doc->frameSelectedChanged();
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}

void KWRemoveColumnCommand::unexecute()
{
    kdDebug() << "KWRemoveColumnCommand::unexecute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    m_pTable->insertCol( m_colPos,m_ListFrameSet,m_copyFrame);
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}
