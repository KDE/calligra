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
#include "kwtextframeset.h"
#include "kwview.h"
#include "kwcommand.h"
#include "kwtablestyle.h"
#include "kwtabletemplate.h"
#include "kwtableframeset.h"
#include "kwanchor.h"
#include "kwvariable.h"
#include "koVariable.h"
#include <kotextobject.h>

#include <kdebug.h>


KWPasteTextCommand::KWPasteTextCommand( KoTextDocument *d, int parag, int idx,
                                const QCString & data )
    : KoTextDocCommand( d ), m_parag( parag ), m_idx( idx ), m_data( data )
{
}

KoTextCursor * KWPasteTextCommand::execute( KoTextCursor *c )
{
    KoTextParag *firstParag = doc->paragAt( m_parag );
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
    kdDebug() << "KWPasteTextCommand::execute Inserting text: '" << text << "'" << endl;
    KWTextDocument * textdoc = static_cast<KWTextDocument *>(c->parag()->document());
    KWTextFrameSet * textFs = textdoc->textFrameSet();
    textFs->unzoom();

    cursor.insert( text, true );

    // Move cursor to the end
    c->setParag( firstParag );
    c->setIndex( m_idx );
    for ( int i = 0; i < (int)text.length(); ++i )
        c->gotoNextLetter();

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
                    KoTextFormat f = parag->loadFormat( formatElem, 0L, QFont() );
                    KoTextFormat * defaultFormat = doc->formatCollection()->format( &f );
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
    textFs->zoom( false );
    // In case loadFormatting queued any image request
    KWDocument * doc = textFs->kWordDocument();
    doc->processPictureRequests();

    kdDebug() << "KWPasteTextCommand::execute calling doc->pasteFrames" << endl;
    // In case of any inline frameset
    doc->pasteFrames( elem, 0,true /*don't change footnote attribute*/ );

    m_lastParag = c->parag()->paragId();
    m_lastIndex = c->index();
    return c;
}

// Helper class for deleting all custom items
// (KWTextFrameset::removeSelectedText and readFormats do that already,
//  but with undo/redo, and copying all formatting etc.)
class KWDeleteCustomItemVisitor : public KoParagVisitor // see kotextdocument.h
{
public:
    KWDeleteCustomItemVisitor() : KoParagVisitor() { }
    virtual bool visit( KoTextParag *parag, int start, int end )
    {
        kdDebug() << "KWPasteTextCommand::execute " << parag->paragId() << " " << start << " " << end << endl;
        for ( int i = start ; i < end ; ++i )
        {
            KoTextStringChar * ch = parag->at( i );
            if ( ch->isCustom() )
	    {
	       KoTextCustomItem* item = ch->customItem();
	       item->setDeleted( true );
            parag->removeCustomItem(i);
	       KCommand* itemCmd = item->deleteCommand();
	       if ( itemCmd ) itemCmd->execute();
	    }
        }
        return true;
    }
};

KoTextCursor * KWPasteTextCommand::unexecute( KoTextCursor *c )
{
    KoTextParag *firstParag = doc->paragAt( m_parag );
    if ( !firstParag ) {
        qWarning( "can't locate parag at %d, last parag: %d", m_parag, doc->lastParag()->paragId() );
        return 0;
    }
    cursor.setParag( firstParag );
    cursor.setIndex( m_idx );
    doc->setSelectionStart( KoTextDocument::Temp, &cursor );

    KoTextParag *lastParag = doc->paragAt( m_lastParag );
    if ( !lastParag ) {
        qWarning( "can't locate parag at %d, last parag: %d", m_lastParag, doc->lastParag()->paragId() );
        return 0;
    }
    cursor.setParag( lastParag );
    cursor.setIndex( m_lastIndex );
    doc->setSelectionEnd( KoTextDocument::Temp, &cursor );
    // Delete all custom items
    KWDeleteCustomItemVisitor visitor;
    doc->visitSelection( KoTextDocument::Temp, &visitor );

    doc->removeSelectedText( KoTextDocument::Temp, c /* sets c to the correct position */ );

    KWTextDocument * textdoc = static_cast<KWTextDocument *>(lastParag->document());
    KWTextFrameSet * textFs = textdoc->textFrameSet();

    textFs->renumberFootNotes();
    if ( m_idx == 0 )
        firstParag->setParagLayout( m_oldParagLayout );
    return c;
}


KWTextDeleteCommand::KWTextDeleteCommand( KoTextDocument *d, int i, int idx, const QMemArray<KoTextStringChar> &str,
                         const CustomItemsMap & customItemsMap,
                         const QValueList<KoParagLayout> & oldParagLayouts )
    :KoTextDeleteCommand(d, i, idx, str, customItemsMap, oldParagLayouts)
{
}

KoTextCursor *KWTextDeleteCommand::execute( KoTextCursor *c )
{
    return KoTextDeleteCommand::execute( c );
}

KoTextCursor *KWTextDeleteCommand::unexecute( KoTextCursor *c )
{
    return KoTextDeleteCommand::unexecute( c );
}

////////////////////////// Frame commands ////////////////////////////////

FrameIndex::FrameIndex( KWFrame *frame )
{
    m_pFrameSet=frame->frameSet();
    m_iFrameIndex=m_pFrameSet->frameFromPtr(frame);
}

KWFrameBorderCommand::KWFrameBorderCommand( const QString &name, QPtrList<FrameIndex> &_listFrameIndex, QPtrList<FrameBorderTypeStruct> &_frameTypeBorder,const KoBorder & _newBorder):
    KNamedCommand(name),
    m_indexFrame(_listFrameIndex),
    m_oldBorderFrameType(_frameTypeBorder),
    m_newBorder( _newBorder)
{
}

KWFrameBorderCommand::~KWFrameBorderCommand()
{
    m_indexFrame.setAutoDelete( true);
    m_oldBorderFrameType.setAutoDelete( true);
}

void KWFrameBorderCommand::execute()
{
    FrameIndex *tmp;
    KWDocument *doc = 0L;
    for ( tmp=m_indexFrame.first(); tmp != 0; tmp=m_indexFrame.next() )
    {
        KWFrameSet *frameSet =tmp->m_pFrameSet;
        doc = frameSet->kWordDocument();
        KWFrame *frame=frameSet->frame(tmp->m_iFrameIndex);
        KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell *>(frame->frameSet());
        FrameBorderTypeStruct *tmpFrameStruct=m_oldBorderFrameType.at(m_indexFrame.find(tmp));

        switch( tmpFrameStruct->m_EFrameType)
        {
            case  FBLeft:
                if(cell!=0L) // is a table cell
                    cell->setLeftBorder(m_newBorder);
                else
                    frame->setLeftBorder(m_newBorder);
                break;
            case FBRight:
                if(cell!=0L) // is a table cell
                    cell->setRightBorder(m_newBorder);
                else
                     frame->setRightBorder(m_newBorder);
                break;
            case FBTop:
                if(cell!=0L) // is a table cell
                    cell->setTopBorder(m_newBorder);
                else
                      frame->setTopBorder(m_newBorder);
                break;
            case FBBottom:
                if(cell!=0L) // is a table cell
                    cell->setBottomBorder(m_newBorder);
                else
                    frame->setBottomBorder(m_newBorder);
                break;
            default:
                break;
        }

	if (cell==0L) {
		frame->frameBordersChanged();
		if (frame->isSelected())
			frame->updateResizeHandles();
		    //fixme frameBorderChanged for table cells here too ?
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
        KWFrame *frame=frameSet->frame(tmp->m_iFrameIndex);
        KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell *>(frame->frameSet());
        FrameBorderTypeStruct *tmpFrameStruct=m_oldBorderFrameType.at(m_indexFrame.find(tmp));
        switch(tmpFrameStruct->m_EFrameType)
        {
            case  FBLeft:
                if(cell!=0L) // is a table cell
                    cell->setLeftBorder(tmpFrameStruct->m_OldBorder);
                else
                    frame->setLeftBorder(tmpFrameStruct->m_OldBorder);
                break;
            case FBRight:
                if(cell!=0L) // is a table cell
                    cell->setRightBorder(tmpFrameStruct->m_OldBorder);
                else
                    frame->setRightBorder(tmpFrameStruct->m_OldBorder);
                break;
            case FBTop:
                if(cell!=0L) // is a table cell
                    cell->setTopBorder(tmpFrameStruct->m_OldBorder);
                else
                    frame->setTopBorder(tmpFrameStruct->m_OldBorder);
                break;
            case FBBottom:
                if(cell!=0L) // is a table cell
                    cell->setBottomBorder(tmpFrameStruct->m_OldBorder);
                else
                    frame->setBottomBorder(tmpFrameStruct->m_OldBorder);
                break;
            default:
                break;
        }
	if (cell==0L) {
		frame->frameBordersChanged();
		if (frame->isSelected())
			frame->updateResizeHandles();
		    //fixme frameBorderChanged for table cells here too ?
	}
    }



    if ( doc )
    {
        doc->refreshFrameBorderButton();
        //update frames
        doc->repaintAllViews();
    }
}

KWFrameBackGroundColorCommand::KWFrameBackGroundColorCommand( const QString &name, QPtrList<FrameIndex> &_listFrameIndex, QPtrList<QBrush> &_oldBrush,const QBrush & _newColor ):
    KNamedCommand(name),
    m_indexFrame(_listFrameIndex),
    m_oldBackGroundColor(_oldBrush),
    m_newColor( _newColor)
{
}

KWFrameBackGroundColorCommand::~KWFrameBackGroundColorCommand()
{
    m_indexFrame.setAutoDelete(true);
    m_oldBackGroundColor.setAutoDelete(true);
}


void KWFrameBackGroundColorCommand::execute()
{
    FrameIndex *tmp;
    KWDocument * doc = 0L;
    for ( tmp=m_indexFrame.first(); tmp != 0; tmp=m_indexFrame.next() )
    {
        KWFrameSet *frameSet =tmp->m_pFrameSet;
        doc = frameSet->kWordDocument();
        KWFrame *frame=frameSet->frame(tmp->m_iFrameIndex);
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
        KWFrame *frame=frameSet->frame(tmp->m_iFrameIndex);
        QBrush *tmpFrameStruct=m_oldBackGroundColor.at(m_indexFrame.find(tmp));
        frame->setBackgroundColor(*tmpFrameStruct);
    }

    //update frames
    if ( doc )
        doc->repaintAllViews();
}

KWFrameStyleCommand::KWFrameStyleCommand( const QString &name, KWFrame *_frame, KWFrameStyle *_fs, bool _repaintViews ) :
    KNamedCommand( name )
{
    m_frame = _frame;
    m_fs = _fs;
    repaintViews = _repaintViews;

    m_oldValues = new KWFrameStyle( "Old", m_frame );
}

void KWFrameStyleCommand::execute()
{
    m_frame->setBackgroundColor( m_fs->backgroundColor() );
    m_frame->setLeftBorder( m_fs->leftBorder() );
    m_frame->setRightBorder( m_fs->rightBorder() );
    m_frame->setTopBorder( m_fs->topBorder() );
    m_frame->setBottomBorder( m_fs->bottomBorder() );

    if ( repaintViews )
        m_frame->frameSet()->kWordDocument()->repaintAllViews();
    m_frame->frameSet()->kWordDocument()->refreshFrameBorderButton();
}

void KWFrameStyleCommand::unexecute()
{
    m_frame->setBackgroundColor( m_oldValues->backgroundColor() );
    m_frame->setLeftBorder( m_oldValues->leftBorder() );
    m_frame->setRightBorder( m_oldValues->rightBorder() );
    m_frame->setTopBorder( m_oldValues->topBorder() );
    m_frame->setBottomBorder( m_oldValues->bottomBorder() );

    if ( repaintViews )
        m_frame->frameSet()->kWordDocument()->repaintAllViews();
    m_frame->frameSet()->kWordDocument()->refreshFrameBorderButton();
}

KWTableStyleCommand::KWTableStyleCommand( const QString &name, KWFrame *_frame, KWTableStyle *_ts, bool _repaintViews ) :
    KNamedCommand( name )
{
    m_frame = _frame;
    m_ts = _ts;
    repaintViews = _repaintViews;

    // No need for i18n because it will never be displayed.
    m_fsc = new KWFrameStyleCommand( "Apply framestyle to frame", m_frame, m_ts->pFrameStyle(), repaintViews );
    m_sc = 0L;
}

KWTableStyleCommand::~KWTableStyleCommand()
{
    delete m_fsc;
    delete m_sc;
}

void KWTableStyleCommand::execute()
{
    if (m_fsc)
        m_fsc->execute();

    if ( (m_ts) && ( m_frame->frameSet()->type() == FT_TEXT ) && ( m_ts->pStyle() ) )
    {
        KoTextObject *textObject = ((KWTextFrameSet*)m_frame->frameSet())->textObject();
        textObject->textDocument()->selectAll( KoTextDocument::Temp );
        m_sc = textObject->applyStyle( 0L, m_ts->pStyle(), KoTextDocument::Temp, KoParagLayout::All, KoTextFormat::Format, true, false );
        textObject->textDocument()->removeSelection( KoTextDocument::Temp );
    }

    if ( repaintViews )
        m_frame->frameSet()->kWordDocument()->repaintAllViews();
    m_frame->frameSet()->kWordDocument()->refreshFrameBorderButton();

}

void KWTableStyleCommand::unexecute()
{
    if (m_fsc)
        m_fsc->unexecute();
    if (m_sc)
        m_sc->unexecute();

    if ( repaintViews )
        m_frame->frameSet()->kWordDocument()->repaintAllViews();
    m_frame->frameSet()->kWordDocument()->refreshFrameBorderButton();
}

KWTableTemplateCommand::KWTableTemplateCommand( const QString &name, KWTableFrameSet *_table, KWTableTemplate *_tt ) :
    KNamedCommand( name )
{
    m_table = _table;
    m_tt = _tt;

    // No need for i18n because it will never be displayed.
    m_tableCommands = new KMacroCommand( "Apply tablestyles to table" );


    KWTableStyle *cell = 0L;
    unsigned int rows = m_table->getRows();
    unsigned int cols = m_table->getCols();

    for ( unsigned int i = 0; i < rows; i++ )
    {
        for ( unsigned int j = 0; j < cols; j++ )
        {
            if ( (i==0) && (j==0) ) // TOP LEFT CORNER
                cell = m_tt->pTopLeftCorner();
            else
            if ( (i==0) && ( j==(cols-1) ) ) // TOP RIGHT CORNER
                cell = m_tt->pTopRightCorner();
            else
            if ( ( i==(rows-1) ) && (j==0) ) // BOTTOM LEFT CORNER
                cell = m_tt->pBottomLeftCorner();
            else
            if ( ( i==(rows-1) ) && ( j==(cols-1) ) ) // BOTTOM RIGHT CORNER
                cell = m_tt->pBottomRightCorner();
            else
            if ( ( i==0 ) && ( j>0 ) && ( j<(cols-1) ) ) // FIRST ROW
                cell = m_tt->pFirstRow();
            else
            if ( ( j==0 ) && ( i>0 ) && ( i<(rows-1) ) ) // FIRST COL
                cell = m_tt->pFirstCol();
            else
            if ( ( i==(rows-1) ) && ( j>0 ) && ( j<(cols-1) ) )  // LAST ROW
                cell = m_tt->pLastRow();
            else
            if ( ( j==(cols-1) ) && ( i>0 ) && ( i<(rows-1) ) ) // LAST COL
                cell = m_tt->pLastCol();
            else
            if ( (i>0) && (j>0) && (i<(rows-1)) && (j<(cols-1)) ) // BODY
                cell = m_tt->pBodyCell();

            m_tableCommands->addCommand( new KWTableStyleCommand( "Apply tablestyle to cell", m_table->getCell(i,j)->frame(0),cell, false ) );
        }
    }
}

KWTableTemplateCommand::~KWTableTemplateCommand()
{
    if (m_tableCommands)
        delete m_tableCommands;
}

void KWTableTemplateCommand::execute()
{
    m_tableCommands->execute();
    m_table->kWordDocument()->repaintAllViews();
}

void KWTableTemplateCommand::unexecute()
{
    m_tableCommands->unexecute();
    m_table->kWordDocument()->repaintAllViews();
}


KWFrameResizeCommand::KWFrameResizeCommand( const QString &name, FrameIndex _frameIndex, FrameResizeStruct _frameResize ) :
    KNamedCommand(name),
    m_indexFrame(_frameIndex),
    m_FrameResize(_frameResize)
{
}

void KWFrameResizeCommand::execute()
{
    KWFrameSet *frameSet = m_indexFrame.m_pFrameSet;
    Q_ASSERT( frameSet );
    KWFrame *frame = frameSet->frame(m_indexFrame.m_iFrameIndex);
    Q_ASSERT( frame );
    frame->setCoords(m_FrameResize.sizeOfEnd.left(),m_FrameResize.sizeOfEnd.top(),m_FrameResize.sizeOfEnd.right(),m_FrameResize.sizeOfEnd.bottom());

    KWTableFrameSet *table = frame->frameSet()->getGroupManager();
    if (table) {
        KWTableFrameSet::Cell *cell=dynamic_cast<KWTableFrameSet::Cell *>(frame->frameSet());
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
        table->refreshSelectedCell();
        //repaintTableHeaders( table );
    }
    KWDocument * doc = frameSet->kWordDocument();
    if ( frameSet->frameSetInfo() != KWFrameSet::FI_BODY ) // header/footer/footnote
        doc->recalcFrames();

    frame->updateRulerHandles();

    //update frames
    doc->frameChanged( frame );
}

void KWFrameResizeCommand::unexecute()
{
    KWFrameSet *frameSet =m_indexFrame.m_pFrameSet;
    KWFrame *frame=frameSet->frame(m_indexFrame.m_iFrameIndex);
    frame->setCoords(m_FrameResize.sizeOfBegin.left(),m_FrameResize.sizeOfBegin.top(),m_FrameResize.sizeOfBegin.right(),m_FrameResize.sizeOfBegin.bottom());
    KWTableFrameSet *table = frame->frameSet()->getGroupManager();
    if (table) {
        KWTableFrameSet::Cell *cell=dynamic_cast<KWTableFrameSet::Cell *>(frame->frameSet());
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
        table->refreshSelectedCell();
        //repaintTableHeaders( table );
    }
    KWDocument * doc = frameSet->kWordDocument();
    if ( frameSet->frameSetInfo() != KWFrameSet::FI_BODY ) // header/footer/footnote
        doc->recalcFrames();

    frame->updateRulerHandles();

    //update frames
    doc->frameChanged( frame );
}

KWFrameChangePictureClipartCommand::KWFrameChangePictureClipartCommand( const QString &name, FrameIndex _frameIndex, const QString & _oldFile, const QString &_newFile, bool _isAPicture ) :
    KNamedCommand(name),
    m_indexFrame(_frameIndex),
    m_oldFile(_oldFile),
    m_newFile(_newFile),
    m_isAPicture(_isAPicture)
{
}

void KWFrameChangePictureClipartCommand::execute()
{
    KWFrameSet *frameSet = m_indexFrame.m_pFrameSet;
    Q_ASSERT( frameSet );
    KWFrame *frame = frameSet->frame(m_indexFrame.m_iFrameIndex);
    Q_ASSERT( frame );
    KWDocument * doc = frameSet->kWordDocument();
    if(m_isAPicture)
    {
        KWPictureFrameSet *frameset = static_cast<KWPictureFrameSet *>(frame->frameSet());
        frameset->loadPicture( m_newFile , doc->zoomRect( *frame ).size() );
    }
    else
    {
        KWPictureFrameSet *frameset = static_cast<KWPictureFrameSet *>(frame->frameSet());
        frameset->loadPicture( m_newFile );
    }
    doc->frameChanged( frame );
}

void KWFrameChangePictureClipartCommand::unexecute()
{
    KWFrameSet *frameSet =m_indexFrame.m_pFrameSet;
    KWFrame *frame=frameSet->frame(m_indexFrame.m_iFrameIndex);
    KWDocument * doc = frameSet->kWordDocument();
    if(m_isAPicture)
    {
        KWPictureFrameSet *frameset = static_cast<KWPictureFrameSet *>(frame->frameSet());
        frameset->loadPicture( m_oldFile , doc->zoomRect( *frame ).size() );
    }
    else
    {
        KWPictureFrameSet *frameset = static_cast<KWPictureFrameSet *>(frame->frameSet());
        frameset->loadPicture( m_oldFile );
    }


    //update frames
    doc->frameChanged( frame );
}


KWFramePartMoveCommand::KWFramePartMoveCommand( const QString &name, FrameIndex _frameIndex,  FrameResizeStruct _frameMove ) :
    KNamedCommand(name),
    m_indexFrame(_frameIndex),
    m_frameMove(_frameMove)
{
}

void KWFramePartMoveCommand::execute()
{
    KWFrameSet *frameSet = m_indexFrame.m_pFrameSet;
    Q_ASSERT( frameSet );
    KWFrame *frame = frameSet->frame(m_indexFrame.m_iFrameIndex);
    Q_ASSERT( frame );
    frame->setCoords(m_frameMove.sizeOfEnd.left(),m_frameMove.sizeOfEnd.top(),m_frameMove.sizeOfEnd.right(),m_frameMove.sizeOfEnd.bottom());

    KWDocument * doc = frameSet->kWordDocument();

    frame->updateRulerHandles();
    doc->frameChanged( frame );
}

void KWFramePartMoveCommand::unexecute()
{
    KWFrameSet *frameSet =m_indexFrame.m_pFrameSet;
    KWFrame *frame=frameSet->frame(m_indexFrame.m_iFrameIndex);
    frame->setCoords(m_frameMove.sizeOfBegin.left(),m_frameMove.sizeOfBegin.top(),m_frameMove.sizeOfBegin.right(),m_frameMove.sizeOfBegin.bottom());

    KWDocument * doc = frameSet->kWordDocument();
    frame->updateRulerHandles();

    //update frames
    doc->frameChanged( frame );
}

bool KWFramePartMoveCommand::frameMoved()
{
    return  (m_frameMove.sizeOfBegin!=m_frameMove.sizeOfEnd);
}

KWFrameMoveCommand::KWFrameMoveCommand( const QString &name, QPtrList<FrameIndex> &_frameIndex, QPtrList<FrameResizeStruct>&_frameMove  ) :
    KNamedCommand(name),
    m_indexFrame(_frameIndex),
    m_frameMove(_frameMove)
{
    // Brrr... why don't we use QValueList rather ?
    m_indexFrame.setAutoDelete(true);
    m_frameMove.setAutoDelete(true);
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
        KWFrame *frame=frameSet->frame(tmp->m_iFrameIndex);
        FrameResizeStruct *tmpFrameMove=m_frameMove.at(m_indexFrame.find(tmp));
        KWTableFrameSet *table=frameSet->getGroupManager();
        if(table)
        {
            table->moveBy(tmpFrameMove->sizeOfEnd.left()-tmpFrameMove->sizeOfBegin.left(),tmpFrameMove->sizeOfEnd.top()-tmpFrameMove->sizeOfBegin.top());
        }
        else
            frame->setCoords(tmpFrameMove->sizeOfEnd.left(),tmpFrameMove->sizeOfEnd.top(),tmpFrameMove->sizeOfEnd.right(),tmpFrameMove->sizeOfEnd.bottom());

        frame->updateRulerHandles();
        needRelayout = needRelayout || ( frame->runAround() != KWFrame::RA_NO );
    }
    if ( doc )
    {
        doc->updateAllFrames();
        if ( needRelayout )
            doc->layout();

        doc->updateRulerFrameStartEnd();
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
        KWFrame *frame=frameSet->frame(tmp->m_iFrameIndex);
        FrameResizeStruct *tmpFrameMove=m_frameMove.at(m_indexFrame.find(tmp));
        KWTableFrameSet *table=frameSet->getGroupManager();
        if(table)
        {
            table->moveBy(tmpFrameMove->sizeOfBegin.left()-tmpFrameMove->sizeOfEnd.left(),tmpFrameMove->sizeOfBegin.top()-tmpFrameMove->sizeOfEnd.top());
        }
        else
            frame->setCoords(tmpFrameMove->sizeOfBegin.left(),tmpFrameMove->sizeOfBegin.top(),tmpFrameMove->sizeOfBegin.right(),tmpFrameMove->sizeOfBegin.bottom());

        frame->updateRulerHandles();
        needRelayout = needRelayout || ( frame->runAround() != KWFrame::RA_NO );
    }

    if ( doc )
    {
        doc->updateAllFrames();
        if ( needRelayout )
            doc->layout();
        doc->updateRulerFrameStartEnd();
        doc->repaintAllViews();
    }
}

KWFramePropertiesCommand::KWFramePropertiesCommand( const QString &name, KWFrame *_frameBefore,  KWFrame *_frameAfter ) :
    KNamedCommand(name),
    m_frameIndex( _frameAfter ),
    m_frameBefore(_frameBefore),
    m_frameAfter(_frameAfter->getCopy())
{
}

KWFramePropertiesCommand::~KWFramePropertiesCommand()
{
    delete m_frameBefore;
    delete m_frameAfter;
}

void KWFramePropertiesCommand::execute()
{
    kdDebug() << "KWFrameChangeParamCommand::execute" << endl;
    KWFrameSet *frameSet = m_frameIndex.m_pFrameSet;
    Q_ASSERT( frameSet );

    KWFrame *frame = frameSet->frame( m_frameIndex.m_iFrameIndex );
    Q_ASSERT( frame );
    frame->copySettings(m_frameAfter);

    KWDocument * doc = frameSet->kWordDocument();
    if(doc)
    {
        doc->frameChanged( frame );
        doc->updateAllFrames();
        doc->layout();
        doc->repaintAllViews();
        doc->updateRulerFrameStartEnd();
        doc->updateResizeHandles();
    }
}

void KWFramePropertiesCommand::unexecute()
{
    kdDebug() << "KWFrameChangeParamCommand::unexecute" << endl;
    KWFrameSet *frameSet = m_frameIndex.m_pFrameSet;
    Q_ASSERT( frameSet );

    KWFrame *frame = frameSet->frame( m_frameIndex.m_iFrameIndex );
    Q_ASSERT( frame );
    frame->copySettings(m_frameBefore);
    KWDocument * doc = frameSet->kWordDocument();
    if(doc)
    {
        doc->frameChanged( frame );
        doc->updateAllFrames();
        doc->layout();
        doc->repaintAllViews();
        doc->updateRulerFrameStartEnd();
        doc->updateResizeHandles();
    }
}


KWFrameSetPropertyCommand::KWFrameSetPropertyCommand( const QString &name, KWFrameSet *frameset, Property prop, const QString& value ) :
    KNamedCommand(name),
    m_pFrameSet( frameset ),
    m_property( prop ),
    m_value( value )
{
    switch ( m_property ) {
    case FSP_NAME:
        m_oldValue = m_pFrameSet->getName();
        break;
    case FSP_FLOATING:
        m_oldValue = m_pFrameSet->isFloating() ? "true" : "false";
        break;
    case FSP_KEEPASPECTRATION:
        m_oldValue = static_cast<KWPictureFrameSet*>(m_pFrameSet)->keepAspectRatio() ? "keepRatio" : "dontKeepRatio";
        break;
    case FSP_PROTECTSIZE:
        m_oldValue = m_pFrameSet->isProtectSize() ? "true" : "false";
        break;
    }
}

void KWFrameSetPropertyCommand::setValue( const QString &value )
{
    kdDebug() << "KWFrameSetPropertyCommand::execute" << endl;
    switch ( m_property ) {
    case FSP_NAME:
        m_pFrameSet->setName( value );
        break;
    case FSP_FLOATING:
        if ( value == "true" )
        {
            // Make frame(set) floating
            m_pFrameSet->setFloating();
            // ## We might want to store a list of anchors in the command, and reuse them
            // in execute/unexecute. Currently setFixed forgets the anchors and setFloating recreates new ones...
        }
        else if(value == "false")
        {
            // Make frame(set) non-floating
            m_pFrameSet->setFixed();
        }
    case FSP_KEEPASPECTRATION:
        if( value == "keepRatio")
        {
            KWPictureFrameSet * frameSet=dynamic_cast<KWPictureFrameSet*>(m_pFrameSet);
            if(frameSet)
                frameSet->setKeepAspectRatio( true );
        }
        else if( value=="dontKeepRatio")
        {
            KWPictureFrameSet * frameSet=dynamic_cast<KWPictureFrameSet*>(m_pFrameSet);
            if(frameSet)
                frameSet->setKeepAspectRatio( false );
        }
        break;
    case FSP_PROTECTSIZE:
        if( value == "true")
            m_pFrameSet->setProtectSize( true );
        else
            m_pFrameSet->setProtectSize( false );
        m_pFrameSet->kWordDocument()->repaintResizeHandles();
        break;
    }

    m_pFrameSet->kWordDocument()->updateResizeHandles();

    m_pFrameSet->kWordDocument()->updateAllFrames();
    m_pFrameSet->kWordDocument()->repaintAllViews();
    m_pFrameSet->kWordDocument()->updateRulerFrameStartEnd();
}

void KWFrameSetPropertyCommand::execute()
{
    setValue( m_value );
}

void KWFrameSetPropertyCommand::unexecute()
{
    setValue( m_oldValue );
}

KWPageLayoutCommand::KWPageLayoutCommand( const QString &name,KWDocument *_doc,pageLayout &_oldLayout, pageLayout &_newLayout  ) :
    KNamedCommand(name),
    m_pDoc(_doc),
    m_OldLayout(_oldLayout),
    m_NewLayout(_newLayout)
{
}

void KWPageLayoutCommand::execute()
{
    m_pDoc->setPageLayout( m_NewLayout._pgLayout,m_NewLayout._cl, m_NewLayout._hf );
}

void KWPageLayoutCommand::unexecute()
{
    m_pDoc->setPageLayout( m_OldLayout._pgLayout,m_OldLayout._cl, m_OldLayout._hf);
}


KWDeleteFrameCommand::KWDeleteFrameCommand( const QString &name, KWFrame * frame ):
    KNamedCommand(name),
    m_frameIndex( frame ),
    m_copyFrame( frame->getCopy() )
{
}

KWDeleteFrameCommand::~KWDeleteFrameCommand()
{
    delete m_copyFrame;
}


void KWDeleteFrameCommand::execute()
{
    KWFrameSet *frameSet = m_frameIndex.m_pFrameSet;
    Q_ASSERT( frameSet );

    KWFrame *frame = frameSet->frame( m_frameIndex.m_iFrameIndex );
    Q_ASSERT( frame );
    frameSet->kWordDocument()->terminateEditing( frameSet );
    frameSet->delFrame( m_frameIndex.m_iFrameIndex );
    //when you delete a frame frame pointer is deleted
    //so used frameChanged with a null pointer.
    frameSet->kWordDocument()->frameChanged( 0L );
    frameSet->kWordDocument()->refreshDocStructure( frameSet->type() );
    frameSet->kWordDocument()->updateRulerFrameStartEnd();
}

void KWDeleteFrameCommand::unexecute()
{
    KWFrameSet *frameSet = m_frameIndex.m_pFrameSet;
    KWFrame * frame = m_copyFrame->getCopy();
    frame->setFrameSet( frameSet );
    frameSet->addFrame( frame );

    KWTextFrameSet * textfs = dynamic_cast<KWTextFrameSet *>( frameSet );
    if ( textfs )
        textfs->textObject()->formatMore();
    KWDocument* doc = frameSet->kWordDocument();
    doc->frameChanged( frame );
    // could have been the last frame on a page, so undeleting it needs to recreate the page
    doc->recalcFrames( frame->pageNum() );
    doc->refreshDocStructure(frameSet->type());
    doc->updateRulerFrameStartEnd();
}

KWCreateFrameCommand::KWCreateFrameCommand( const QString &name, KWFrame * frame ) :
    KWDeleteFrameCommand( name, frame )
{}


KWUngroupTableCommand::KWUngroupTableCommand( const QString &name, KWTableFrameSet * _table ):
    KNamedCommand(name),
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
    Q_ASSERT(m_pTable);
    m_pTable->group();
    KWDocument * doc = m_pTable->kWordDocument();
    KWFrameSet *tmp;
    for ( tmp=m_ListFrame.first(); tmp != 0; tmp=m_ListFrame.next() )
    {
        tmp->setGroupManager(m_pTable);
        doc->removeFrameSet(tmp);
        KWTableFrameSet::Cell *cell=static_cast<KWTableFrameSet::Cell *>(tmp);
        Q_ASSERT(cell);
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
    KNamedCommand(name),
    m_pTable(_table)
{
    Q_ASSERT(m_pTable);
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
    doc->updateRulerFrameStartEnd();

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
    doc->updateRulerFrameStartEnd();
}


KWInsertColumnCommand::KWInsertColumnCommand( const QString &name, KWTableFrameSet * _table, int _col, double _maxRight ):
    KNamedCommand(name),
    m_pTable(_table),
    m_colPos(_col),
    m_maxRight(_maxRight),
    m_oldWidth(0)
{
    Q_ASSERT(m_pTable);
    m_ListFrameSet.clear();
}

void KWInsertColumnCommand::execute()
{
    kdDebug() << "KWInsertColumnCommand::execute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    // a insert column = KWTableFrameSet::m_sDefaultColWidth, see kwtableframeset.cc
    if (m_pTable->boundingRect().right() + KWTableFrameSet::m_sDefaultColWidth >= static_cast<int>(m_maxRight))
    {   // must create space (resize the table)
        m_oldWidth = m_pTable->boundingRect().width();
        // here we calculate the new table size for a table that would take the
        // entire width of the page, which what the user wants 99% of the time.
        double newTableWidth =m_maxRight - m_pTable->boundingRect().left();
        double newColSize = newTableWidth / (m_pTable->getCols()+1);
        double resizeTableWidth = m_maxRight - m_pTable->boundingRect().left();
        m_pTable->resizeWidth(resizeTableWidth - newColSize);
        m_pTable->insertCol(m_colPos, m_ListFrameSet, QPtrList<KWFrame>(), newColSize);
    }
    else
    {   // simply insert the column without asking for a specific size :
        m_pTable->insertCol(m_colPos, m_ListFrameSet);
    }
    Q_ASSERT(m_pTable->boundingRect().right() <= m_maxRight);
    doc->updateAllFrames();
    doc->layout();
    doc->updateResizeHandles( );
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
    // now undo the resize of the table if necessary:
    if (m_oldWidth) {
        // yes, the table was resized, let's undo that :
        m_pTable->resizeWidth(m_oldWidth);
    }
    doc->updateAllFrames();
    doc->layout();
    doc->updateResizeHandles( );
    doc->repaintAllViews();
}



KWInsertRowCommand::KWInsertRowCommand( const QString &name, KWTableFrameSet * _table, int _row ):
    KNamedCommand(name),
    m_pTable(_table),
    m_rowPos(_row)
{
    Q_ASSERT(m_pTable);
    m_ListFrameSet.clear();
}

void KWInsertRowCommand::execute()
{
    kdDebug() << "KWInsertRowCommand::execute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    m_pTable->insertRow( m_rowPos,m_ListFrameSet);
    doc->updateAllFrames();
    doc->layout();
    doc->updateResizeHandles( );
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
    doc->updateResizeHandles( );
    doc->repaintAllViews();
}




KWRemoveRowCommand::KWRemoveRowCommand( const QString &name, KWTableFrameSet * _table, int _row ):
    KNamedCommand(name),
    m_pTable(_table),
    m_rowPos(_row)
{
    Q_ASSERT(m_pTable);
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
            m_copyFrame.append(cell->frame(0)->getCopy());
        }
    }

    m_pTable->deleteRow( m_rowPos);
    doc->frameSelectedChanged();
    doc->updateAllFrames();
    doc->layout();
    doc->updateResizeHandles( );
    doc->repaintAllViews();
}

void KWRemoveRowCommand::unexecute()
{
    kdDebug() << "KWRemoveRowCommand::unexecute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    m_pTable->insertRow( m_rowPos,m_ListFrameSet,m_copyFrame);
    doc->updateAllFrames();
    doc->layout();
    doc->updateResizeHandles( );
    doc->repaintAllViews();
}


KWRemoveColumnCommand::KWRemoveColumnCommand( const QString &name, KWTableFrameSet * _table, int _col ):
    KNamedCommand(name),
    m_pTable(_table),
    m_colPos(_col)
{
    Q_ASSERT(m_pTable);
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
            m_copyFrame.append(cell->frame(0)->getCopy());
        }
    }

    m_pTable->deleteCol( m_colPos);
    doc->frameSelectedChanged();
    doc->updateAllFrames();
    doc->layout();
    doc->updateResizeHandles( );
    doc->repaintAllViews();
}

void KWRemoveColumnCommand::unexecute()
{
    kdDebug() << "KWRemoveColumnCommand::unexecute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    m_pTable->insertCol( m_colPos,m_ListFrameSet,m_copyFrame);
    doc->updateAllFrames();
    doc->layout();
    doc->updateResizeHandles( );
    doc->repaintAllViews();
}



KWSplitCellCommand::KWSplitCellCommand( const QString &name, KWTableFrameSet * _table,unsigned int colBegin,unsigned int rowBegin, unsigned int colEnd,unsigned int rowEnd ):
    KNamedCommand(name),
    m_pTable(_table),
    m_colBegin(colBegin),
    m_rowBegin(rowBegin),
    m_colEnd(colEnd),
    m_rowEnd(rowEnd)
{
    Q_ASSERT(m_pTable);
}

void KWSplitCellCommand::execute()
{
    kdDebug() << "KWSplitCellCommand::execute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    doc->terminateEditing(m_pTable);
    //kdDebug()<<"split Cell m_colBegin :"<<m_colBegin<<" m_colEnd :"<<m_colEnd<<" m_rowBegin :"<<m_rowBegin<<" m_colEnd :"<<m_colEnd<<endl;
    m_pTable->splitCell(m_rowEnd, m_colEnd,m_colBegin,m_rowBegin,m_ListFrameSet);
    doc->frameSelectedChanged();
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}

void KWSplitCellCommand::unexecute()
{
    kdDebug() << "KWSplitCellCommand::unexecute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    doc->terminateEditing(m_pTable);

    //kdDebug()<<"Join Cell m_colBegin :"<<m_colBegin<<" m_colEnd :"<<m_colBegin+m_colEnd-1<<" m_rowBegin :"<<m_rowBegin<<" m_rowEnd :"<<m_rowBegin+m_rowEnd-1<<endl;

    if(m_ListFrameSet.isEmpty())
    {
        for ( unsigned int i = 0; i < m_pTable->getCols(); i++ )
        {
            for ( unsigned int j = 0; j < m_pTable->getRows(); j++ )
            {
                if(j>=m_rowBegin && j<=(m_rowBegin+m_rowEnd-1)
                   && i>=m_colBegin && i<=(m_colEnd+m_colBegin-1))
                {
                    //don't store first cell
                    if( !(j==m_rowBegin && i==m_colBegin))
                    {
                        kdDebug()<<"store cell row :"<<j<<" col :"<<i<<endl;
                        KWTableFrameSet::Cell *cell=static_cast<KWTableFrameSet::Cell *>(m_pTable->getCell( j,i ));
                        m_ListFrameSet.append(cell);
                    }
                }

            }
        }
    }
    KWTableFrameSet::Cell *cell=static_cast<KWTableFrameSet::Cell *>(m_pTable->getCell( m_rowBegin,m_colBegin ));
    m_pTable->joinCells(m_colBegin,m_rowBegin,m_colEnd+m_colBegin-1+cell->m_cols-1,m_rowBegin+m_rowEnd-1+cell->m_rows-1);

    doc->frameSelectedChanged();
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}



KWJoinCellCommand::KWJoinCellCommand( const QString &name, KWTableFrameSet * _table,unsigned int colBegin,unsigned int rowBegin, unsigned int colEnd,unsigned int rowEnd, QPtrList<KWFrameSet> listFrameSet,QPtrList<KWFrame> listCopyFrame):
    KNamedCommand(name),
    m_pTable(_table),
    m_colBegin(colBegin),
    m_rowBegin(rowBegin),
    m_colEnd(colEnd),
    m_rowEnd(rowEnd),
    m_ListFrameSet(listFrameSet),
    m_copyFrame(listCopyFrame)
{
    Q_ASSERT(m_pTable);
}
KWJoinCellCommand::~KWJoinCellCommand()
{
    m_copyFrame.setAutoDelete( true );
}

void KWJoinCellCommand::execute()
{
    kdDebug() << "KWJoinCellCommand::execute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    doc->terminateEditing(m_pTable);
    m_pTable->joinCells(m_colBegin,m_rowBegin,m_colEnd,m_rowEnd);
    doc->frameSelectedChanged();
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}

void KWJoinCellCommand::unexecute()
{
    kdDebug() << "KWJoinCellCommand::unexecute" << endl;
    KWDocument * doc = m_pTable->kWordDocument();
    doc->terminateEditing(m_pTable);
    m_pTable->splitCell(m_rowEnd-m_rowBegin+1, m_colEnd-m_colBegin+1,m_colBegin,m_rowBegin,m_ListFrameSet,m_copyFrame);
    doc->frameSelectedChanged();
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}


KWChangeStartingPageCommand::KWChangeStartingPageCommand( const QString &name, KWDocument *_doc, int _oldStartingPage, int _newStartingPage):
    KNamedCommand(name),
    m_doc(_doc),
    oldStartingPage(_oldStartingPage),
    newStartingPage(_newStartingPage)
{
}

void KWChangeStartingPageCommand::execute()
{
    m_doc->getVariableCollection()->variableSetting()->setStartingPage(newStartingPage);
    m_doc->recalcVariables( VT_PGNUM );
}

void KWChangeStartingPageCommand::unexecute()
{
    m_doc->getVariableCollection()->variableSetting()->setStartingPage(oldStartingPage);
    m_doc->recalcVariables( VT_PGNUM );
}

KWChangeVariableSettingsCommand::KWChangeVariableSettingsCommand( const QString &name, KWDocument *_doc, bool _oldValue, bool _newValue, VariableProperties _type):
    KNamedCommand(name),
    m_doc(_doc),
    type(_type),
    m_bOldValue(_oldValue),
    m_bNewValue(_newValue)
{
}

void KWChangeVariableSettingsCommand::changeValue( bool b )
{
    switch(type)
    {
    case VS_DISPLAYLINK:
        m_doc->getVariableCollection()->variableSetting()->setDisplayLink(b);
        m_doc->recalcVariables( VT_LINK );
        break;
    case  VS_UNDERLINELINK:
        m_doc->getVariableCollection()->variableSetting()->setUnderlineLink(b);
        m_doc->recalcVariables( VT_LINK );
        break;
    case VS_DISPLAYCOMMENT:
        m_doc->getVariableCollection()->variableSetting()->setDisplayComment(b);
        m_doc->recalcVariables( VT_NOTE );
        break;
    case VS_DISPLAYFIELDCODE:
        m_doc->getVariableCollection()->variableSetting()->setDisplayFiedCode(b);
        //hack necessary otherwise footnote frameset is not refreshing
        //and footnote is not resize.
        m_doc->displayFootNoteFiedCode();
        m_doc->recalcVariables( VT_ALL );
        break;
    }
}

void KWChangeVariableSettingsCommand::execute()
{
    changeValue(m_bNewValue);
}

void KWChangeVariableSettingsCommand::unexecute()
{
    changeValue(m_bOldValue);
}

KWChangeCustomVariableValue::KWChangeCustomVariableValue( const QString &name, KWDocument *_doc,const QString & _oldValue, const QString & _newValue,KoCustomVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KWChangeCustomVariableValue::~KWChangeCustomVariableValue()
{
}

void KWChangeCustomVariableValue::execute()
{
    Q_ASSERT(m_var);
    m_var->setValue(newValue);
    m_doc->recalcVariables( VT_CUSTOM );
}

void KWChangeCustomVariableValue::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setValue(oldValue);
    m_doc->recalcVariables( VT_CUSTOM );
}

KWChangeFieldVariableSubType::KWChangeFieldVariableSubType( const QString &name, KWDocument *_doc,
                        short int _oldValue, short int _newValue,
                        KWFieldVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KWChangeFieldVariableSubType::~KWChangeFieldVariableSubType()
{
}

void KWChangeFieldVariableSubType::execute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(newValue);
    m_doc->recalcVariables( VT_FIELD );
}

void KWChangeFieldVariableSubType::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(oldValue);
    m_doc->recalcVariables( VT_FIELD );
}


KWChangeVariableNoteText::KWChangeVariableNoteText( const QString &name, KWDocument *_doc,
                        const QString &_oldValue,const QString &_newValue,
                        KoNoteVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KWChangeVariableNoteText::~KWChangeVariableNoteText()
{
}

void KWChangeVariableNoteText::execute()
{
    Q_ASSERT(m_var);
    m_var->setNote(newValue);
}

void KWChangeVariableNoteText::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setNote(oldValue);
}


KWChangeDateVariableSubType::KWChangeDateVariableSubType( const QString &name, KWDocument *_doc,
                        short int _oldValue, short int _newValue,
                        KWDateVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KWChangeDateVariableSubType::~KWChangeDateVariableSubType()
{
}

void KWChangeDateVariableSubType::execute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(newValue);
    m_doc->recalcVariables( VT_DATE );
}

void KWChangeDateVariableSubType::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(oldValue);
    m_doc->recalcVariables( VT_DATE );
}

KWChangeDateVariableFormat::KWChangeDateVariableFormat( const QString &name, KWDocument *_doc,
                        const QString _oldValue, const QString _newValue,
                        KWDateVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KWChangeDateVariableFormat::~KWChangeDateVariableFormat()
{
}

void KWChangeDateVariableFormat::execute()
{
    Q_ASSERT(m_var);
    dynamic_cast<KoVariableDateFormat *>(m_var->variableFormat())->m_strFormat = newValue;
    m_doc->recalcVariables( VT_DATE );
}

void KWChangeDateVariableFormat::unexecute()
{
    Q_ASSERT(m_var);
    dynamic_cast<KoVariableDateFormat *>(m_var->variableFormat())->m_strFormat = oldValue;
    m_doc->recalcVariables( VT_DATE );
}

KWChangeTimeVariableSubType::KWChangeTimeVariableSubType( const QString &name, KWDocument *_doc,
                        short int _oldValue, short int _newValue,
                        KWTimeVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KWChangeTimeVariableSubType::~KWChangeTimeVariableSubType()
{
}

void KWChangeTimeVariableSubType::execute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(newValue);
    m_doc->recalcVariables( VT_TIME );
}

void KWChangeTimeVariableSubType::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(oldValue);
    m_doc->recalcVariables( VT_TIME );
}

KWChangeTimeVariableFormat::KWChangeTimeVariableFormat( const QString &name, KWDocument *_doc,
                        const QString _oldValue, const QString _newValue,
                        KWTimeVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KWChangeTimeVariableFormat::~KWChangeTimeVariableFormat()
{
}

void KWChangeTimeVariableFormat::execute()
{
    Q_ASSERT(m_var);
    dynamic_cast<KoVariableTimeFormat *>(m_var->variableFormat())->m_strFormat = newValue;
    m_doc->recalcVariables( VT_TIME );
}

void KWChangeTimeVariableFormat::unexecute()
{
    Q_ASSERT(m_var);
    dynamic_cast<KoVariableTimeFormat *>(m_var->variableFormat())->m_strFormat = oldValue;
    m_doc->recalcVariables( VT_TIME );
}

KWChangePgNumVariableValue::KWChangePgNumVariableValue( const QString &name, KWDocument *_doc,
                        short int _oldValue, short int _newValue,
                        KWPgNumVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KWChangePgNumVariableValue::~KWChangePgNumVariableValue()
{
}

void KWChangePgNumVariableValue::execute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(newValue);
    m_doc->recalcVariables( VT_PGNUM );
}

void KWChangePgNumVariableValue::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(oldValue);
    m_doc->recalcVariables( VT_PGNUM );
}


KWChangeLinkVariable::KWChangeLinkVariable( const QString &name, KWDocument *_doc,const QString & _oldHref, const QString & _newHref, const QString & _oldLink,const QString &_newLink, KoLinkVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    oldHref(_oldHref),
    newHref(_newHref),
    oldLink(_oldLink),
    newLink(_newLink),
    m_var(var)
{
}


void KWChangeLinkVariable::execute()
{
    m_var->setLink(newLink,newHref);
    m_doc->recalcVariables(VT_LINK);
}

void KWChangeLinkVariable::unexecute()
{
    m_var->setLink(oldLink,oldHref);
    m_doc->recalcVariables(VT_LINK);
}

KWHideShowHeader::KWHideShowHeader( const QString &name, KWDocument *_doc, bool _newValue):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue)
{
}


void KWHideShowHeader::execute()
{
    m_doc->setHeaderVisible(newValue );
    m_doc->updateHeaderButton();

}

void KWHideShowHeader::unexecute()
{
    m_doc->setHeaderVisible(!newValue );
    m_doc->updateHeaderButton();
}

KWHideShowFooter::KWHideShowFooter( const QString &name, KWDocument *_doc, bool _newValue):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue)
{
}


void KWHideShowFooter::execute()
{
    m_doc->setFooterVisible( newValue );
    m_doc->updateFooterButton();
}

void KWHideShowFooter::unexecute()
{
    m_doc->setFooterVisible( !newValue );
    m_doc->updateFooterButton();
}


KWProtectContentCommand::KWProtectContentCommand( const QString &name, KWTextFrameSet*frameset, bool _protect):
    KNamedCommand(name),
    m_pFrameSet(frameset),
    m_bProtect(_protect)
{
}


void KWProtectContentCommand::execute()
{
    m_pFrameSet->textObject()->setProtectContent(m_bProtect);
    m_pFrameSet->kWordDocument()->updateTextFrameSetEdit();
    m_pFrameSet->kWordDocument()->testAndCloseAllFrameSetProtectedContent();

}

void KWProtectContentCommand::unexecute()
{
    m_pFrameSet->textObject()->setProtectContent(!m_bProtect);
    m_pFrameSet->kWordDocument()->updateTextFrameSetEdit();
    m_pFrameSet->kWordDocument()->testAndCloseAllFrameSetProtectedContent();
}


FrameMarginsStruct::FrameMarginsStruct( KWFrame *frame )
{
    topMargin = frame->bTop();
    bottomMargin= frame->bBottom();
    leftMargin = frame->bLeft();
    rightMargin= frame->bRight();
}

FrameMarginsStruct::FrameMarginsStruct( double _left, double _top, double _right, double _bottom ):
    topMargin(_top),
    bottomMargin(_bottom),
    leftMargin(_left),
    rightMargin(_right)
{
}


KWFrameChangeFrameMarginCommand::KWFrameChangeFrameMarginCommand( const QString &name, FrameIndex _frameIndex, FrameMarginsStruct _frameMarginsBegin, FrameMarginsStruct _frameMarginsEnd ) :
    KNamedCommand(name),
    m_indexFrame(_frameIndex),
    m_frameMarginsBegin(_frameMarginsBegin),
    m_frameMarginsEnd(_frameMarginsEnd)
{
}

void KWFrameChangeFrameMarginCommand::execute()
{
    KWFrameSet *frameSet = m_indexFrame.m_pFrameSet;
    Q_ASSERT( frameSet );
    KWFrame *frame = frameSet->frame(m_indexFrame.m_iFrameIndex);
    Q_ASSERT( frame );
    frame->setFrameMargins( m_frameMarginsEnd.leftMargin,m_frameMarginsEnd.topMargin , m_frameMarginsEnd.rightMargin, m_frameMarginsEnd.bottomMargin);
    KWDocument * doc = frameSet->kWordDocument();
    doc->frameChanged( frame );
}

void KWFrameChangeFrameMarginCommand::unexecute()
{
    KWFrameSet *frameSet = m_indexFrame.m_pFrameSet;
    Q_ASSERT( frameSet );
    KWFrame *frame = frameSet->frame(m_indexFrame.m_iFrameIndex);
    Q_ASSERT( frame );
    frame->setFrameMargins( m_frameMarginsBegin.leftMargin,m_frameMarginsBegin.topMargin , m_frameMarginsBegin.rightMargin, m_frameMarginsBegin.bottomMargin);
    KWDocument * doc = frameSet->kWordDocument();
    doc->frameChanged( frame );
}

KWChangeFootEndNoteSettingsCommand::KWChangeFootEndNoteSettingsCommand( const QString &name, KoParagCounter _oldCounter, KoParagCounter _newCounter ,bool _footNote ,KWDocument *_doc):
    KNamedCommand(name),
    m_oldCounter(_oldCounter),
    m_newCounter(_newCounter),
    m_footNote( _footNote ),
    m_doc(_doc)
{
}

void KWChangeFootEndNoteSettingsCommand::execute()
{
    changeCounter( m_newCounter);
}

void KWChangeFootEndNoteSettingsCommand::unexecute()
{
    changeCounter( m_oldCounter);
}

void KWChangeFootEndNoteSettingsCommand::changeCounter( KoParagCounter counter)
{
    if (m_footNote )
    {
        static_cast<KWVariableSettings*>(m_doc->getVariableCollection()->variableSetting())->changeFootNoteCounter(counter );
    }
    else
    {
        static_cast<KWVariableSettings*>(m_doc->getVariableCollection()->variableSetting())->changeEndNoteCounter(counter );
    }
    m_doc->changeFootNoteConfig();
}


KWChangeTabStopValueCommand::KWChangeTabStopValueCommand( const QString &name, double _oldValue, double _newValue, KWDocument *_doc):
    KNamedCommand(name),
    m_doc( _doc ),
    m_oldValue(_oldValue),
    m_newValue(_newValue)
{
}

void KWChangeTabStopValueCommand::execute()
{
    m_doc->setTabStopValue ( m_newValue );
}

void KWChangeTabStopValueCommand::unexecute()
{
    m_doc->setTabStopValue ( m_oldValue );
}



FootNoteParameter::FootNoteParameter( KWFootNoteVariable *_var )
{
    noteType = _var->noteType();
    numberingType = _var->numberingType();
    manualString = _var->manualString();
}

FootNoteParameter::FootNoteParameter( NoteType _noteType, KWFootNoteVariable::Numbering _numberingType, const QString &_manualString)
{
    noteType= _noteType;
    numberingType = _numberingType;
    manualString = _manualString;
}

KWChangeFootNoteParametersCommand::KWChangeFootNoteParametersCommand( const QString &name, KWFootNoteVariable * _var, FootNoteParameter _oldParameter, FootNoteParameter _newParameter, KWDocument *_doc):
    KNamedCommand(name),
    m_doc( _doc ),
    m_var( _var ),
    m_oldParameter( _oldParameter ),
    m_newParameter( _newParameter)
{
}

void KWChangeFootNoteParametersCommand::execute()
{
    changeVariableParameter( m_newParameter );
}

void KWChangeFootNoteParametersCommand::unexecute()
{
    changeVariableParameter( m_oldParameter );
}

void KWChangeFootNoteParametersCommand::changeVariableParameter( FootNoteParameter _param )
{
    m_var->setNoteType( _param.noteType );

    m_var->setNumberingType( _param.numberingType );
    m_var->setManualString( _param.manualString );
    m_var->resize();
    m_var->frameSet()->setCounterText( m_var->text() );
    m_var->paragraph()->invalidate(0);
    m_var->paragraph()->setChanged( true );
    m_doc->slotRepaintVariable();
}


KWChangeFootNoteLineSeparatorParametersCommand::KWChangeFootNoteLineSeparatorParametersCommand( const QString &name, SeparatorLinePos _oldValuePos, SeparatorLinePos _newValuePos, int _oldLength, int _newLength, double _oldWidth, double _newWidth, SeparatorLineLineType _oldLineType, SeparatorLineLineType _newLineType, KWDocument *_doc):
    KNamedCommand(name),
    m_doc( _doc ),
    m_oldValuePos(_oldValuePos),
    m_newValuePos(_newValuePos),
    m_oldLength(_oldLength),
    m_newLength(_newLength),
    m_oldWidth(_oldWidth),
    m_newWidth(_newWidth),
    m_oldLineType(_oldLineType),
    m_newLineType(_newLineType)

{
}

void KWChangeFootNoteLineSeparatorParametersCommand::execute()
{
    changeLineSeparatorParameter( m_newValuePos, m_newLength, m_newWidth,m_newLineType );
}

void KWChangeFootNoteLineSeparatorParametersCommand::unexecute()
{
    changeLineSeparatorParameter( m_oldValuePos, m_oldLength, m_oldWidth, m_oldLineType);
}

void KWChangeFootNoteLineSeparatorParametersCommand::changeLineSeparatorParameter( SeparatorLinePos _pos, int _length, double _width, SeparatorLineLineType _type)
{
    m_doc->setFootNoteSeparatorLinePosition( _pos );
    m_doc->setFootNoteSeparatorLineLength( _length);
    m_doc->setFootNoteSeparatorLineWidth(_width );
    m_doc->setFootNoteSeparatorLineType( _type );
    m_doc->repaintAllViews();
}


KWRenameBookmarkCommand::KWRenameBookmarkCommand( const QString &name, const QString & _oldname, const QString & _newName, KWDocument *_doc):
    KNamedCommand(name),
    m_doc( _doc ),
    m_oldName( _oldname),
    m_newName( _newName)
{
}

void KWRenameBookmarkCommand::execute()
{
    m_doc->renameBookMark( m_oldName, m_newName);
}

void KWRenameBookmarkCommand::unexecute()
{
        m_doc->renameBookMark( m_newName, m_oldName);
}


