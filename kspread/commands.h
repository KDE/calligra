/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2004 Laurent Montel <montel@kde.org>

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

#include <koPageLayout.h>
#include <koUnit.h>

#include <kcommand.h>

#include <qstring.h>
#include <qptrstack.h>
#include <qrect.h>
#include <qptrlist.h>
#include <qvaluelist.h>

#include "kspread_sheet.h"

class KSpreadCell;
class KSpreadDoc;
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
\li DissociateCellCommand
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
  QString rangeName;
};


/**
 * Class DissociateCellCommand implements a command for breaking merged cells.
 */
class DissociateCellCommand : public KCommand
{
public:
  DissociateCellCommand( KSpreadCell* cell );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  KSpreadCell* cell;
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
  RenameSheetCommand( KSpreadSheet* sheet, const QString &name );

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


/**
 * Class SheetPropertiesCommand implements a command for changing sheet properties.
 */

class SheetPropertiesCommand : public KCommand
{
public:
  SheetPropertiesCommand( KSpreadDoc* doc, KSpreadSheet* sheet );
  void setLayoutDirection( KSpreadSheet::LayoutDirection direction );
  void setAutoCalc( bool b );
  void setShowGrid( bool b );
  void setShowPageBorders( bool b );
  void setShowFormula( bool b );
  void setHideZero( bool b );
  void setShowFormulaIndicator( bool b );
  void setColumnAsNumber( bool b );
  void setLcMode( bool b );
  void setCapitalizeFirstLetter( bool b );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  KSpreadSheet* sheet;
  KSpreadDoc* doc;
  KSpreadSheet::LayoutDirection oldDirection, newDirection;
  bool oldAutoCalc, newAutoCalc;
  bool oldShowGrid, newShowGrid;
  bool oldShowPageBorders, newShowPageBorders;
  bool oldShowFormula, newShowFormula;
  bool oldHideZero, newHideZero;
  bool oldShowFormulaIndicator, newShowFormulaIndicator;
  bool oldColumnAsNumber, newColumnAsNumber;
  bool oldLcMode, newLcMode;
  bool oldCapitalizeFirstLetter, newCapitalizeFirstLetter;
};


class InsertColumnCommand : public KCommand
{
public:
  InsertColumnCommand( KSpreadSheet* s , unsigned int _column, unsigned int _nbCol );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
    KSpreadDoc* doc;
    QString sheetName;
    unsigned int insertPosColumn;
    unsigned int nbColumnInserted;

};


class DefinePrintRangeCommand : public KCommand
{
public:
  DefinePrintRangeCommand( KSpreadSheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
    KSpreadDoc* doc;
    QString sheetName;
    QRect printRangeRedo, printRange;
};


class PaperLayoutCommand : public KCommand
{
public:
  PaperLayoutCommand( KSpreadSheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
    KSpreadDoc* doc;
    QString sheetName;
    KoPageLayout pl;
    KoPageLayout plRedo;
    KoHeadFoot hf;
    KoHeadFoot hfRedo;
    KoUnit::Unit unit;
    KoUnit::Unit unitRedo;
    bool printGrid;
    bool printGridRedo;
    bool printCommentIndicator;
    bool printCommentIndicatorRedo;
    bool printFormulaIndicator;
    bool printFormulaIndicatorRedo;
    QRect printRange;
    QRect printRangeRedo;
    QPair<int, int> printRepeatColumns;
    QPair<int, int> printRepeatColumnsRedo;
    QPair<int, int> printRepeatRows;
    QPair<int, int> printRepeatRowsRedo;
    double zoom;
    double zoomRedo;
    int pageLimitX;
    int pageLimitXRedo;
    int pageLimitY;
    int pageLimitYRedo;

};

class LinkCommand : public KCommand
{
public:
  LinkCommand( KSpreadCell* cell, const QString& text, const QString& link );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  KSpreadCell* cell;
  KSpreadDoc* doc;
  QString oldText;
  QString oldLink;
  QString newText;
  QString newLink;
};


#endif /* KSPREAD_COMMANDS */
