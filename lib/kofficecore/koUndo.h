/* This file is part of the KDE project
   Copyright (C) 1999 Michael Koch <koch@kde.org>
 
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

#ifndef __koffice_undo_h__
#define __koffice_undo_h__

#include <qstring.h>
#include <qlist.h>
#include <qobject.h>

#define MAX_UNDO_REDO 100

class KoCommand;

typedef QList<KoCommand> KoCommandList;

/**
 *  The KoCommand class is an abstract command.
 *  This class has to be derived to use it.
 *
 *  @see KoCommandHistory
 *
 *  @short The KoCommand class is an abstract command.
 *  @author Michael Koch <koch@kde.orh>
 */
class KoCommand
{
public:

  /**
   *  Constructor.
   *
   *  @param _name Name of the command. This name is normally shown in the menu after "Undo" and "Redo".
   */
  KoCommand( QString _name )
  : m_name( _name ) {}

  /**
   *  Destructor.
   */   
  virtual ~KoCommand() {}

  /**
   *  Executes the command.
   *
   *  This method has to be overridden by each derived class.
   */
  virtual void execute() = 0;

  /**
   *  Makes the command undone.
   *
   *  This method has to be overridden by each derived class.
   */
  virtual void unexecute() = 0;

  QString name()
  { return m_name; }

  void setName( const QString& _name ) { m_name = _name; };

private:

  QString m_name;
};

/**
 *  The KoCommandHistory class provides undo/redo functionality
 *  for all KOffice-apps. It uses derived classes from KoCommand.
 *
 *  @see KoCommand
 * 
 *  @short The KoCommandHistory class provides undo/redo functionality.
 *  @author Michael Koch <koch@kde.org>
 */
class KoCommandHistory : public QObject
{
  Q_OBJECT

public:

  /**
   *  Constructor.
   */
  KoCommandHistory();

  /**
   *  Adds a class derived from KoCommand to the history.
   *
   *  @param _command The command that shall be added.
   *
   *  @see KoCommand
   */
  void addCommand( KoCommand *_command );

  /**
   *  Makes the last command undone.
   */
  void undo();

  /**
   *  Redoes the last command.
   */
  void redo();

  /**
   *  Retrieves the name of the actual command that can be undone.
   *
   *  This name should be present in the menu.
   *
   *  @see getRedoName, redo, undo
   */
  QString getUndoName();

  /**
   *  Retrieves the name of the actual command that can be redone.
   *
   *  This name should be present in the menu.
   *
   *  @see getUndoName, redo, undo
   */
  QString getRedoName();

  int count() { return m_history.count(); }

private:

  KoCommandList m_history;
  int m_current;

signals:

  /**
   *  This signal is emitted when the names of the undo and redo commands changed.
   *
   *  @param _undo New text of the actual undo command.
   *  @param _redo New text of the actual redo command.
   */
  void undoRedoChanged( QString, QString );
};

#endif // __koffice_undo_h__
