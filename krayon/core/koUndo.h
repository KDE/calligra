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

#include <qlist.h>
#include <qobject.h>

class KoCommand;
class QString;
class QStringList;

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
  KoCommand( const QString& _name )
    : m_name( _name ) {}

  /**
   *  Destructor.
   */
  virtual ~KoCommand() {}

  /**
   *  Executes the command.
   *
   *  This method has to be overridden by each derived class.
   *
   *  @see unexecute
   */
  virtual void execute() = 0;

  /**
   *  Makes the command undone.
   *
   *  This method has to be overridden by each derived class.
   *
   *  @see execute
   */
  virtual void unexecute() = 0;

  /**
   *  Retrieves the name of the command.
   *
   *  @see setName
   */
  QString name() const
  { return m_name; }

  /**
   *  Sets the name of the command.
   *
   *  @see name
   */
  void setName( const QString& _name ) { m_name = _name; }

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
   *
   *  @param _number      The number of strings that will be returned by
   *                      @ref #getUndoList and @ref #getRedoList.
   *  @param _maxundoredo The number of commands that can be stored in the
   *                      history maximal.
   *
   *  @see #getUndoList, #getRedoList
   */
  KoCommandHistory( int _number = 0, int _maxundoredo = 100 );

  /**
   *  Adds a class derived from KoCommand to the history.
   *
   *  @param _command The command that shall be added.
   */
  void addCommand( KoCommand *_command );

  /**
   *  Makes the last command undone.
   *
   *  @see #redo
   */
  void undo();

  /**
   *  Redoes the last command.
   *
   *  @see #undo
   */
  void redo();

  /**
   *  Retrieves the name of the actual command that can be undone.
   *
   *  This name should be present in the menu.
   *
   *  @see #getRedoName, #redo, #undo
   */
  QString getUndoName();

  /**
   *  Retrieves the strings for last actions that can be undone. The maximal
   *  number of strings is set by the constructor or by @ref setNumToolbarItems .
   *
   *  @return List of UNDO-strings.
   *
   *  @see #getUndoName, #setNumToolbarItems, #numToolbarItems
   */
  QStringList getUndoList();

  /**
   *  Retrieves the name of the actual command that can be redone.
   *
   *  This name should be present in the menu.
   *
   *  @see #getUndoName, #redo, #undo
   */
  QString getRedoName();

  /**
   *  Retrieves the strings for last actions that can be redone. The maximal
   *  number of strings is set by the constructor or by @ref setNumToolbarItems .
   *
   *  @return List of REDO-strings.
   *
   *  @see #getRedoName, #setNumToolbarItems, #numToolbarItems
   */
  QStringList getRedoList();

  /**
   *  Returns the number of items in the histoy.
   *
   *  @see #maxUndoRedo, #setMaxUndoRedo
   */
  int count() const { return m_history.count(); }

  /**
   *  Returns the maximal number of items that can be undone/redone.
   *
   *  @see #setMaxUndoRedo
   */
  int maxUndoRedo() const { return m_maxUndoRedo; }

  /**
   *  Sets the maximal number of items that can be undone/redone.
   *
   *  @see #maxUndoRedo
   */
  void setMaxUndoRedo( int _maxundoredo );

  /**
   *  Returns the maximal number of items that will be return in a undo/redo lists.
   *
   *  @see #setNumToolbarItems, #getUndoList, #getRedoList
   */
  int numToolbarItems() const { return m_numToolbarItems; }

  /**
   *  Sets the maximal number of items that will be return in a undo/redo lists.
   *
   *  @see #numToolbarItems, #getUndoList, #getRedoList
   */
  void setNumToolbarItems( int _number );

private:

  void emitSignals();

  KoCommandList m_history;
  int m_current;
  uint m_maxUndoRedo;
  int m_numToolbarItems;

signals:

  /**
   *  This signal is emitted when the names of the undo and redo commands changed.
   *
   *  @param _undo New text of the actual undo command.
   *  @param _redo New text of the actual redo command.
   *
   *  @see #getUndoName, #getRedoName
   */
  void undoRedoChanged( QString, QString );

  /**
   *  This signal is emitted when the names of the undo and redo commands changed.
   *
   *  @param _undo New list of text of the actual undo commands.
   *  @param _redo New list of text of the actual redo commands.
   *
   *  @see #getUndoList, #getRedoList
   */
  void undoRedoChanged( QStringList, QStringList );
};

#endif // __koffice_undo_h__
