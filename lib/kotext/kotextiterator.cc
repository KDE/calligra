/* This file is part of the KDE project
   Copyright (C) 2002 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kotextiterator.h"
#include "kotextview.h"
#include <kfinddialog.h>
#include <kdebug.h>

//// TODO connect to kotextobject's destroyed() signal (QObject)
//// and same with the paragraph pointers (signal from kotextobject)
// QGuardedPtr sets to 0 but doesn't allow skipping properly.

void KoTextIterator::init( const QValueList<KoTextObject *> & lstObjects, KoTextView* textView, int options )
{
    Q_ASSERT( !lstObjects.isEmpty() );
    if ( textView )
        // Hmm textView->textObject() should be first in lstObjects!
        // by contract? or should we change order over here?
        Q_ASSERT( lstObjects.first() == textView->textObject() );

    m_lstObjects.clear();
    m_firstParag = 0;
    m_firstIndex = 0;
    m_options = options;

    // 'From Cursor' option
    if ( options & KFindDialog::FromCursor )
    {
        if ( textView ) {
            m_firstParag = textView->cursor()->parag();
            m_firstIndex = textView->cursor()->index();
        } else {
            // !? FromCursor option can't work
            m_options &= ~KFindDialog::FromCursor;
            kdWarning(32500) << "FromCursor specified, but no textview?" << endl;
        }
    } // no else here !

    bool forw = ! ( options & KFindDialog::FindBackwards );

    // 'Selected Text' option
    if ( textView && ( options & KFindDialog::SelectedText ) )
    {
        KoTextObject* textObj = lstObjects.first();
        KoTextCursor c1 = textObj->textDocument()->selectionStartCursor( KoTextDocument::Standard );
        KoTextCursor c2 = textObj->textDocument()->selectionEndCursor( KoTextDocument::Standard );
        if ( !m_firstParag ) // not from cursor
        {
            m_firstParag = forw ? c1.parag() : c2.parag();
            m_firstIndex = forw ? c1.index() : c2.index();
        }
        m_lastParag = forw ? c2.parag() : c1.parag();
        m_lastIndex = forw ? c2.index() : c1.index();
        // Find in the selection only -> only one textobject
        m_lstObjects.append( textObj );
        m_currentTextObj = m_lstObjects.begin();
    }
    else
    {
        // Not "selected text" -> loop through all textobjects
        m_lstObjects = lstObjects;
        KoTextParag* firstParag = m_lstObjects.first()->textDocument()->firstParag();
        int firstIndex = 0;
        KoTextParag* lastParag = m_lstObjects.last()->textDocument()->lastParag();
        int lastIndex = lastParag->length()-1;
        if ( m_firstParag )
        {
            // from cursor. So we have to start from the textview's object, even when going backwards
            m_currentTextObj = m_lstObjects.begin();
            // so we end at the last textobject's end - which end depends on the direction
            if ( forw ) {
                m_lastParag = lastParag;
                m_lastIndex = lastIndex;
            } else {
                m_lastParag = m_lstObjects.last()->textDocument()->firstParag();
                m_lastIndex = 0;
            }
        }
        else // not from cursor.
        {
            m_firstParag = forw ? firstParag : lastParag;
            m_firstIndex = forw ? firstIndex : lastIndex;
            m_lastParag = forw ? lastParag : firstParag;
            m_lastIndex = forw ? lastIndex : firstIndex;
            m_currentTextObj = forw ? m_lstObjects.begin() : m_lstObjects.fromLast();
        }
    }

    assert( *m_currentTextObj ); // all branches set it
    assert( m_firstParag );
    assert( m_lastParag );
    m_currentParag = m_firstParag;
    kdDebug(32500) << "KoTextIterator::init from(" << *m_currentTextObj << "," << m_firstParag->paragId() << ") - to(" << (forw?m_lstObjects.last():m_lstObjects.first()) << "," << m_lastParag->paragId() << "), " << m_lstObjects.count() << " textObjects." << endl;
}

void KoTextIterator::restart()
{
    m_currentParag = m_firstParag;
    bool forw = ! ( m_options & KFindDialog::FindBackwards );
    if ( (m_options & KFindDialog::FromCursor) || forw )
        m_currentTextObj = m_lstObjects.begin();
    else
        m_currentTextObj = m_lstObjects.fromLast();
}

// Go to next paragraph that we must iterate over
void KoTextIterator::operator++()
{
    if ( !m_currentParag ) {
        kdDebug() << k_funcinfo << " called past the end" << endl;
        return;
    }
    if ( m_currentParag == m_lastParag ) {
        m_currentParag = 0L;
        return;
    }
    bool forw = ! ( m_options & KFindDialog::FindBackwards );
    KoTextParag* parag = forw ? m_currentParag->next() : m_currentParag->prev();
    if ( parag )
    {
        m_currentParag = parag;
    }
    else
    {
        // We need to go to the next textobject
        if ( forw ) {
            ++m_currentTextObj;
            if ( m_currentTextObj == m_lstObjects.end() )
                m_currentParag = 0L; // done
            else
                m_currentParag = (*m_currentTextObj)->textDocument()->firstParag();
        } else {
            if ( m_currentTextObj == m_lstObjects.begin() )
            {
                --m_currentTextObj;
                m_currentParag = (*m_currentTextObj)->textDocument()->lastParag();
            } else
                m_currentParag = 0L; // done
        }
    }
    kdDebug(32500) << "KoTextIterator++ (" << *m_currentTextObj << "," <<
        (m_currentParag ? m_currentParag->paragId() : 0) << ")" << endl;
}

bool KoTextIterator::atEnd() const
{
    // operator++ sets m_currentParag to 0 when it's done
    return m_currentParag == 0L;
}

int KoTextIterator::currentStartIndex() const
{
    return currentTextAndIndex().first;
}

QString KoTextIterator::currentText() const
{
    return currentTextAndIndex().second;
}

QPair<int, QString> KoTextIterator::currentTextAndIndex() const
{
    Q_ASSERT( m_currentParag );
    QString str = m_currentParag->string()->toString();
    str.truncate( str.length() - 1 ); // damn trailing space
    bool forw = ! ( m_options & KFindDialog::FindBackwards );
    if ( m_currentParag == m_firstParag )
    {
        if ( m_firstParag == m_lastParag ) // special case, needs truncating at both ends
            return qMakePair( m_firstIndex, str.mid( m_firstIndex, m_lastIndex - m_firstIndex ) );
        else
            return forw ? qMakePair( m_firstIndex, str.mid( m_firstIndex ) )
                        : qMakePair( 0, str.left( m_firstIndex ) );
    }
    if ( m_currentParag == m_lastParag )
    {
        return forw ? qMakePair( 0, str.left( m_lastIndex ) )
                    : qMakePair( m_lastIndex, str.mid( m_lastIndex ) );
    }
    // Not the first parag, nor the last, so we return it all
    return qMakePair( 0, str );
}

bool KoTextIterator::hasText() const
{
    // Same logic as currentTextAndIndex, but w/o calling it, to avoid all the string copying
    bool forw = ! ( m_options & KFindDialog::FindBackwards );
    int strLength = m_currentParag->string()->length() - 1;
    if ( m_currentParag == m_firstParag )
    {
        if ( m_firstParag == m_lastParag )
            return m_firstIndex < m_lastIndex;
        else
            return forw ? m_firstIndex < strLength
                        : m_firstIndex > 0;
    }
    if ( m_currentParag == m_lastParag )
        return forw ? m_lastIndex > 0
                    : m_lastIndex < strLength;
    return strLength > 0;
}

void KoTextIterator::setOptions( int options )
{
    if ( m_options != options )
    {
        bool wasBack = (m_options & KFindDialog::FindBackwards);
        bool isBack = (options & KFindDialog::FindBackwards);
        if ( wasBack != isBack )
        {
            qSwap( m_firstParag, m_lastParag );
            qSwap( m_firstIndex, m_lastIndex );
            if ( m_currentParag == 0 ) // done? -> reinit
            {
                kdDebug() << k_funcinfo << "was done -> reinit" << endl;
                restart();
            }
        }
        m_options = options;
    }
}
