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

#include <qtimer.h>
#include <qregexp.h>
#include "kotextobject.h"
#include "koparagcounter.h"
#include "kozoomhandler.h"
#include "kocommand.h"
#include "kostyle.h"
#include <klocale.h>
#include <kdebug.h>
#include <kotextformat.h>

//#define DEBUG_FORMATS
//#define DEBUG_FORMAT_MORE

struct KoTextObject::KoTextObjectPrivate
{
public:
    KoTextObjectPrivate() {
        afterFormattingEmitted = false;
    }
    bool afterFormattingEmitted;
};

KoTextObject::KoTextObject( KoZoomHandler *zh, const QFont& defaultFont, KoStyle* defaultStyle,
                            QObject* parent, const char *name )
    : QObject( parent, name ), m_defaultStyle( defaultStyle ), undoRedoInfo( this )
{
    textdoc = new KoTextDocument( zh, new KoTextFormatCollection( defaultFont ) );
    init();
}

KoTextObject::KoTextObject( KoTextDocument* _textdoc, KoStyle* defaultStyle,
                            QObject* parent, const char *name )
 : QObject( parent, name ), m_defaultStyle( defaultStyle ), undoRedoInfo( this )
{
    textdoc = _textdoc;
    init();
}

void KoTextObject::init()
{
    d = new KoTextObjectPrivate;
    m_availableHeight = -1;
    m_lastFormatted = textdoc->firstParag();
    m_highlightSelectionAdded = false;
    interval = 0;
    changeIntervalTimer = new QTimer( this );
    connect( changeIntervalTimer, SIGNAL( timeout() ),
             this, SLOT( doChangeInterval() ) );

    formatTimer = new QTimer( this );
    connect( formatTimer, SIGNAL( timeout() ),
             this, SLOT( formatMore() ) );

    // Apply default style to initial paragraph
    if ( m_lastFormatted && m_defaultStyle )
        static_cast<KoTextParag*>(m_lastFormatted)->applyStyle( m_defaultStyle );
}

KoTextObject::~KoTextObject()
{
    delete textdoc; textdoc = 0;
    delete d;
}

int KoTextObject::availableHeight() const
{
    if ( m_availableHeight == -1 )
        emit const_cast<KoTextObject *>(this)->availableHeightNeeded();
    Q_ASSERT( m_availableHeight != -1 );
    return m_availableHeight;
}

int KoTextObject::docFontSize( QTextFormat * format ) const
{
    Q_ASSERT( format );
    return static_cast<int>( KoTextZoomHandler::layoutUnitPtToPt( format->font().pointSize() ) );
}

int KoTextObject::zoomedFontSize( int docFontSize ) const
{
    kdDebug() << "KoTextObject::zoomedFontSize: docFontSize=" << docFontSize
              << " - in LU: " << KoTextZoomHandler::ptToLayoutUnitPt( docFontSize ) << endl;
    return KoTextZoomHandler::ptToLayoutUnitPt( docFontSize );
}

void KoTextObject::slotAfterUndoRedo()
{
    formatMore();
    emit repaintChanged( this );
    emit updateUI( true );
    emit showCursor();
    emit ensureCursorVisible();
}

void KoTextObject::clearUndoRedoInfo()
{
    undoRedoInfo.clear();
}


void KoTextObject::checkUndoRedoInfo( QTextCursor * cursor, UndoRedoInfo::Type t )
{
    if ( undoRedoInfo.valid() && ( t != undoRedoInfo.type || cursor != undoRedoInfo.cursor ) ) {
        undoRedoInfo.clear();
    }
    undoRedoInfo.type = t;
    undoRedoInfo.cursor = cursor;
}

void KoTextObject::undo()
{
    undoRedoInfo.clear();
    emit hideCursor();
    QTextCursor *cursor = new QTextCursor( textDocument() ); // Kindof a dummy cursor
    QTextCursor *c = textDocument()->undo( cursor );
    if ( !c ) {
        emit showCursor();
        return;
    }
    // We have to set this new cursor position in all views :(
    // It sucks a bit for useability, but otherwise one view might still have
    // a cursor inside a deleted paragraph -> crash.
    emit setCursor( c );
    setLastFormattedParag( textdoc->firstParag() );
    QTimer::singleShot( 0, this, SLOT( slotAfterUndoRedo() ) );
}

void KoTextObject::redo()
{
    undoRedoInfo.clear();
    emit hideCursor();
    QTextCursor *cursor = new QTextCursor( textDocument() ); // Kindof a dummy cursor
    QTextCursor *c = textDocument()->redo( cursor );
    if ( !c ) {
        emit showCursor();
        return;
    }
    emit setCursor( c ); // see undo
    setLastFormattedParag( textdoc->firstParag() );
    QTimer::singleShot( 0, this, SLOT( slotAfterUndoRedo() ) );
}

KoTextObject::UndoRedoInfo::UndoRedoInfo( KoTextObject *to )
    : type( Invalid ), textobj(to), cursor( 0 )
{
    text = QString::null;
    id = -1;
    index = -1;
    placeHolderCmd = 0L;
}

bool KoTextObject::UndoRedoInfo::valid() const
{
    return text.length() > 0 && id >= 0 && index >= 0;
}

void KoTextObject::UndoRedoInfo::clear()
{
    if ( valid() ) {
        KoTextDocument * textdoc = textobj->textDocument();
        switch (type) {
            case Insert:
            case Return:
            {
                QTextCommand * cmd = new KoTextInsertCommand( textdoc, id, index, text.rawData(), customItemsMap, oldParagLayouts );
                textdoc->addCommand( cmd );
                Q_ASSERT( placeHolderCmd );
                // Inserting any custom items -> macro command, to let custom items add their command
                if ( !customItemsMap.isEmpty() )
                {
                    CustomItemsMap::Iterator it = customItemsMap.begin();
                    for ( ; it != customItemsMap.end(); ++it )
                    {
                        KoTextCustomItem * item = it.data();
                        KCommand * itemCmd = item->createCommand();
                        if ( itemCmd )
                            placeHolderCmd->addCommand( itemCmd );
                    }
                    placeHolderCmd->addCommand( new KoTextCommand( textobj, /*cmd, */QString::null ) );
                }
                else
                {
                    placeHolderCmd->addCommand( new KoTextCommand( textobj, /*cmd, */QString::null ) );
                }
            } break;
            case Delete:
            case RemoveSelected:
            {
                QTextCommand * cmd = new KoTextDeleteCommand( textdoc, id, index, text.rawData(), customItemsMap, oldParagLayouts );
                textdoc->addCommand( cmd );
                Q_ASSERT( placeHolderCmd );
                placeHolderCmd->addCommand( new KoTextCommand( textobj, /*cmd, */QString::null ) );
                // Deleting any custom items -> let them add their command
                if ( !customItemsMap.isEmpty() )
                {
                    customItemsMap.deleteAll( placeHolderCmd );
                }
           } break;
            case Invalid:
                break;
        }
    }
    type = Invalid;
    text = QString::null; // calls KoTextString::clear(), which calls resize(0) on the array, which _detaches_. Tricky.
    id = -1;
    index = -1;
    oldParagLayouts.clear();
    customItemsMap.clear();
    placeHolderCmd = 0L;
}

void KoTextObject::copyCharFormatting( Qt3::QTextParag *parag, int position, int index /*in text*/, bool moveCustomItems )
{
    KoTextStringChar * ch = parag->at( position );
    if ( ch->format() ) {
        ch->format()->addRef();
        undoRedoInfo.text.at( index ).setFormat( ch->format() );
    }
    if ( ch->isCustom() )
    {
        kdDebug(32001) << "KoTextObject::copyCharFormatting moving custom item " << ch->customItem() << " to text's " << index << " char"  << endl;
        undoRedoInfo.customItemsMap.insert( index, static_cast<KoTextCustomItem *>( ch->customItem() ) );
        // We copy the custom item to customItemsMap in all cases (see setFormat)
        // We only remove from 'ch' if moveCustomItems was specified
        if ( moveCustomItems )
            static_cast<KoTextParag*>(parag)->removeCustomItem(position);
        //ch->loseCustomItem();
    }
}

// Based on QTextView::readFormats - with all code duplication moved to copyCharFormatting
void KoTextObject::readFormats( QTextCursor &c1, QTextCursor &c2, bool copyParagLayouts, bool moveCustomItems )
{
    //kdDebug() << "KoTextObject::readFormats moveCustomItems=" << moveCustomItems << endl;
    c2.restoreState();
    c1.restoreState();
    int oldLen = undoRedoInfo.text.length();
    if ( c1.parag() == c2.parag() ) {
        undoRedoInfo.text += c1.parag()->string()->toString().mid( c1.index(), c2.index() - c1.index() );
        for ( int i = c1.index(); i < c2.index(); ++i )
            copyCharFormatting( c1.parag(), i, oldLen + i - c1.index(), moveCustomItems );
    } else {
        int lastIndex = oldLen;
        int i;
        //kdDebug() << "KoTextObject::readFormats copying from " << c1.index() << " to " << c1.parag()->length()-1 << " into lastIndex=" << lastIndex << endl;
        // Replace the trailing spaces with '\n'. That char carries the formatting for the trailing space.
        undoRedoInfo.text += c1.parag()->string()->toString().mid( c1.index(), c1.parag()->length() - 1 - c1.index() ) + '\n';
        for ( i = c1.index(); i < c1.parag()->length(); ++i, ++lastIndex )
            copyCharFormatting( c1.parag(), i, lastIndex, moveCustomItems );
        //++lastIndex; // skip the '\n'.
        Qt3::QTextParag *p = c1.parag()->next();
        while ( p && p != c2.parag() ) {
            undoRedoInfo.text += p->string()->toString().left( p->length() - 1 ) + '\n';
            //kdDebug() << "KoTextObject::readFormats (mid) copying from 0 to "  << p->length()-1 << " into i+" << lastIndex << endl;
            for ( i = 0; i < p->length(); ++i )
                copyCharFormatting( p, i, i + lastIndex, moveCustomItems );
            lastIndex += p->length(); // + 1; // skip the '\n'
            //kdDebug() << "KoTextObject::readFormats lastIndex now " << lastIndex << " - text is now " << undoRedoInfo.text.toString() << endl;
            p = p->next();
        }
        //kdDebug() << "KoTextObject::readFormats copying [last] from 0 to " << c2.index() << " into i+" << lastIndex << endl;
        undoRedoInfo.text += c2.parag()->string()->toString().left( c2.index() );
        for ( i = 0; i < c2.index(); ++i )
            copyCharFormatting( c2.parag(), i, i + lastIndex, moveCustomItems );
    }

    if ( copyParagLayouts ) {
        Qt3::QTextParag *p = c1.parag();
        while ( p ) {
            undoRedoInfo.oldParagLayouts << static_cast<KoTextParag*>(p)->paragLayout();
            if ( p == c2.parag() )
                break;
            p = p->next();
        }
    }
}

void KoTextObject::newPlaceHolderCommand( const QString & name )
{
    Q_ASSERT( !undoRedoInfo.placeHolderCmd );
    undoRedoInfo.placeHolderCmd = new KMacroCommand( name );
    emit newCommand( undoRedoInfo.placeHolderCmd );
}

void KoTextObject::storeParagUndoRedoInfo( QTextCursor * cursor, int selectionId )
{
    undoRedoInfo.clear();
    KoTextDocument * textdoc = textDocument();
    undoRedoInfo.oldParagLayouts.clear();
    undoRedoInfo.text = " ";
    undoRedoInfo.index = 1;
    if ( !textdoc->hasSelection( selectionId ) ) {
        Qt3::QTextParag * p = cursor->parag();
        undoRedoInfo.id = p->paragId();
        undoRedoInfo.eid = p->paragId();
        undoRedoInfo.oldParagLayouts << static_cast<KoTextParag*>(p)->paragLayout();
    }
    else{
        Qt3::QTextParag *start = textdoc->selectionStart( selectionId );
        Qt3::QTextParag *end = textdoc->selectionEnd( selectionId );
        undoRedoInfo.id = start->paragId();
        undoRedoInfo.eid = end->paragId();
        for ( ; start && start != end->next() ; start = start->next() )
        {
            undoRedoInfo.oldParagLayouts << static_cast<KoTextParag*>(start)->paragLayout();
            //kdDebug(32001) << "KoTextObject:storeParagUndoRedoInfo storing counter " << static_cast<KoTextParag*>(start)->paragLayout().counter.counterType << endl;
        }
    }
}

void KoTextObject::doKeyboardAction( QTextCursor * cursor, KoTextFormat * & /*currentFormat*/, KeyboardAction action )
{
    KoTextParag * parag = static_cast<KoTextParag *>(cursor->parag());
    setLastFormattedParag( parag );
    emit hideCursor();
    bool doUpdateCurrentFormat = true;
    switch ( action ) {
    case ActionDelete: {
        checkUndoRedoInfo( cursor, UndoRedoInfo::Delete );
        if ( !undoRedoInfo.valid() ) {
            newPlaceHolderCommand( i18n("Delete text") );
            undoRedoInfo.id = parag->paragId();
            undoRedoInfo.index = cursor->index();
            undoRedoInfo.text = QString::null;
            undoRedoInfo.oldParagLayouts << parag->paragLayout();
        }
        if ( !cursor->atParagEnd() )
        {
            KoTextStringChar * ch = parag->at( cursor->index() );
            undoRedoInfo.text += ch->c;
            copyCharFormatting( parag, cursor->index(), undoRedoInfo.text.length()-1, true );
        }
        KoParagLayout paragLayout;
        if ( parag->next() )
            paragLayout = static_cast<KoTextParag *>( parag->next() )->paragLayout();

        Qt3::QTextParag *old = cursor->parag();
        if ( cursor->remove() ) {
            if ( old != cursor->parag() && m_lastFormatted == old ) // 'old' has been deleted
                m_lastFormatted = cursor->parag() ? cursor->parag()->prev() : 0;
            undoRedoInfo.text += "\n";
            undoRedoInfo.oldParagLayouts << paragLayout;
        }
    } break;
    case ActionBackspace: {
        // Remove counter
        if ( parag->counter() && parag->counter()->style() != KoParagCounter::STYLE_NONE && cursor->index() == 0 ) {
            // parag->decDepth(); // We don't have support for nested lists at the moment
                                  // (only in titles, but you don't want Backspace to move it up)
            KoParagCounter c;
            KCommand *cmd=setCounterCommand( cursor, c );
            if(cmd)
                emit newCommand(cmd);
        }
        else if ( !cursor->atParagStart() || parag->prev() )
        // Nothing to do if at the beginning of the very first parag
        {
            checkUndoRedoInfo( cursor, UndoRedoInfo::Delete );
            if ( !undoRedoInfo.valid() ) {
                newPlaceHolderCommand( i18n("Delete text") );
                undoRedoInfo.id = parag->paragId();
                undoRedoInfo.index = cursor->index();
                undoRedoInfo.text = QString::null;
                undoRedoInfo.oldParagLayouts << parag->paragLayout();
            }
            cursor->gotoPreviousLetter();
            KoTextStringChar * ch = cursor->parag()->at( cursor->index() );
            undoRedoInfo.text.prepend( QString( ch->c ) );
            copyCharFormatting( cursor->parag(), cursor->index(), 0, true );
            undoRedoInfo.index = cursor->index();
            KoParagLayout paragLayout = static_cast<KoTextParag *>( cursor->parag() )->paragLayout();
            if ( cursor->remove() ) {
                undoRedoInfo.text.remove( 0, 1 );
                undoRedoInfo.text.prepend( "\n" );
                undoRedoInfo.index = cursor->index();
                undoRedoInfo.id = cursor->parag()->paragId();
                undoRedoInfo.oldParagLayouts.prepend( paragLayout );
            }
            m_lastFormatted = cursor->parag();
        }
    } break;
    case ActionReturn: {
        checkUndoRedoInfo( cursor, UndoRedoInfo::Return );
        if ( !undoRedoInfo.valid() ) {
            newPlaceHolderCommand( i18n("Insert text") );
            undoRedoInfo.id = cursor->parag()->paragId();
            undoRedoInfo.index = cursor->index();
            undoRedoInfo.text = QString::null;
        }
        undoRedoInfo.text += "\n";
        cursor->splitAndInsertEmptyParag();
        Q_ASSERT( cursor->parag()->prev() );
        if ( cursor->parag()->prev() )
            setLastFormattedParag( cursor->parag()->prev() );

        doUpdateCurrentFormat = false;
        KoStyle * style = static_cast<KoTextParag *>( cursor->parag()->prev() )->style();
        if ( style )
        {
            KoStyle * newStyle = style->followingStyle();
            if ( newStyle && style != newStyle ) // different "following style" applied
            {
                doUpdateCurrentFormat = true;
                //currentFormat = static_cast<KoTextFormat *>( textDocument()->formatCollection()->format( cursor->parag()->paragFormat() ) );
                //kdDebug() << "KoTextFrameSet::doKeyboardAction currentFormat=" << currentFormat << " " << currentFormat->key() << endl;
            }
        }

    } break;
    case ActionKill:
        // Nothing to kill if at end of very last paragraph
        if ( !cursor->atParagEnd() || cursor->parag()->next() ) {
            checkUndoRedoInfo( cursor, UndoRedoInfo::Delete );
            if ( !undoRedoInfo.valid() ) {
                newPlaceHolderCommand( i18n("Delete text") );
                undoRedoInfo.id = cursor->parag()->paragId();
                undoRedoInfo.index = cursor->index();
                undoRedoInfo.text = QString::null;
                undoRedoInfo.oldParagLayouts << parag->paragLayout();
            }
            if ( cursor->atParagEnd() ) {
                // Get paraglayout from next parag (next can't be 0 here)
                KoParagLayout paragLayout = static_cast<KoTextParag *>( parag->next() )->paragLayout();
                if ( cursor->remove() )
                {
                    m_lastFormatted = cursor->parag();
                    undoRedoInfo.text += "\n";
                    undoRedoInfo.oldParagLayouts << paragLayout;
                }
            } else {
                int oldLen = undoRedoInfo.text.length();
                undoRedoInfo.text += cursor->parag()->string()->toString().mid( cursor->index() );
                for ( int i = cursor->index(); i < cursor->parag()->length(); ++i )
                    copyCharFormatting( cursor->parag(), i, oldLen + i - cursor->index(), true );
                cursor->killLine();
            }
        }
        break;
    }

    if ( !undoRedoInfo.customItemsMap.isEmpty() )
        clearUndoRedoInfo();

    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit showCursor();
    emit updateUI( doUpdateCurrentFormat );
}

void KoTextObject::insert( QTextCursor * cursor, KoTextFormat * currentFormat,
                             const QString &txt, bool checkNewLine,
                             bool removeSelected, const QString & commandName,
                             CustomItemsMap customItemsMap )
{
    //kdDebug(32001) << "KoTextObject::insert txt=" << txt << endl;
    KoTextDocument *textdoc = textDocument();
    emit hideCursor();
    if ( textdoc->hasSelection( KoTextDocument::Standard ) && removeSelected  ) {
        //removeSelectedText( cursor );
        if( customItemsMap.isEmpty())
        {
            emitNewCommand(replaceSelectionCommand( cursor, txt,KoTextDocument::Standard, commandName));
            return;
        }
        else
        {
            removeSelectedTextCommand( cursor,KoTextDocument::Standard  );
        }
    }
    QTextCursor c2 = *cursor;
    if ( !customItemsMap.isEmpty() )
        clearUndoRedoInfo();
    checkUndoRedoInfo( cursor, UndoRedoInfo::Insert );
    if ( !undoRedoInfo.valid() ) {
        if ( !commandName.isNull() ) // see replace-selection
            newPlaceHolderCommand( commandName );
        undoRedoInfo.id = cursor->parag()->paragId();
        undoRedoInfo.index = cursor->index();
        undoRedoInfo.text = QString::null;
    }
    int oldLen = undoRedoInfo.text.length();
    setLastFormattedParag( checkNewLine && cursor->parag()->prev() ?
                           cursor->parag()->prev() : cursor->parag() );
    QTextCursor oldCursor = *cursor;
    cursor->insert( txt, checkNewLine );  // insert the text

    if ( !customItemsMap.isEmpty() ) {
        customItemsMap.insertItems( oldCursor, txt.length() );
        undoRedoInfo.customItemsMap = customItemsMap;
    }

    textdoc->setSelectionStart( KoTextDocument::Temp, &oldCursor );
    textdoc->setSelectionEnd( KoTextDocument::Temp, cursor );
    //kdDebug() << "KoTextObject::insert setting format " << currentFormat << endl;
    textdoc->setFormat( KoTextDocument::Temp, currentFormat, QTextFormat::Format );
    textdoc->removeSelection( KoTextDocument::Temp );

    // Speed optimization: if we only type a char, and it doesn't
    // invalidate the next parag, only format the current one
    Qt3::QTextParag *parag = cursor->parag();
    if ( !checkNewLine && m_lastFormatted == parag && parag->next() && parag->next()->isValid() )
    {
        parag->format();
        m_lastFormatted = m_lastFormatted->next();
    } else
    {
        formatMore();
    }
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit showCursor();
    undoRedoInfo.text += txt;
    for ( int i = 0; i < (int)txt.length(); ++i ) {
        if ( txt[ oldLen + i ] != '\n' )
            copyCharFormatting( c2.parag(), c2.index(), oldLen + i, false );
        c2.gotoNextLetter();
    }

    if ( !removeSelected ) {
        // ## not sure why we do this. I'd prefer leaving the selection unchanged...
        // but then it'd need adjustements in the offsets etc.
        if ( textdoc->removeSelection( KoTextDocument::Standard ) )
        {
            selectionChangedNotify();
            emit repaintChanged( this );
        }
    }
    if ( !customItemsMap.isEmpty() )
        clearUndoRedoInfo();
}

void KoTextObject::pasteText( QTextCursor * cursor, const QString & text, KoTextFormat * currentFormat, bool removeSelected )
{
    kdDebug(32001) << "KoTextObject::pasteText" << endl;
    QString t = text;
    // Need to convert CRLF to NL
    QRegExp crlf( QString::fromLatin1("\r\n") );
    t.replace( crlf, QChar('\n') );
    // Convert non-printable chars
    for ( int i=0; (uint) i<t.length(); i++ ) {
        if ( t[ i ] < ' ' && t[ i ] != '\n' && t[ i ] != '\t' )
            t[ i ] = ' ';
    }
    if ( !t.isEmpty() )
        insert( cursor, currentFormat, t, true /*checkNewLine*/, removeSelected, i18n("Paste Text") );
}


void KoTextObject::applyStyle( QTextCursor * cursor, const KoStyle * newStyle,
                               int selectionId,
                               int paragLayoutFlags, int formatFlags,
                               bool createUndoRedo, bool interactive )
{
    KoTextDocument * textdoc = textDocument();
    if ( interactive )
        emit hideCursor();

    /// Applying a style is three distinct operations :
    /// 1 - Changing the paragraph settings (setParagLayout)
    /// 2 - Changing the character formatting for each char in the paragraph (setFormat(indices))
    /// 3 - Changing the character formatting for the whole paragraph (setFormat()) [just in case]
    /// -> We need a macro command to hold the 3 commands
    KMacroCommand * macroCmd = createUndoRedo ? new KMacroCommand( i18n("Apply style %1").
                                                                   arg(newStyle->translatedName() ) ) : 0;

    // 1
    //kdDebug(32001) << "KoTextObject::applyStyle setParagLayout" << endl;
    storeParagUndoRedoInfo( cursor, selectionId );
    undoRedoInfo.type = UndoRedoInfo::Invalid; // tricky, we don't want clear() to create a command
    if ( paragLayoutFlags != 0 )
    {
        if ( !textdoc->hasSelection( selectionId ) ) {
            static_cast<KoTextParag*>(cursor->parag())->setParagLayout( newStyle->paragLayout(), paragLayoutFlags );
        } else {
            Qt3::QTextParag *start = textdoc->selectionStart( selectionId );
            Qt3::QTextParag *end = textdoc->selectionEnd( selectionId );
            for ( ; start && start != end->next() ; start = start->next() )
                static_cast<KoTextParag*>(start)->setParagLayout( newStyle->paragLayout(), paragLayoutFlags );
        }

        if ( createUndoRedo )
        {
            //kdDebug(32001) << "KoTextObject::applyStyle KoTextParagCommand" << endl;
            QTextCommand * cmd = new KoTextParagCommand( textdoc, undoRedoInfo.id, undoRedoInfo.eid,
                                                         undoRedoInfo.oldParagLayouts,
                                                         newStyle->paragLayout(), paragLayoutFlags );
            textdoc->addCommand( cmd );
            macroCmd->addCommand( new KoTextCommand( this, /*cmd, */QString::null ) );
        }
    }

    // 2
    //kdDebug(32001) << "KoTextObject::applyStyle gathering text and formatting" << endl;
    Qt3::QTextParag * firstParag;
    Qt3::QTextParag * lastParag;
    if ( !textdoc->hasSelection( selectionId ) ) {
        // No selection -> apply style formatting to the whole paragraph
        firstParag = cursor->parag();
        lastParag = cursor->parag();
    }
    else
    {
        firstParag = textdoc->selectionStart( selectionId );
        lastParag = textdoc->selectionEnd( selectionId );
    }

    if ( formatFlags != 0 )
    {
        KoTextFormat * newFormat = static_cast<KoTextFormat *>( textdoc->formatCollection()->format( &newStyle->format() ) );

        if ( createUndoRedo )
        {
            QValueList<QTextFormat *> lstFormats;
            //QString str;
            for ( Qt3::QTextParag * parag = firstParag ; parag && parag != lastParag->next() ; parag = parag->next() )
            {
                //str += parag->string()->toString() + '\n';
                lstFormats.append( parag->paragFormat() );
            }
            QTextCursor c1( textdoc );
            c1.setParag( firstParag );
            c1.setIndex( 0 );
            QTextCursor c2( textdoc );
            c2.setParag( lastParag );
            c2.setIndex( lastParag->string()->length() );
            undoRedoInfo.clear();
            undoRedoInfo.type = UndoRedoInfo::Invalid; // same trick
            readFormats( c1, c2 ); // gather char-format info but not paraglayouts nor customitems

            QTextCommand * cmd = new KoTextFormatCommand( textdoc, firstParag->paragId(), 0,
                                                         lastParag->paragId(), c2.index(),
                                                         undoRedoInfo.text.rawData(), newFormat,
                                                         formatFlags );
            textdoc->addCommand( cmd );
            macroCmd->addCommand( new KoTextCommand( this, /*cmd, */QString::null ) );

            // sub-command for '3' (paragFormat)
            cmd = new KoParagFormatCommand( textdoc, firstParag->paragId(), lastParag->paragId(),
                                            lstFormats, newFormat );
            textdoc->addCommand( cmd );
            macroCmd->addCommand( new KoTextCommand( this, /*cmd, */QString::null ) );
        }

        // apply '2' and '3' (format)
        for ( Qt3::QTextParag * parag = firstParag ; parag && parag != lastParag->next() ; parag = parag->next() )
        {
            kdDebug(32001) << "KoTextObject::applyStyle parag:" << parag->paragId()
                           << ", from 0 to " << parag->string()->length() << ", format=" << newFormat << endl;
            parag->setFormat( 0, parag->string()->length(), newFormat, true, formatFlags );
            parag->setFormat( newFormat );
        }
        //currentFormat = static_cast<KoTextFormat *>( textDocument()->formatCollection()->format( newFormat ) );
        //kdDebug() << "KoTextObject::applyStyle currentFormat=" << currentFormat << " " << currentFormat->key() << endl;
    }

    //resize all variables after applying the style
    QPtrListIterator<Qt3::QTextCustomItem> cit( textdoc->allCustomItems() );
    for ( ; cit.current() ; ++cit )
        static_cast<KoTextCustomItem *>( cit.current() )->resize();


    if ( interactive )
    {
        setLastFormattedParag( firstParag );
        formatMore();
        emit repaintChanged( this );
        emit updateUI( true );
        if ( createUndoRedo )
            emit newCommand( macroCmd );
        emit showCursor();
    }

    undoRedoInfo.clear();
}

void KoTextObject::applyStyleChange( KoStyle * changedStyle, int paragLayoutChanged, int formatChanged )
{
    // TODO: if we are concerned about performance when updating many styles,
    // We could make a single call to this method, with a QMap<KoStyle *, struct holding flags>
    // in order to iterate over all paragraphs only once.

    /*kdDebug(32001) << "KoTextObject::applyStyleChange " << changedStyle->name()
                     << " paragLayoutChanged=" << paragLayoutChanged
                     << " formatChanged=" << formatChanged
                     << endl;*/

    KoTextDocument * textdoc = textDocument();
    KoTextParag *p = static_cast<KoTextParag *>(textdoc->firstParag());
    while ( p ) {
        if ( p->style() == changedStyle )
        {
            if ( paragLayoutChanged == -1 || formatChanged == -1 ) // Style has been deleted
            {
                p->setStyle( m_defaultStyle ); // keeps current formatting
                // TODO, make this undoable somehow
            }
            else
            {
                // Apply this style again, to get the changes
                QTextCursor cursor( textdoc );
                cursor.setParag( p );
                cursor.setIndex( 0 );
                kdDebug() << "KoTextObject::applyStyleChange applying to paragraph " << p << " " << p->paragId() << endl;
#if 0
                KoStyle styleApplied=*style;
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_BORDER) == 0)
                {
                    styleApplied.paragLayout().leftBorder=p->leftBorder();
                    styleApplied.paragLayout().rightBorder=p->rightBorder();
                    styleApplied.paragLayout().topBorder=p->topBorder();
                    styleApplied.paragLayout().bottomBorder=p->bottomBorder();
                }
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_ALIGN )==0)
                {
                    styleApplied.setAlign(p->alignment());
                }
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_NUMBERING)==0 )
                {
                    styleApplied.paragLayout().counter=*(p->counter());
                }
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_COLOR)==0 )
                {
                    styleApplied.format().setColor(p->paragFormat()->color());
                }
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_TABS)==0 )
                {
                    styleApplied.paragLayout().setTabList(p->tabList());
                }
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_INDENT)==0 )
                {
                    styleApplied.paragLayout().lineSpacing=p->kwLineSpacing();
                    styleApplied.paragLayout().margins[QStyleSheetItem::MarginLeft]=p->margin(QStyleSheetItem::MarginLeft);
                    styleApplied.paragLayout().margins[QStyleSheetItem::MarginRight]=p->margin(QStyleSheetItem::MarginRight);
                    styleApplied.paragLayout().margins[QStyleSheetItem::MarginFirstLine]=p->margin(QStyleSheetItem::MarginFirstLine);
                    styleApplied.paragLayout().margins[QStyleSheetItem::MarginBottom]=p->margin(QStyleSheetItem::MarginBottom);
                    styleApplied.paragLayout().margins[QStyleSheetItem::MarginTop]=p->margin(QStyleSheetItem::MarginTop);
                }
#endif
                applyStyle( &cursor, changedStyle,
                            -1, // A selection we can't possibly have
                            paragLayoutChanged, formatChanged,
                            false, false ); // don't create undo/redo, not interactive
            }
        }
        p = static_cast<KoTextParag *>(p->next());
    }
    setLastFormattedParag( textdoc->firstParag() );
    formatMore();
    emit repaintChanged( this );
    emit updateUI( true );
}

KCommand * KoTextObject::setFormatCommand( QTextCursor * cursor, KoTextFormat * & currentFormat, KoTextFormat *format, int flags, bool zoomFont, int selectionId )
{
    KCommand *ret = 0;
    KoTextDocument * textdoc = textDocument();
    Q_ASSERT( currentFormat );
    bool newFormat = ( currentFormat && currentFormat->key() != format->key() );
    if ( newFormat )
    {
        int origFontSize = 0;
        if ( zoomFont ) // The format has a user-specified font (e.g. setting a style, or a new font size)
        {
            origFontSize = format->font().pointSize();
            format->setPointSize( zoomedFontSize( origFontSize ) );
            kdDebug(32001) << "KoTextObject::setFormatCommand format " << format->key() << " zoomed from " << origFontSize << " to " << format->font().pointSizeFloat() << endl;
        }
        // Find format in collection
        currentFormat->removeRef();
        currentFormat = static_cast<KoTextFormat *>( textdoc->formatCollection()->format( format ) );
        if ( currentFormat->isMisspelled() ) {
            currentFormat->removeRef();
            currentFormat = static_cast<KoTextFormat *>( textdoc->formatCollection()->format( currentFormat->font(), currentFormat->color() ) );
        }

    }

    if ( textdoc->hasSelection( selectionId ) ) {
        emit hideCursor();
        QTextCursor c1 = textdoc->selectionStartCursor( selectionId );
        QTextCursor c2 = textdoc->selectionEndCursor( selectionId );
        undoRedoInfo.clear();
        int id = c1.parag()->paragId();
        int index = c1.index();
        int eid = c2.parag()->paragId();
        int eindex = c2.index();
        readFormats( c1, c2 ); // read previous formatting info
        //kdDebug(32001) << "KoTextObject::setFormatCommand undoredo info done" << endl;
        textdoc->setFormat( selectionId, format, flags );
        if ( !undoRedoInfo.customItemsMap.isEmpty() )
        {
            // Some custom items (e.g. variables) depend on the format
            CustomItemsMap::Iterator it = undoRedoInfo.customItemsMap.begin();
            for ( ; it != undoRedoInfo.customItemsMap.end(); ++it )
                it.data()->resize();
        }
        KoTextFormatCommand *cmd = new KoTextFormatCommand(
            textdoc, id, index, eid, eindex, undoRedoInfo.text.rawData(),
            format, flags );
        textdoc->addCommand( cmd );
        ret = new KoTextCommand( this, /*cmd, */i18n("Format text") );
        undoRedoInfo.clear();
        setLastFormattedParag( c1.parag() );
        formatMore();
        emit repaintChanged( this );
        emit showCursor();
    }
    //kdDebug(32001) << "KoTextObject::setFormatCommand currentFormat:" << currentFormat->key() << " new format:" << format->key() << endl;
    if ( newFormat ) {
        emit showCurrentFormat();
        //kdDebug(32001) << "KoTextObject::setFormatCommand index=" << cursor->index() << " length-1=" << cursor->parag()->length() - 1 << endl;
        if ( cursor && cursor->index() == cursor->parag()->length() - 1 ) {
            currentFormat->addRef();
            cursor->parag()->string()->setFormat( cursor->index(), currentFormat, TRUE );
            if ( cursor->parag()->length() == 1 ) {
                currentFormat->addRef();
                cursor->parag()->setFormat( currentFormat );
                cursor->parag()->invalidate(0);
                cursor->parag()->format();
                emit repaintChanged( this );
            }
        }
    }
    return ret;
}

void KoTextObject::setFormat( QTextCursor * cursor, KoTextFormat * & currentFormat, KoTextFormat *format, int flags, bool zoomFont )
{
    KCommand *cmd = setFormatCommand( cursor, currentFormat, format, flags, zoomFont );
    if (cmd)
        emit newCommand( cmd );
}

void KoTextObject::emitNewCommand(KCommand *cmd)
{
    if(cmd)
        emit newCommand( cmd );
}

KCommand *KoTextObject::setCounterCommand( QTextCursor * cursor, const KoParagCounter & counter, int selectionId  )
{
    KoTextDocument * textdoc = textDocument();
    const KoParagCounter * curCounter = 0L;
    if(cursor)
        curCounter=static_cast<KoTextParag*>(cursor->parag())->counter();
    if ( !textdoc->hasSelection( selectionId ) &&
         curCounter && counter == *curCounter )
        return 0L;
    emit hideCursor();
    if(cursor)
        storeParagUndoRedoInfo( cursor );
    if ( !textdoc->hasSelection( selectionId ) && cursor) {
        static_cast<KoTextParag*>(cursor->parag())->setCounter( counter );
        setLastFormattedParag( cursor->parag() );
    } else {
        Qt3::QTextParag *start = textdoc->selectionStart( selectionId );
        Qt3::QTextParag *end = textdoc->selectionEnd( selectionId );
        // Special hack for BR25742, don't apply bullet to last empty parag of the selection
        if ( start != end && end->length() <= 1 )
        {
            end = end->prev();
            undoRedoInfo.eid = end->paragId();
        }
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KoTextParag*>(start)->setCounter( counter );
    }
    formatMore();
    emit repaintChanged( this );
    if ( !undoRedoInfo.newParagLayout.counter )
        undoRedoInfo.newParagLayout.counter = new KoParagCounter;
    *undoRedoInfo.newParagLayout.counter = counter;
    KoTextParagCommand *cmd = new KoTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::BulletNumber );
    textdoc->addCommand( cmd );

    undoRedoInfo.clear(); // type is still Invalid -> no command created
    emit showCursor();
    emit updateUI( true );
    return new KoTextCommand( this, /*cmd, */i18n("Change list type") );
}

KCommand * KoTextObject::setAlignCommand( QTextCursor * cursor, int align , int selectionId )
{
    KoTextDocument * textdoc = textDocument();
    if ( !textdoc->hasSelection( selectionId ) && cursor &&
         cursor->parag()->alignment() == align )
        return 0L; // No change needed.

    emit hideCursor();
    if(cursor)
        storeParagUndoRedoInfo( cursor );
    if ( !textdoc->hasSelection( selectionId ) &&cursor ) {
        static_cast<KoTextParag *>(cursor->parag())->setAlign(align);
        setLastFormattedParag( cursor->parag() );
    }
    else
    {
        Qt3::QTextParag *start = textDocument()->selectionStart( selectionId );
        Qt3::QTextParag *end = textDocument()->selectionEnd( selectionId  );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KoTextParag *>(start)->setAlign(align);
    }
    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.alignment = align;
    KoTextParagCommand *cmd = new KoTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::Alignment );
    textdoc->addCommand( cmd );
    undoRedoInfo.clear(); // type is still Invalid -> no command created
    emit showCursor();
    emit updateUI( true );
    return new KoTextCommand( this, /*cmd, */i18n("Change Alignment") );
}

KCommand * KoTextObject::setMarginCommand( QTextCursor * cursor, QStyleSheetItem::Margin m, double margin , int selectionId ) {
    KoTextDocument * textdoc = textDocument();
    //kdDebug(32001) << "KoTextObject::setMargin " << m << " to value " << margin << endl;
    //kdDebug(32001) << "Current margin is " << static_cast<KoTextParag *>(cursor->parag())->margin(m) << endl;
    if ( !textdoc->hasSelection( selectionId ) && cursor &&
         static_cast<KoTextParag *>(cursor->parag())->margin(m) == margin )
        return 0L; // No change needed.

    emit hideCursor();
    if( cursor)
        storeParagUndoRedoInfo( cursor );
    if ( !textdoc->hasSelection( selectionId )&&cursor ) {
        static_cast<KoTextParag *>(cursor->parag())->setMargin(m, margin);
        setLastFormattedParag( cursor->parag() );
    }
    else
    {
        Qt3::QTextParag *start = textDocument()->selectionStart( selectionId );
        Qt3::QTextParag *end = textDocument()->selectionEnd( selectionId );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KoTextParag *>(start)->setMargin(m, margin);
    }
    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.margins[m] = margin;
    KoTextParagCommand *cmd = new KoTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::Margins, m );
    textdoc->addCommand( cmd );
    QString name;
    if ( m == QStyleSheetItem::MarginFirstLine )
        name = i18n("Change First Line Indent");
    else if ( m == QStyleSheetItem::MarginLeft || m == QStyleSheetItem::MarginRight )
        name = i18n("Change Indent");
    else
        name = i18n("Change Paragraph Spacing");
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI( true );
    return  new KoTextCommand( this, /*cmd, */name );
}

KCommand * KoTextObject::setLineSpacingCommand( QTextCursor * cursor, double spacing, int selectionId )
{
    KoTextDocument * textdoc = textDocument();
    //kdDebug(32001) << "KoTextObject::setLineSpacing to value " << spacing << endl;
    //kdDebug(32001) << "Current spacing is " << static_cast<KoTextParag *>(cursor->parag())->kwLineSpacing() << endl;
    //kdDebug(32001) << "Comparison says " << ( static_cast<KoTextParag *>(cursor->parag())->kwLineSpacing() == spacing ) << endl;
    //kdDebug(32001) << "hasSelection " << textdoc->hasSelection( KoTextDocument::Standard ) << endl;
    if ( !textdoc->hasSelection( selectionId ) && cursor &&
         static_cast<KoTextParag *>(cursor->parag())->kwLineSpacing() == spacing )
        return 0L; // No change needed.

    emit hideCursor();
    if(cursor)
        storeParagUndoRedoInfo( cursor );
    if ( !textdoc->hasSelection( selectionId ) && cursor ) {
        static_cast<KoTextParag *>(cursor->parag())->setLineSpacing(spacing);
        setLastFormattedParag( cursor->parag() );
    }
    else
    {
        Qt3::QTextParag *start = textDocument()->selectionStart( selectionId );
        Qt3::QTextParag *end = textDocument()->selectionEnd( selectionId );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KoTextParag *>(start)->setLineSpacing(spacing);
    }
    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.lineSpacing = spacing;
    KoTextParagCommand *cmd = new KoTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::LineSpacing );
    textdoc->addCommand( cmd );

    undoRedoInfo.clear();
    emit showCursor();
    return new KoTextCommand( this, /*cmd, */i18n("Change Line Spacing") );
}


KCommand * KoTextObject::setBordersCommand( QTextCursor * cursor, const KoBorder& leftBorder, const KoBorder& rightBorder, const KoBorder& topBorder, const KoBorder& bottomBorder , int selectionId )
{
  KoTextDocument * textdoc = textDocument();
  if ( !textdoc->hasSelection( selectionId ) && cursor &&
       static_cast<KoTextParag *>(cursor->parag())->leftBorder() ==leftBorder &&
       static_cast<KoTextParag *>(cursor->parag())->rightBorder() ==rightBorder &&
       static_cast<KoTextParag *>(cursor->parag())->topBorder() ==topBorder &&
       static_cast<KoTextParag *>(cursor->parag())->bottomBorder() ==bottomBorder )
        return 0L; // No change needed.

    emit hideCursor();
    if(cursor)
        storeParagUndoRedoInfo( cursor );
    if ( !textdoc->hasSelection( selectionId ) ) {
      static_cast<KoTextParag *>(cursor->parag())->setLeftBorder(leftBorder);
      static_cast<KoTextParag *>(cursor->parag())->setRightBorder(rightBorder);
      static_cast<KoTextParag *>(cursor->parag())->setBottomBorder(bottomBorder);
      static_cast<KoTextParag *>(cursor->parag())->setTopBorder(topBorder);
      setLastFormattedParag( cursor->parag() );
    }
    else
    {
        Qt3::QTextParag *start = textDocument()->selectionStart( selectionId );
        Qt3::QTextParag *end = textDocument()->selectionEnd( selectionId );
        setLastFormattedParag( start );
        KoBorder tmpBorder;
        tmpBorder.ptWidth=0;
        for ( ; start && start != end->next() ; start = start->next() )
          {
            static_cast<KoTextParag *>(start)->setLeftBorder(leftBorder);
            static_cast<KoTextParag *>(start)->setRightBorder(rightBorder);
            //remove border
            static_cast<KoTextParag *>(start)->setTopBorder(tmpBorder);
            static_cast<KoTextParag *>(start)->setBottomBorder(tmpBorder);
          }
        static_cast<KoTextParag *>(end)->setBottomBorder(bottomBorder);
        static_cast<KoTextParag *>(textDocument()->selectionStart( selectionId ))->setTopBorder(topBorder);
    }
    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.leftBorder=leftBorder;
    undoRedoInfo.newParagLayout.rightBorder=rightBorder;
    undoRedoInfo.newParagLayout.topBorder=topBorder;
    undoRedoInfo.newParagLayout.bottomBorder=bottomBorder;

    KoTextParagCommand *cmd = new KoTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::Borders);
    textdoc->addCommand( cmd );

    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI( true );
    return new KoTextCommand( this, /*cmd, */i18n("Change Borders") );
}


KCommand * KoTextObject::setTabListCommand( QTextCursor * cursor, const KoTabulatorList &tabList, int selectionId  )
{
    KoTextDocument * textdoc = textDocument();
    if ( !textdoc->hasSelection( selectionId ) && cursor &&
         static_cast<KoTextParag *>(cursor->parag())->tabList() == tabList )
        return 0L; // No change needed.

    emit hideCursor();
    if(cursor)
        storeParagUndoRedoInfo( cursor );

    if ( !textdoc->hasSelection( selectionId ) && cursor ) {
        static_cast<KoTextParag *>(cursor->parag())->setTabList( tabList );
        setLastFormattedParag( cursor->parag() );
    }
    else
    {
        Qt3::QTextParag *start = textDocument()->selectionStart( selectionId );
        Qt3::QTextParag *end = textDocument()->selectionEnd( selectionId );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KoTextParag *>(start)->setTabList( tabList );
    }

    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.setTabList( tabList );
    KoTextParagCommand *cmd = new KoTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::Tabulator);
    textdoc->addCommand( cmd );
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI( true );
    return new KoTextCommand( this, /*cmd, */i18n("Change Tabulator") );
}

KCommand * KoTextObject::setShadowCommand( QTextCursor * cursor,double dist, short int direction, const QColor &col,int selectionId )
{
    KoTextDocument * textdoc = textDocument();
    if ( !textdoc->hasSelection( selectionId ) && cursor &&
         static_cast<KoTextParag *>(cursor->parag())->shadowColor() == col &&
         static_cast<KoTextParag *>(cursor->parag())->shadowDirection() == direction &&
         static_cast<KoTextParag *>(cursor->parag())->shadowDistance() == dist )
        return 0L; // No change needed.

    emit hideCursor();
    if(cursor)
        storeParagUndoRedoInfo( cursor );

    if ( !textdoc->hasSelection( selectionId ) && cursor ) {
        static_cast<KoTextParag *>(cursor->parag())->setShadow( dist, direction, col );
        setLastFormattedParag( cursor->parag() );
    }
    else
    {
        Qt3::QTextParag *start = textDocument()->selectionStart( selectionId );
        Qt3::QTextParag *end = textDocument()->selectionEnd( selectionId );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KoTextParag *>(start)->setShadow( dist, direction, col );
    }

    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.shadowDistance=dist;
    undoRedoInfo.newParagLayout.shadowColor=col;
    undoRedoInfo.newParagLayout.shadowDirection=direction;
    KoTextParagCommand *cmd = new KoTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::Shadow);
    textdoc->addCommand( cmd );
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI( true );
    return new KoTextCommand( this, /*cmd, */i18n("Change Shadow") );
}


void KoTextObject::removeSelectedText( QTextCursor * cursor, int selectionId, const QString & cmdName )
{
    KoTextDocument * textdoc = textDocument();
    emit hideCursor();
    checkUndoRedoInfo( cursor, UndoRedoInfo::RemoveSelected );
    if ( !undoRedoInfo.valid() ) {
        textdoc->selectionStart( selectionId, undoRedoInfo.id, undoRedoInfo.index );
        undoRedoInfo.text = QString::null;
        newPlaceHolderCommand( cmdName.isNull() ? i18n("Remove Selected Text") : cmdName );
    }
    QTextCursor c1 = textdoc->selectionStartCursor( selectionId );
    QTextCursor c2 = textdoc->selectionEndCursor( selectionId );
    readFormats( c1, c2, true, true );
    kdDebug() << "KoTextObject::removeSelectedText text=" << undoRedoInfo.text.toString() << endl;

    textdoc->removeSelectedText( selectionId, cursor );

    setLastFormattedParag( cursor->parag() );
    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit updateUI( true );
    emit showCursor();
    if(selectionId==KoTextDocument::Standard)
        selectionChangedNotify();
    undoRedoInfo.clear();
}

KCommand * KoTextObject::removeSelectedTextCommand( QTextCursor * cursor, int selectionId )
{
    undoRedoInfo.clear();
    textdoc->selectionStart( selectionId, undoRedoInfo.id, undoRedoInfo.index );

    QTextCursor c1 = textdoc->selectionStartCursor( selectionId );
    QTextCursor c2 = textdoc->selectionEndCursor( selectionId );
    readFormats( c1, c2, true, true );

    textdoc->removeSelectedText( selectionId, cursor );

    KMacroCommand *macroCmd = new KMacroCommand( i18n("Remove Selected Text") );

    QTextCommand *cmd = new KoTextDeleteCommand( textdoc, undoRedoInfo.id, undoRedoInfo.index,
                                                 undoRedoInfo.text.rawData(),
                                                 undoRedoInfo.customItemsMap,
                                                 undoRedoInfo.oldParagLayouts );
    textdoc->addCommand(cmd);
    macroCmd->addCommand(new KoTextCommand( this, /*cmd, */QString::null ));

    if(!undoRedoInfo.customItemsMap.isEmpty())
        undoRedoInfo.customItemsMap.deleteAll( macroCmd );

    undoRedoInfo.type = UndoRedoInfo::Invalid; // we don't want clear() to create a command
    undoRedoInfo.clear();

    return macroCmd;
}

KCommand* KoTextObject::replaceSelectionCommand( QTextCursor * cursor, const QString & replacement,
                                                   int selectionId, const QString & cmdName)
{
    emit hideCursor();
    KMacroCommand * macroCmd = new KMacroCommand( cmdName );

    // Remember formatting
    QTextCursor c1 = textdoc->selectionStartCursor( selectionId );
    QTextFormat * format = c1.parag()->at( c1.index() )->format();
    format->addRef();

    // Remove selected text
    macroCmd->addCommand( removeSelectedTextCommand( cursor, selectionId ) );

    // Insert replacement
    insert( cursor, static_cast<KoTextFormat *>(format),
            replacement, true, false, QString::null /* no place holder command */ );

    QTextCommand * cmd = new KoTextInsertCommand( textdoc, undoRedoInfo.id, undoRedoInfo.index,
                                                  undoRedoInfo.text.rawData(),
                                                  CustomItemsMap(), undoRedoInfo.oldParagLayouts );
    textdoc->addCommand( cmd );
    macroCmd->addCommand( new KoTextCommand( this, /*cmd, */QString::null ) );

    undoRedoInfo.type = UndoRedoInfo::Invalid; // we don't want clear() to create a command
    undoRedoInfo.clear();

    format->removeRef();


    setLastFormattedParag( c1.parag() );
    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit updateUI( true );
    emit showCursor();
    if(selectionId==KoTextDocument::Standard)
        selectionChangedNotify();
    return macroCmd;
}

KCommand * KoTextObject::insertParagraphCommand( QTextCursor *cursor )
{
    return replaceSelectionCommand( cursor, "\n", KoTextDocument::Standard, QString::null );
}

void KoTextObject::highlightPortion( Qt3::QTextParag * parag, int index, int length )
{
    if ( !m_highlightSelectionAdded )
    {
        textdoc->addSelection( HighlightSelection );
        textdoc->setSelectionColor( HighlightSelection,
                                    QApplication::palette().color( QPalette::Active, QColorGroup::Dark ) );
        textdoc->setInvertSelectionText( HighlightSelection, true );
        m_highlightSelectionAdded = true;
    }

    removeHighlight(); // remove previous highlighted selection
    QTextCursor cursor( textdoc );
    cursor.setParag( parag );
    cursor.setIndex( index );
    textdoc->setSelectionStart( HighlightSelection, &cursor );
    cursor.setIndex( index + length );
    textdoc->setSelectionEnd( HighlightSelection, &cursor );
    parag->setChanged( true );
    emit repaintChanged( this );
}

void KoTextObject::removeHighlight()
{
    if ( textdoc->hasSelection( HighlightSelection ) )
    {
        Qt3::QTextParag * oldParag = textdoc->selectionStart( HighlightSelection );
        oldParag->setChanged( true );
        textdoc->removeSelection( HighlightSelection );
    }
    emit repaintChanged( this );
}

void KoTextObject::selectAll( bool select )
{
    if ( !select )
        textDocument()->removeSelection( KoTextDocument::Standard );
    else
        textDocument()->selectAll( KoTextDocument::Standard );
    selectionChangedNotify();
}

void KoTextObject::selectionChangedNotify( bool enableActions /* = true */)
{
    emit repaintChanged( this );
    if ( enableActions )
        emit selectionChanged( hasSelection() );
}

void KoTextObject::setViewArea( QWidget* w, int maxY )
{
    m_mapViewAreas.replace( w, maxY );
}

void KoTextObject::setLastFormattedParag( Qt3::QTextParag *parag )
{
    if ( !m_lastFormatted || !parag || m_lastFormatted->paragId() >= parag->paragId() )
        m_lastFormatted = parag;
}

void KoTextObject::ensureFormatted( Qt3::QTextParag * parag, bool emitAfterFormatting /* = true */ )
{
    while ( !parag->isValid() )
    {
        if ( !m_lastFormatted || m_availableHeight == -1 )
            return; // formatMore will do nothing -> give up
        formatMore( emitAfterFormatting );
    }
}

void KoTextObject::formatMore( bool emitAfterFormatting /* = true */ )
{
    if ( ( !m_lastFormatted && d->afterFormattingEmitted )
         /* || !isVisible()*/ || m_availableHeight == -1 )
        return;

    if ( !textDocument()->lastParag() )
        return; // safety test

    int bottom = 0;
    if ( m_lastFormatted )
    {
        d->afterFormattingEmitted = false;
        int to = !sender() ? 2 : 20; // 20 when it comes from the formatTimer

        int viewsBottom = 0;
        QMapIterator<QWidget *, int> mapIt = m_mapViewAreas.begin();
        for ( ; mapIt != m_mapViewAreas.end() ; ++mapIt )
            viewsBottom = QMAX( viewsBottom, mapIt.data() );

#ifdef DEBUG_FORMAT_MORE
        kdDebug(32002) << "formatMore " << name()
                       << " lastFormatted id=" << m_lastFormatted->paragId()
                       << " lastFormatted's top=" << m_lastFormatted->rect().top()
                       << " lastFormatted's height=" << m_lastFormatted->rect().height()
                       << " to=" << to << " viewsBottom=" << viewsBottom
                       << " availableHeight=" << m_availableHeight << endl;
#endif
#ifdef TIMING_FORMAT
        if ( m_lastFormatted->prev() == 0 )
        {
            kdDebug(32002) << "formatMore " << name() << ". First parag -> starting timer" << endl;
            m_time.start();
        }
#endif

        // Stop if we have formatted everything or if we need more space
        // Otherwise, stop formatting after "to" paragraphs,
        // but make sure we format everything the views need
        int i;
        for ( i = 0;
              m_lastFormatted && bottom + m_lastFormatted->rect().height() <= m_availableHeight &&
                  ( i < to || bottom <= viewsBottom ) ; ++i )
        {
#ifdef DEBUG_FORMAT_MORE
            kdDebug(32002) << "formatMore formatting id=" << m_lastFormatted->paragId() << endl;
#endif
            m_lastFormatted->format();
            bottom = m_lastFormatted->rect().top() + m_lastFormatted->rect().height();
#ifdef DEBUG_FORMAT_MORE
            kdDebug() << "formatMore(inside) top=" << m_lastFormatted->rect().top()
                      << " height=" << m_lastFormatted->rect().height()
                      << " bottom=" << bottom << " m_lastFormatted(next parag) = " << m_lastFormatted->next() << endl;
#endif
            if (!m_lastFormatted->isValid())
                kdWarning() << "PARAGRAPH " << m_lastFormatted->paragId() << " STILL INVALID AFTER FORMATTING" << endl;
            m_lastFormatted = m_lastFormatted->next();
        }
    }
    else // formatting was done previously, but not emit afterFormatting
    {
        QRect rect = textDocument()->lastParag()->rect();
        bottom = rect.top() + rect.height();
    }
#ifdef DEBUG_FORMAT_MORE
    kdDebug(32002) << "formatMore finished formatting. "
                   << " bottom=" << bottom
                   << " m_lastFormatted=" << m_lastFormatted
                   << endl;
#endif

    if ( emitAfterFormatting )
    {
        d->afterFormattingEmitted = true;
        bool abort = false;
        emit afterFormatting( bottom, m_lastFormatted, &abort );
        if ( abort )
            return;
    }

    // Now let's see when we'll need to get back here.
    if ( m_lastFormatted )
    {
        formatTimer->start( interval, TRUE );
#ifdef DEBUG_FORMAT_MORE
        kdDebug(32002) << "formatMore: will have to format more. formatTimer->start with interval=" << interval << endl;
#endif
    }
    else
    {
        interval = QMAX( 0, interval );
#ifdef DEBUG_FORMAT_MORE
        kdDebug(32002) << "formatMore: all formatted interval=" << interval << endl;
#endif
#ifdef TIMING_FORMAT
        //if ( frameSetInfo() == FI_BODY )
        kdDebug(32002) << "formatMore: " << name() << " all formatted. Took "
                       << (double)(m_time.elapsed()) / 1000 << " seconds." << endl;
#endif
    }
}

void KoTextObject::doChangeInterval()
{
#ifdef DEBUG_FORMAT_MORE
    kdDebug() << "KoTextObject::doChangeInterval back to interval=0" << endl;
#endif
    interval = 0;
}

void KoTextObject::typingStarted()
{
#ifdef DEBUG_FORMAT_MORE
    kdDebug() << "KoTextObject::typingStarted" << endl;
#endif
    changeIntervalTimer->stop();
    interval = 10;
}

void KoTextObject::typingDone()
{
    changeIntervalTimer->start( 100, TRUE );
}

bool KoTextObject::textSelectedIsAnLink(QString & href)
{
    QTextCursor c1 = textDocument()->selectionStartCursor( KoTextDocument::Standard );
    QTextCursor c2 = textDocument()->selectionEndCursor( KoTextDocument::Standard );
    //a link should be in one parag.
    if(c1.parag()!=c2.parag())
        return false;

    bool isAnchor=false;
    for(int i = c1.index();i<= c2.index();i++)
    {

       KoTextStringChar * ch = c1.parag()->at( i );
       if(ch->format()->isAnchor())
           isAnchor=true;
       else
           return false;
       href=ch->format()->anchorHref();
    }
    return true;
}


KCommand *KoTextObject::changeCaseOfTextParag(int cursorPosStart, int cursorPosEnd,KoChangeCaseDia::TypeOfCase _type,QTextCursor *cursor, KoTextParag *parag)
{
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Change case") );
    KoTextFormat *curFormat = static_cast<KoTextParag *>(parag)->paragraphFormat();
    QString text = parag->string()->toString().mid(cursorPosStart , cursorPosEnd - cursorPosStart );
    QString repl;
    int posStart=cursorPosStart;
    int posEnd=cursorPosStart;
    QTextCursor c1( textDocument() );
    QTextCursor c2( textDocument() );
    for ( int i = cursorPosStart; i < cursorPosEnd; ++i )
    {
        KoTextStringChar & ch = *(parag->at(i));
        KoTextFormat * newFormat = static_cast<KoTextFormat *>( ch.format() );
        if( ch.isCustom())
        {
            posEnd=i;
            c1.setParag(parag  );
            c1.setIndex( posStart );
            c2.setParag( parag );
            c2.setIndex( posEnd );

            repl=text.mid(posStart-cursorPosStart,posEnd-posStart);
            textDocument()->setSelectionStart( KoTextDocument::Temp, &c1 );
            textDocument()->setSelectionEnd( KoTextDocument::Temp, &c2 );
            macroCmd->addCommand(replaceSelectionCommand(
                                     cursor, textChangedCase(repl,_type),
                                     KoTextDocument::Temp, "" ));
            do
            {
                ++i;
            }
            while( parag->at(i)->isCustom() && i != cursorPosEnd);
            posStart=i;
            posEnd=i;
        }
        else
        {
            if ( newFormat != curFormat )
            {
                posEnd=i;
                c1.setParag(parag  );
                c1.setIndex( posStart );
                c2.setParag( parag );
                c2.setIndex( posEnd );

                repl=text.mid(posStart-cursorPosStart,posEnd-posStart);
                textDocument()->setSelectionStart( KoTextDocument::Temp, &c1 );
                textDocument()->setSelectionEnd( KoTextDocument::Temp, &c2 );
                macroCmd->addCommand(replaceSelectionCommand(
                                         cursor, textChangedCase(repl,_type),
                                         KoTextDocument::Temp, "" ));
                posStart=i;
                posEnd=i;
                curFormat = newFormat;
            }
        }
    }
    //change last word
    c1.setParag(parag  );
    c1.setIndex( posStart );
    c2.setParag(parag );
    c2.setIndex( cursorPosEnd );

    textDocument()->setSelectionStart( KoTextDocument::Temp, &c1 );
    textDocument()->setSelectionEnd( KoTextDocument::Temp, &c2 );
    repl=text.mid(posStart-cursorPosStart,cursorPosEnd-posStart);
    macroCmd->addCommand(replaceSelectionCommand(
                             cursor, textChangedCase(repl,_type),
                             KoTextDocument::Temp, "" ));
    return macroCmd;

}

void KoTextObject::changeCaseOfText(QTextCursor *cursor,KoChangeCaseDia::TypeOfCase _type)
{
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Change case") );

    QTextCursor start = textDocument()->selectionStartCursor( KoTextDocument::Standard );
    QTextCursor end = textDocument()->selectionEndCursor( KoTextDocument::Standard );

    if ( start.parag() == end.parag() )
    {
        macroCmd->addCommand(changeCaseOfTextParag(start.index(),end.index() , _type,cursor, static_cast<KoTextParag*>(start.parag()) ));
    }
    else
    {
        macroCmd->addCommand(changeCaseOfTextParag(start.index(), start.parag()->length() - 1 - start.index(), _type,cursor, static_cast<KoTextParag*>(start.parag()) ));
        Qt3::QTextParag *p = start.parag()->next();
        while ( p && p != end.parag() )
        {
            macroCmd->addCommand(changeCaseOfTextParag(0,p->length()-1 , _type,cursor, static_cast<KoTextParag*>(p) ));
            p = p->next();
        }
        macroCmd->addCommand(changeCaseOfTextParag(0,end.index() , _type,cursor, static_cast<KoTextParag*>(end.parag()) ));
    }

    if (macroCmd)
        emit newCommand( macroCmd );
}

QString KoTextObject::textChangedCase(const QString _text,KoChangeCaseDia::TypeOfCase _type)
{
    QString text(_text);
    switch(_type)
    {
        case KoChangeCaseDia::UpperCase:
            text=text.upper();
            break;
        case KoChangeCaseDia::LowerCase:
            text=text.lower();
            break;
        case KoChangeCaseDia::TitleCase:
            for(uint i=0;i<text.length();i++)
            {
                if(text.at(i)!=' ')
                {
                    QChar prev = text.at(QMAX(i-1,0));
                    if(i==0 || prev  == ' ' || prev == '\n' || prev == '\t')
                        text=text.replace(i, 1, text.at(i).upper() );
                    else
                        text=text.replace(i, 1, text.at(i).lower() );
                }
            }
            break;
        case KoChangeCaseDia::ToggleCase:
            for(uint i=0;i<text.length();i++)
            {
                QString repl=QString(text.at(i));
                if(text.at(i)!=text.at(i).upper())
                    repl=repl.upper();
                else if(text.at(i).lower()!=text.at(i))
                    repl=repl.lower();
                text=text.replace(i, 1, repl );
            }
            break;
        default:
            kdDebug()<<"Error in changeCaseOfText !\n";
            break;

    }
    return text;
}

KoTextFormat * KoTextObject::currentFormat() const
{
    // We use the formatting of the very first character
    // Should we use a style instead, maybe ?
    KoTextStringChar *ch = textDocument()->firstParag()->at( 0 );
    return static_cast<KoTextFormat *>(ch->format());
}

const KoParagLayout * KoTextObject::currentParagLayoutFormat() const
{
    KoTextParag * parag=static_cast<KoTextParag *>(textDocument()->firstParag());
    return &(parag->paragLayout());
}

void KoTextObject::setParagLayoutFormat( KoParagLayout *newLayout,int flags,int marginIndex)
{
    KoTextDocument *textdoc = textDocument();
    textdoc->selectAll( KoTextDocument::Temp );
    QTextCursor *cursor = new QTextCursor( textDocument() );
    KCommand *cmd =0L;
    KoParagCounter c;
    if(newLayout->counter)
        c=*newLayout->counter;
    switch(flags)
    {
    case KoParagLayout::Alignment:
    {
        cmd = setAlignCommand( cursor, newLayout->alignment ,KoTextDocument::Temp );
        break;
    }
    case KoParagLayout::Margins:
        cmd= setMarginCommand( cursor, (QStyleSheetItem::Margin)marginIndex, newLayout->margins[marginIndex] ,KoTextDocument::Temp );
        break;
    case KoParagLayout::Tabulator:
        cmd= setTabListCommand( cursor, newLayout->tabList(),KoTextDocument::Temp  );
        break;
    case KoParagLayout::BulletNumber:
        cmd= setCounterCommand( cursor, c,KoTextDocument::Temp   );
        break;
    default:
        break;
    }
    textdoc->removeSelection( KoTextDocument::Temp );
    if (cmd)
        emit newCommand( cmd );
}

void KoTextObject::setFormat( KoTextFormat * newFormat, int flags, bool zoomFont )
{
    // This pointer will be modified by setFormatCommand, so we need a holder,
    // but in fact we don't care about the modifications here.
    KoTextFormat* curFormat = currentFormat();
    // This version of setFormat works on the whole textobject - we use the Temp selection for that
    KoTextDocument *textdoc = textDocument();
    textdoc->selectAll( KoTextDocument::Temp );
    KCommand *cmd = setFormatCommand( 0L, curFormat, newFormat,
                                      flags, zoomFont, KoTextDocument::Temp );
    textdoc->removeSelection( KoTextDocument::Temp );
    if (cmd)
        emit newCommand( cmd );

    KoTextFormat format = *currentFormat();
    format.setPointSize( docFontSize( currentFormat() ) ); // "unzoom" the font size
    emit showFormatObject(format);
}

#ifndef NDEBUG
void KoTextObject::printRTDebug(int info)
{
    KoTextDocument *textdoc = textDocument();
    for (Qt3::QTextParag * parag = textdoc->firstParag(); parag ; parag = parag->next())
    {
        KoTextParag * p = static_cast<KoTextParag *>(parag);
        p->printRTDebug( info );
    }
    if ( info == 1 )
        textdoc->formatCollection()->debug();
}
#endif

void KoTextFormatInterface::setBold(bool on) {
    KoTextFormat format( *currentFormat() );
    format.setBold( on );
    setFormat( &format, QTextFormat::Bold );
}

void KoTextFormatInterface::setItalic( bool on ) {
    KoTextFormat format( *currentFormat() );
    format.setItalic( on );
    setFormat( &format, QTextFormat::Italic );
}

void KoTextFormatInterface::setUnderline( bool on ) {
    KoTextFormat format( *currentFormat() );
    format.setUnderline( on );
    setFormat( &format, QTextFormat::Underline );
}

void KoTextFormatInterface::setStrikeOut( bool on ) {
    KoTextFormat format( *currentFormat() );
    format.setStrikeOut( on );
    setFormat( &format, KoTextFormat::StrikeOut );
}

void KoTextFormatInterface::setTextBackgroundColor(const QColor & col) {
    KoTextFormat format( *currentFormat() );
    format.setTextBackgroundColor( col );
    setFormat( &format, KoTextFormat::TextBackgroundColor );
}

QColor KoTextFormatInterface::textBackgroundColor() const {
    return currentFormat()->textBackgroundColor();
}

QColor KoTextFormatInterface::textColor() const {
    return currentFormat()->color();
}

QFont KoTextFormatInterface::textFont() const {
    QFont fn( currentFormat()->font() );
    // "unzoom" the font size
    fn.setPointSize( static_cast<int>( KoTextZoomHandler::layoutUnitPtToPt( fn.pointSize() ) ) );
    return fn;
}

QString KoTextFormatInterface::textFontFamily()const {
    return currentFormat()->font().family();
}

void KoTextFormatInterface::setPointSize( int s ){
    KoTextFormat format( *currentFormat() );
    format.setPointSize( s );
    setFormat( &format, QTextFormat::Size, true /* zoom the font size */ );
}

void KoTextFormatInterface::setFamily(const QString &font){
    KoTextFormat format( *currentFormat() );
    format.setFamily( font );
    setFormat( &format, QTextFormat::Family );
}

void KoTextFormatInterface::setFont( const QFont &font, bool _subscript, bool _superscript, const QColor &col,const QColor &backGroundColor, int flags )
{
    KoTextFormat format( *currentFormat() );
    format.setFont( font );
    format.setColor( col );
    format.setTextBackgroundColor(backGroundColor);
    if(!_subscript)
    {
        if(!_superscript)
            format.setVAlign(QTextFormat::AlignNormal);
        else
            format.setVAlign(QTextFormat::AlignSuperScript);
    }
    else
        format.setVAlign(QTextFormat::AlignSubScript);

    setFormat( &format, flags, true /* zoom the font size */);
}

void KoTextFormatInterface::setTextColor(const QColor &color) {
    KoTextFormat format( *currentFormat() );
    format.setColor( color );
    setFormat( &format, QTextFormat::Color );
}

void KoTextFormatInterface::setTextSubScript(bool on)
{
    KoTextFormat format( *currentFormat() );
    if(!on)
        format.setVAlign(QTextFormat::AlignNormal);
    else
        format.setVAlign(QTextFormat::AlignSubScript);
    setFormat( &format, QTextFormat::VAlign );
}

void KoTextFormatInterface::setTextSuperScript(bool on)
{
    KoTextFormat format( *currentFormat() );
    if(!on)
        format.setVAlign(QTextFormat::AlignNormal);
    else
        format.setVAlign(QTextFormat::AlignSuperScript);
    setFormat( &format, QTextFormat::VAlign );
}

void KoTextFormatInterface::setDefaultFormat() {
    QTextFormatCollection * coll = currentFormat()->parent();
    Q_ASSERT(coll);
    if(coll)
    {
        KoTextFormat * format = static_cast<KoTextFormat *>(coll->defaultFormat());
        setFormat( format, QTextFormat::Format );
    }
}

void KoTextFormatInterface::setAlign(int align)
{
    KoParagLayout format( *currentParagLayoutFormat() );
    format.alignment=align;
    setParagLayoutFormat(&format,KoParagLayout::Alignment);
}

void KoTextFormatInterface::setMargin(QStyleSheetItem::Margin m, double margin)
{
    KoParagLayout format( *currentParagLayoutFormat() );
    format.margins[m]=margin;
    setParagLayoutFormat(&format,KoParagLayout::Margins,(int)m);
}

void KoTextFormatInterface::setTabList(const KoTabulatorList & tabList )
{
    KoParagLayout format( *currentParagLayoutFormat() );
    format.setTabList(tabList);
    setParagLayoutFormat(&format,KoParagLayout::Tabulator);
}

void KoTextFormatInterface::setCounter(const KoParagCounter & counter )
{
    KoParagLayout format( *currentParagLayoutFormat() );
    if(!format.counter)
        format.counter = new KoParagCounter;
    *format.counter= counter;
    setParagLayoutFormat(&format,KoParagLayout::BulletNumber);
}

#include "kotextobject.moc"
