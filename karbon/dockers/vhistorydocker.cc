/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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

#include <qpainter.h>
#include <qptrvector.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>

#include "vhistorydocker.h"

VHistoryItem::VHistoryItem( QListBox* parent, VCommand* command )
		: QListBoxItem( parent ), m_command( command )
{
	setCustomHighlighting( true );
} // VHistoryItem::VHistoryItem


int VHistoryItem::width( const QListBox* lb ) const
{
	return lb->width() - 25;
} // VHistoryItem::width

void VHistoryItem::paint( QPainter* painter )
{
	KIconLoader il;
	painter->save();
	painter->setRasterOp( Qt::CopyROP );
	QRect r ( 0, 0, width( listBox() ), height( listBox() ) );
	painter->fillRect( r, ( m_command->success() ? Qt::white : Qt::lightGray ) );
	painter->drawPixmap( 1, 1, QPixmap( il.iconPath( m_command->icon(), KIcon::Small ) ) );
	painter->setPen( ( m_command->success() ? Qt::black : Qt::gray ) );
	painter->drawText( 25, 1, width( listBox() ) - 27,  height( listBox() ) - 2, Qt::AlignVCenter, m_command->name() );
	painter->restore();
	if ( isSelected() )
	{
		painter->setPen( listBox()->colorGroup().highlight() );
		painter->drawRect( r );
	}
	painter->flush();
} // VHistoryItem::paint

VHistoryDocker::VHistoryDocker( KarbonView* view )
		: m_view( view )
{
	setCaption( i18n( "History" ) );
	setWidget( m_history = new QListBox( this ) );
	m_history->setFixedSize( 160, 120 );
	m_history->setVScrollBarMode( QListBox::AlwaysOn );
	m_history->setSelectionMode( QListBox::Single );
	QPtrVector<VCommand> cmds;
	view->part()->commandHistory()->commands()->toVector( &cmds );
	int c = cmds.count();
	for ( int i = 0; i < c; i++ )
		new VHistoryItem( m_history, cmds[ i ] );

	connect( m_history, SIGNAL( mouseButtonClicked( int, QListBoxItem*, const QPoint& ) ), this, SLOT( commandClicked( int, QListBoxItem*, const QPoint& ) ) );
	connect( view->part()->commandHistory(), SIGNAL( historyCleared() ), this, SLOT( historyCleared() ) );
	connect( view->part()->commandHistory(), SIGNAL( commandAdded( VCommand* ) ), this, SLOT( commandAdded( VCommand* ) ) );
	connect( view->part()->commandHistory(), SIGNAL( commandExecuted( VCommand* ) ), this, SLOT( commandExecuted( VCommand* ) ) );
	connect( view->part()->commandHistory(), SIGNAL( firstCommandRemoved() ), this, SLOT( removeFirstCommand() ) );
	connect( view->part()->commandHistory(), SIGNAL( lastCommandRemoved() ), this, SLOT( removeLastCommand() ) );
	connect( this, SIGNAL( undoCommand( VCommand* ) ), view->part()->commandHistory(), SLOT( undo( VCommand* ) ) );
	connect( this, SIGNAL( redoCommand( VCommand* ) ), view->part()->commandHistory(), SLOT( redo( VCommand* ) ) );
	connect( this, SIGNAL( undoCommandsTo( VCommand* ) ), view->part()->commandHistory(), SLOT( undoAllTo( VCommand* ) ) );
	connect( this, SIGNAL( redoCommandsTo( VCommand* ) ), view->part()->commandHistory(), SLOT( redoAllTo( VCommand* ) ) );
} // VHistoryDocker::VHistoryDocker

void VHistoryDocker::historyCleared()
{
	m_history->clear();
} // VHistoryDocker::historyCleared

void VHistoryDocker::commandExecuted( VCommand* command )
{
	int i = 0;
	int c = m_history->count();
	while ( ( i < c ) && m_history->item( i ) && ( ( (VHistoryItem*)m_history->item( i ) )->command() != command ) )
		i++;
	if ( ( (VHistoryItem*)m_history->item( i ) )->command() == command )
		m_history->setCurrentItem( i );
	m_history->ensureCurrentVisible();
	m_history->repaintContents( m_history->itemRect( m_history->item( i ) ) );
} // VHistoryDocker::commandExecuted

void VHistoryDocker::commandAdded( VCommand* command )
{
	m_history->setCurrentItem( new VHistoryItem( m_history, command ) );
	m_history->ensureCurrentVisible();
} // VHistoryDocker::commandAdded

void VHistoryDocker::removeFirstCommand()
{
	if ( m_history->count() > 0 )
		m_history->removeItem( 0 );
} // VHistoryDocker::removeFirstCommand

void VHistoryDocker::removeLastCommand()
{
	if ( m_history->count() > 0 )
		m_history->removeItem( m_history->count() - 1 );
} // VHistoryDocker::removeLastCommand

void VHistoryDocker::commandClicked( int button, QListBoxItem* item, const QPoint& )
{
    if ( !item )
        return;
	VCommand* cmd = ( (VHistoryItem*)item )->command();
	if ( cmd->success() )
		if ( button == 1 )
			emit undoCommandsTo( ( (VHistoryItem*)item )->command() );
		else
			emit undoCommand( ( (VHistoryItem*)item )->command() );
	else
		if ( button == 1 )
			emit redoCommandsTo( ( (VHistoryItem*)item )->command() );
		else
			emit redoCommand( ( (VHistoryItem*)item )->command() );
} // VHistoryDocker::commandClicked

#include "vhistorydocker.moc"
