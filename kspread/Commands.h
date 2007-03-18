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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_COMMANDS
#define KSPREAD_COMMANDS

#include <QList>
#include <QRect>
#include <QString>
#include <QUndoCommand>

#include <KoPageLayout.h>
#include <KoQueryTrader.h>
#include <KoUnit.h>

#include "Object.h"
#include "Sheet.h" // for Sheet::LayoutDirection

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
in a command (based on QUndoCommand).
There is one command class (which will be instantiated) for every unique
action. You need to reimplement the redo() and undo() methods
of QUndoCommand.

Each command is created from the user interface, and then added
to the command history using Doc::addCommand method. The command is
immediately executed, if you add it.
This is an example of typical use of command:

\code
QUndoCommand* command = new RenameSheetCommand( sheet, name );
doc->addCommand( command );
\endcode

Then whenever the user triggers an "undo", the corresponding
undo() method of the command is called by the undo action,
thereby reverting the previously executed command. Similar thing
happens for the "redo".

Alphabetical list of commands:

\li AddSheetCommand
\li DissociateCellCommand
\li MergeCellCommand
\li RemoveSheetCommand
\li RenameSheetCommand


\sa Doc::addCommand

*/

/**
 * Class UndoWrapperCommand is used to help migration from custom
 * UndoAction to QUndoCommand-based system.
 * See Doc::addCommand for more information.
 */
class UndoWrapperCommand : public QUndoCommand
{
public:
  explicit UndoWrapperCommand( UndoAction* undoAction );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
  UndoAction* undoAction;
};


/**
 * Class MergeCellCommand implements a command for merging two or more cells
 * into one cell.
 * \deprecated Use MergeManipulator
 */
class KDE_DEPRECATED MergeCellCommand : public QUndoCommand
{
public:
  MergeCellCommand( const Cell& cell, int colSpan, int rowSpan );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
  Cell cell;
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
class KDE_DEPRECATED DissociateCellCommand : public QUndoCommand
{
public:
  explicit DissociateCellCommand( const Cell& cell );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
  Cell cell;
  int oldColSpan;
  int oldRowSpan;
};


/**
 * Class RenameSheetCommand implements a command for renaming a sheet.
 *
 * \sa Sheet::setSheetName
 */

class RenameSheetCommand : public QUndoCommand
{
public:
  RenameSheetCommand( Sheet* sheet, const QString &name );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
  Sheet* sheet;
  QString oldName;
  QString newName;
};

class HideSheetCommand : public QUndoCommand
{
public:
  explicit HideSheetCommand( Sheet* sheet );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
  Doc* doc;
  QString sheetName;
};

class ShowSheetCommand : public QUndoCommand
{
public:
  explicit ShowSheetCommand( Sheet* sheet );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
  Doc* doc;
  QString sheetName;
};


class AddSheetCommand : public QUndoCommand
{
public:
  explicit AddSheetCommand( Sheet* sheet );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
    Sheet* sheet;
    Doc* doc;
};


class RemoveSheetCommand : public QUndoCommand
{
public:
  explicit RemoveSheetCommand( Sheet* sheet );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
    Sheet* sheet;
    Doc* doc;
};


/**
 * Class SheetPropertiesCommand implements a command for changing sheet properties.
 */

class SheetPropertiesCommand : public QUndoCommand
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

  virtual void redo();
  virtual void undo();
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


class DefinePrintRangeCommand : public QUndoCommand
{
public:
  explicit DefinePrintRangeCommand( Sheet* sheet );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
    Doc* doc;
    QString sheetName;
    QRect printRangeRedo, printRange;
};


class PaperLayoutCommand : public QUndoCommand
{
public:
  explicit PaperLayoutCommand( Sheet* sheet );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
    Doc* doc;
    QString sheetName;
    KoPageLayout pl;
    KoPageLayout plRedo;
    KoHeadFoot hf;
    KoHeadFoot hfRedo;
    KoUnit unit;
    KoUnit unitRedo;
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

class LinkCommand : public QUndoCommand
{
public:
  LinkCommand( const Cell& cell, const QString& text, const QString& link );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
  Cell cell;
  Doc* doc;
  QString oldText;
  QString oldLink;
  QString newText;
  QString newLink;
};


class ChangeObjectGeometryCommand : public QUndoCommand
{
  public:
    ChangeObjectGeometryCommand( EmbeddedObject *_obj, const QPointF &_m_diff, const QSizeF &_r_diff );
    ~ChangeObjectGeometryCommand();

    virtual void redo();
    virtual void undo();
    virtual QString name() const;

  protected:
    QPointF m_diff;
    QSizeF r_diff;
    EmbeddedObject *obj;
    Doc *doc;
};

class RemoveObjectCommand : public QUndoCommand
{
  public:
    explicit RemoveObjectCommand( EmbeddedObject *_obj, bool _cut = false );
    ~RemoveObjectCommand();

    virtual void redo();
    virtual void undo();
    virtual QString name() const;

  protected:
    EmbeddedObject *obj;
    Doc* doc;
    bool executed;
    bool cut;
};

class InsertObjectCommand : public QUndoCommand
{
  public:
    InsertObjectCommand( const QRectF& _geometry, KoDocumentEntry&, Canvas *_canvas ); //child
    InsertObjectCommand( const QRectF& _geometry, KoDocumentEntry&, const QRect& _data, Canvas *_canvas ); //chart
    InsertObjectCommand( const QRectF& _geometry, KUrl& _file, Canvas *_canvas ); //picture
    ~InsertObjectCommand();

    virtual void redo();
    virtual void undo();
    virtual QString name() const;

  protected:
    QRectF geometry;
    Canvas *canvas;
    bool executed;
    KoDocumentEntry entry;
    QRect data;
    ObjType type;
    KUrl file;
    EmbeddedObject *obj;
};

class RenameNameObjectCommand : public QUndoCommand
{
public:
    RenameNameObjectCommand( const QString &_name, const QString &_objectName, EmbeddedObject *_obj, Doc *_doc );
    ~RenameNameObjectCommand();
    void redo();
    void undo();
protected:
    QString oldObjectName, newObjectName;
    EmbeddedObject *object;
    Doc *doc;
    Sheet *m_page;
};


class GeometryPropertiesCommand : public QUndoCommand
{
public:
    enum KgpType { ProtectSize, KeepRatio};
    GeometryPropertiesCommand( const QString &name, QList<EmbeddedObject*> &objects,
                                  bool newValue, KgpType type, Doc *_doc );
    GeometryPropertiesCommand( const QString &name, QList<bool> &lst, QList<EmbeddedObject*> &objects,
                                  bool newValue, KgpType type, Doc *_doc );
    ~GeometryPropertiesCommand();

    virtual void redo();
    virtual void undo();

protected:
    QList<bool> m_oldValue;
    QList<EmbeddedObject*> m_objects;
    bool m_newValue;
    KgpType m_type;
    Doc *m_doc;
};

class MoveObjectByCmd : public QUndoCommand
{
public:
    MoveObjectByCmd( const QString &_name, const QPointF &_diff, QList<EmbeddedObject*> &_objects,
               Doc *_doc, Sheet *m_page );
    ~MoveObjectByCmd();

    virtual void redo();
    virtual void undo();

protected:

    QPointF diff;
    QList<EmbeddedObject*> objects;
    Doc *doc;
    Sheet *m_page;
};

} // namespace KSpread

#endif /* KSPREAD_COMMANDS */
