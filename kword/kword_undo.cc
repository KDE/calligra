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

#include "kword_undo.h"
#include "kword_undo.moc"
#include "kword_doc.h"
#include "kword_frame.h"
#include "fc.h"

/******************************************************************/
/* Class: KWTextChangeCommand                                     */
/******************************************************************/

/*================================================================*/
void KWTextChangeCommand::execute()
{
    if ( parags.isEmpty() )
        return;

    QList<KWParag> old;
    old.setAutoDelete( false );

    KWParag *parag = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( frameset ) )->getFirstParag();

    KWParag *parag1 = 0L, *parag2 = 0L;

    while ( parag )
    {
        if ( parag->getParagName() == before )
            parag1 = parag;

        if ( parag->getParagName() == after )
            parag2 = parag;

        if ( parag1 && parag2 ) break;

        parag = parag->getNext();
    }

    if ( !parag1 && !parag2 )
        dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( frameset ) )->setFirstParag( parags.first() );

    else if ( !parag1 && parag2 )
    {
        parag = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( frameset ) )->getFirstParag();
        while ( parag != parag2 )
        {
            old.append( new KWParag( *parag ) );
            parag = parag->getNext();
        }
        dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( frameset ) )->setFirstParag( parags.first() );
        parag2->setPrev( parags.last() );
    }

    else if ( parag1 && !parag2 )
    {
        parag = parag1->getNext();
        while ( parag )
        {
            old.append( new KWParag( *parag ) );
            parag = parag->getNext();
        }
        parag1->setNext( parags.first() );
    }

    if ( parag1 && parag2 )
    {
        parag = parag1->getNext();
        while ( parag != parag2 )
        {
            old.append( new KWParag( *parag ) );
            parag = parag->getNext();
        }
        parag1->setNext( parags.first() );
        parag2->setPrev( parags.last() );
    }

    fc->setTextPos( textPos );

    parags.clear();
    parags = old;
    parags.setAutoDelete( false );
}

/*================================================================*/
void KWTextChangeCommand::unexecute()
{
    if ( parags.isEmpty() )
        return;

    QList<KWParag> old;
    old.setAutoDelete( false );

    KWParag *parag = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( frameset ) )->getFirstParag();

    KWParag *parag1 = 0L, *parag2 = 0L;

    while ( parag )
    {
        if ( parag->getParagName() == before )
            parag1 = parag;

        if ( parag->getParagName() == after )
            parag2 = parag;

        if ( parag1 && parag2 ) break;

        parag = parag->getNext();
    }

    if ( !parag1 && !parag2 )
    {
        dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( frameset ) )->setFirstParag( parags.first() );
        old.append( new KWParag( *dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( frameset ) )->getFirstParag() ) );
    }

    else if ( !parag1 && parag2 )
    {
        parag = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( frameset ) )->getFirstParag();
        while ( parag != parag2 )
        {
            old.append( new KWParag( *parag ) );
            parag = parag->getNext();
        }
        dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( frameset ) )->setFirstParag( parags.first() );
        parag2->setPrev( parags.last() );
    }

    else if ( parag1 && !parag2 )
    {
        parag = parag1->getNext();
        while ( parag )
        {
            old.append( new KWParag( *parag ) );
            parag = parag->getNext();
        }
        parag1->setNext( parags.first() );
    }

    if ( parag1 && parag2 )
    {
        parag = parag1->getNext();
        while ( parag != parag2 )
        {
            old.append( new KWParag( *parag ) );
            parag = parag->getNext();
        }
        parag1->setNext( parags.first() );
        parag2->setPrev( parags.last() );
    }

    fc->setTextPos( textPos );

    parags.clear();
    parags = old;
    parags.setAutoDelete( false );
}

/******************************************************************/
/* Class: KWCommandHistory                                        */
/******************************************************************/

/*================================================================*/
KWCommandHistory::KWCommandHistory()
    : current( -1 )
{
    history.setAutoDelete( true );
}

/*================================================================*/
void KWCommandHistory::addCommand( KWCommand *_command )
{
    if ( current < static_cast<int>( history.count() ) )
    {
        QList<KWCommand> _commands;
        _commands.setAutoDelete( false );

        for ( int i = 0; i < current; i++ )
        {
            _commands.insert( i, history.at( 0 ) );
            history.take( 0 );
        }

        _commands.append( _command );
        history.clear();
        history = _commands;
        history.setAutoDelete( true );
    }
    else
        history.append( _command );

    if ( history.count() > MAX_UNDO_REDO )
        history.removeFirst();
    else
        current++;

    emit undoRedoChanged( getUndoName(), getRedoName() );
}

/*================================================================*/
void KWCommandHistory::undo()
{
    if ( current > 0 )
    {
        history.at( current - 1 )->unexecute();
        current--;
        emit undoRedoChanged( getUndoName(), getRedoName() );
    }

}

/*================================================================*/
void KWCommandHistory::redo()
{
    if ( current < static_cast<int>( history.count() ) && current > -1 )
    {
        history.at( current )->execute();
        current++;
        emit undoRedoChanged( getUndoName(), getRedoName() );
    }
}

/*================================================================*/
QString KWCommandHistory::getUndoName()
{
    if ( current > 0 )
        return history.at( current - 1 )->getName();
    else
        return QString();
}

/*================================================================*/
QString KWCommandHistory::getRedoName()
{
    if ( current < static_cast<int>( history.count() ) && current > -1 )
        return history.at( current )->getName();
    else
        return QString();
}
