/* This file is part of the KDE project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>

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
class KSpreadUndoAction;

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

#endif /* KSPREAD_COMMANDS */
