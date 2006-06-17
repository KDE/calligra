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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_COMMANDS
#define KSPREAD_COMMANDS

#include <QList>
#include <QRect>
#include <QString>

#include <KoPageLayout.h>
#include <KoQueryTrader.h>
#include <KoUnit.h>

#include <kcommand.h>

#include "kspread_object.h"
#include "kspread_sheet.h" // for Sheet::LayoutDirection

/**
 * The KSpread namespace.
 */
namespace KSpread
{
class Cell;
class Doc;
class UndoAction;

/** \page commands Commands

To implement undo and redo functionality, every possible action
by the user for editing and manipulating the document is encapsulated
in a command (based on KCommand).
There is one command class (which will be instantiated) for every unique
action. You need to reimplement the execute() and unexecute() methods
of KCommand.

Each command is created from the user interface, and then added
to the command history (see Doc::commandHistory) using
Doc::addCommand method. Because the command is not immediately
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


\sa Doc::addCommand
\sa KoCommandHistory

*/

/**
 * Class UndoWrapperCommand is used to help migration from custom
 * UndoAction to KCommand-based system.
 * See Doc::addCommand for more information.
 */
class UndoWrapperCommand : public KCommand
{
public:
  UndoWrapperCommand( UndoAction* undoAction );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  UndoAction* undoAction;
};


/**
 * Class MergeCellCommand implements a command for merging two or more cells
 * into one cell.
 * \deprecated Use MergeManipulator
 */
class KDE_DEPRECATED MergeCellCommand : public KCommand
{
public:
  MergeCellCommand( Cell* cell, int colSpan, int rowSpan );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  Cell* cell;
  int colSpan;
  int rowSpan;
  int oldColSpan;
  int oldRowSpan;
  QString rangeName;
};


/**
 * Class DissociateCellCommand implements a command for breaking merged cells.
 * \deprecated Use MergeManipulator
 */
class KDE_DEPRECATED DissociateCellCommand : public KCommand
{
public:
  DissociateCellCommand( Cell* cell );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  Cell* cell;
  int oldColSpan;
  int oldRowSpan;
};


/**
 * Class RenameSheetCommand implements a command for renaming a sheet.
 *
 * \sa Sheet::setSheetName
 */

class RenameSheetCommand : public KCommand
{
public:
  RenameSheetCommand( Sheet* sheet, const QString &name );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  Sheet* sheet;
  QString oldName;
  QString newName;
};

class HideSheetCommand : public KCommand
{
public:
  HideSheetCommand( Sheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  Doc* doc;
  QString sheetName;
};

class ShowSheetCommand : public KCommand
{
public:
  ShowSheetCommand( Sheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  Doc* doc;
  QString sheetName;
};


class AddSheetCommand : public KCommand
{
public:
  AddSheetCommand( Sheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
    Sheet* sheet;
    Doc* doc;
};


class RemoveSheetCommand : public KCommand
{
public:
  RemoveSheetCommand( Sheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
    Sheet* sheet;
    Doc* doc;
};


/**
 * Class SheetPropertiesCommand implements a command for changing sheet properties.
 */

class SheetPropertiesCommand : public KCommand
{
public:
  SheetPropertiesCommand( Doc* doc, Sheet* sheet );
  void setLayoutDirection( Sheet::LayoutDirection direction );
  void setAutoCalc( bool b );
  void setShowGrid( bool b );
  void setShowPageBorders( bool b );
  void setShowFormula( bool b );
  void setHideZero( bool b );
  void setShowFormulaIndicator( bool b );
  void setShowCommentIndicator( bool b );
  void setColumnAsNumber( bool b );
  void setLcMode( bool b );
  void setCapitalizeFirstLetter( bool b );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  Sheet* sheet;
  Doc* doc;
  Sheet::LayoutDirection oldDirection, newDirection;
  bool oldAutoCalc, newAutoCalc;
  bool oldShowGrid, newShowGrid;
  bool oldShowPageBorders, newShowPageBorders;
  bool oldShowFormula, newShowFormula;
  bool oldHideZero, newHideZero;
  bool oldShowFormulaIndicator, newShowFormulaIndicator;
  bool oldShowCommentIndicator, newShowCommentIndicator;
  bool oldColumnAsNumber, newColumnAsNumber;
  bool oldLcMode, newLcMode;
  bool oldCapitalizeFirstLetter, newCapitalizeFirstLetter;
};


class InsertColumnCommand : public KCommand
{
public:
  InsertColumnCommand( Sheet* s , unsigned int _column, unsigned int _nbCol );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
    Doc* doc;
    QString sheetName;
    unsigned int insertPosColumn;
    unsigned int nbColumnInserted;

};


class DefinePrintRangeCommand : public KCommand
{
public:
  DefinePrintRangeCommand( Sheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
    Doc* doc;
    QString sheetName;
    QRect printRangeRedo, printRange;
};


class PaperLayoutCommand : public KCommand
{
public:
  PaperLayoutCommand( Sheet* sheet );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
    Doc* doc;
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
  LinkCommand( Cell* cell, const QString& text, const QString& link );

  virtual void execute();
  virtual void unexecute();
  virtual QString name() const;

protected:
  Cell* cell;
  Doc* doc;
  QString oldText;
  QString oldLink;
  QString newText;
  QString newLink;
};


class ChangeObjectGeometryCommand : public KCommand
{
  public:
    ChangeObjectGeometryCommand( EmbeddedObject *_obj, const KoPoint &_m_diff, const KoSize &_r_diff );
    ~ChangeObjectGeometryCommand();

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;

  protected:
    KoPoint m_diff;
    KoSize r_diff;
    EmbeddedObject *obj;
    Doc *doc;
};

class RemoveObjectCommand : public KCommand
{
  public:
    RemoveObjectCommand( EmbeddedObject *_obj, bool _cut = false );
    ~RemoveObjectCommand();

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;

  protected:
    EmbeddedObject *obj;
    Doc* doc;
    bool executed;
    bool cut;
};

class InsertObjectCommand : public KCommand
{
  public:
    InsertObjectCommand( const KoRect& _geometry, KoDocumentEntry&, Canvas *_canvas ); //child
    InsertObjectCommand( const KoRect& _geometry, KoDocumentEntry&, const QRect& _data, Canvas *_canvas ); //chart
    InsertObjectCommand( const KoRect& _geometry, KUrl& _file, Canvas *_canvas ); //picture
    ~InsertObjectCommand();

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;

  protected:
    KoRect geometry;
    Canvas *canvas;
    bool executed;
    KoDocumentEntry entry;
    QRect data;
    ObjType type;
    KUrl file;
    EmbeddedObject *obj;
};

class RenameNameObjectCommand : public KNamedCommand
{
public:
    RenameNameObjectCommand( const QString &_name, const QString &_objectName, EmbeddedObject *_obj, Doc *_doc );
    ~RenameNameObjectCommand();
    void execute();
    void unexecute();
protected:
    QString oldObjectName, newObjectName;
    EmbeddedObject *object;
    Doc *doc;
    Sheet *m_page;
};


class GeometryPropertiesCommand : public KNamedCommand
{
public:
    enum KgpType { ProtectSize, KeepRatio};
    GeometryPropertiesCommand( const QString &name, QList<EmbeddedObject*> &objects,
                                  bool newValue, KgpType type, Doc *_doc );
    GeometryPropertiesCommand( const QString &name, QList<bool> &lst, QList<EmbeddedObject*> &objects,
                                  bool newValue, KgpType type, Doc *_doc );
    ~GeometryPropertiesCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    QList<bool> m_oldValue;
    QList<EmbeddedObject*> m_objects;
    bool m_newValue;
    KgpType m_type;
    Doc *m_doc;
};

class MoveObjectByCmd : public KNamedCommand
{
public:
    MoveObjectByCmd( const QString &_name, const KoPoint &_diff, QList<EmbeddedObject*> &_objects,
               Doc *_doc, Sheet *m_page );
    ~MoveObjectByCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    KoPoint diff;
    QList<EmbeddedObject*> objects;
    Doc *doc;
    Sheet *m_page;
};

} // namespace KSpread

#endif /* KSPREAD_COMMANDS */
