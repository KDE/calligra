/* This file is part of the KDE project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
             (C) 2004 Laurent Montel <montel@kde.org>

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

#ifndef KSPREAD_COMMANDS
#define KSPREAD_COMMANDS

#include <qstring.h>

#include <kcommand.h>

class KSpreadDoc;
class KSpreadSheet;
class KSpreadCell;
class KSpreadUndoAction;

/** \page commands Commands

To implement undo and redo functionality, every possible action
by the user for editing and manipulating the document is encapsulated
in a command (based on KCommand). 
There is one command class (which will be instantiated) for every unique 
action. You need to reimplement the execute() and unexecute() methods 
of KCommand.
 
Each command is created from the user interface, and then added 
to the command history (see KSpreadDoc::commandHistory) using 
KSpreadDoc::addCommand method. Because the command is not immediately 
executed, you also need to call the execute() method of that command.
This is an example of typical use of command:
 
\code
KCommand* command = new RenameSheetCommand( sheet, name );
doc->addCommand( command );
command->execute();
\endcode
 
Then whenever the user triggers an "undo", the corresponding 
unexecute() method of the command is called by the undo action,
thereby reverting the previously executed command. Similar thing 
happens for the "redo".

Alphabetical list of commands:
 
\li AddSheetCommand
\li MergeCellCommand
\li RemoveSheetCommand
\li RenameSheetCommand


\sa KSpreadDoc::addCommand
\sa KoCommandHistory
 
*/

/**
 * Class UndoWrapperCommand is used to help migration from custom
 * KSpreadUndoAction to KCommand-based system.
 * See KSpreadDoc::addCommand for more information.
 */
class UndoWrapperCommand : public KCommand
{
public:
  UndoWrapperCommand( KSpreadUndoAction* undoAction );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  KSpreadUndoAction* undoAction;
};


/**
 * Class MergeCellCommand implements a command for merging two or more cells
 * into one cell.
 */
class MergeCellCommand : public KCommand
{
public:
  MergeCellCommand( KSpreadCell* cell, int colSpan, int rowSpan );
  
  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;
  
protected:
  KSpreadCell* cell;
  int colSpan;
  int rowSpan;
  int oldColSpan;
  int oldRowSpan;
};


/**
 * Class RenameSheetCommand implements a command for renaming a sheet.
 *
 * \sa KSpreadSheet::setTableName
 */

class RenameSheetCommand : public KCommand
{
public:
  RenameSheetCommand( KSpreadSheet* sheet, QString name );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  KSpreadSheet* sheet;
  QString oldName;
  QString newName;
};

class HideSheetCommand : public KCommand
{
public:
  HideSheetCommand( KSpreadSheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  KSpreadDoc* doc;
  QString sheetName;
};

class ShowSheetCommand : public KCommand
{
public:
  ShowSheetCommand( KSpreadSheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  KSpreadDoc* doc;
  QString sheetName;
};


class AddSheetCommand : public KCommand
{
public:
  AddSheetCommand( KSpreadSheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
    KSpreadSheet* sheet;
    KSpreadDoc* doc;
};


class RemoveSheetCommand : public KCommand
{
public:
  RemoveSheetCommand( KSpreadSheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
    KSpreadSheet* sheet;
    KSpreadDoc* doc;
};

#endif /* KSPREAD_COMMANDS */
