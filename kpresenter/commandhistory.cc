/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Command History                                        */
/******************************************************************/

#include "commandhistory.h"
#include "commandhistory.moc"

/******************************************************************/
/* Class: CommandHistory                                          */
/******************************************************************/

/*====================== constructor =============================*/
CommandHistory::CommandHistory()
{
	commands.setAutoDelete( true );
	present = 0;
}

/*======================= destructor =============================*/
CommandHistory::~CommandHistory()
{
	commands.clear();
}

/*====================== add command =============================*/
void CommandHistory::addCommand( Command *_command )
{
	if ( present < static_cast<int>( commands.count() ) )
    {
		QList<Command> _commands;
		_commands.setAutoDelete( false );

		for ( int i = 0; i < present; i++ )
		{
			_commands.insert( i, commands.at( 0 ) );
			commands.take( 0 );
		}

		_commands.append( _command );
		commands.clear();
		commands = _commands;
		commands.setAutoDelete( true );
    }
	else
		commands.append( _command );

	if ( commands.count() > MAX_UNDO_REDO )
		commands.removeFirst();
	else
		present++;

	emit undoRedoChanged( getUndoName(), getRedoName() );
}

/*======================= undo ===================================*/
void CommandHistory::undo()
{
	if ( present > 0 )
    {
		commands.at( present - 1 )->unexecute();
		present--;
		emit undoRedoChanged( getUndoName(), getRedoName() );
    }
}

/*======================= redo ===================================*/
void CommandHistory::redo()
{
	if ( present < static_cast<int>( commands.count() ) )
    {
		commands.at( present )->execute();
		present++;
		emit undoRedoChanged( getUndoName(), getRedoName() );
    }
}

/*======================== get undo name ========================*/
QString CommandHistory::getUndoName()
{
	if ( present > 0 )
		return commands.at( present - 1 )->getName();
	else
		return QString();
}

/*======================== get redo name ========================*/
QString CommandHistory::getRedoName()
{
	if ( present < static_cast<int>( commands.count() ) )
		return commands.at( present )->getName();
	else
		return QString();
}
