/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#ifndef __kspread_undo_h__
#define __kspread_undo_h__

class KSpreadUndo;
class KSpreadUndoAction;
class KSpreadTable;
class KSpreadLayout;
//class KSpreadCell;
class KSpreadDoc;
class ColumnLayout;
class RowLayout;
#include "kspread_cell.h"
#include <qstack.h>
#include <qstring.h>
#include <qrect.h>
#include <qlist.h>
#include <qvaluelist.h>

struct rowSize {
int rowNumber;
int rowHeight;
};

struct columnSize {
int columnNumber;
int columnWidth;
};

struct textOfCell {
int row;
int col;
QString text;
};

struct layoutCell {
int row;
int col;
KSpreadLayout *l;
};

struct layoutColumn {
int col;
ColumnLayout *l;
};

struct layoutRow {
int row;
RowLayout *l;
};


/**
 * Abstract base class. Every undo/redo action must
 * derive from this class.
 */
class KSpreadUndoAction
{
public:
    KSpreadUndoAction( KSpreadDoc *_doc ) { m_pDoc = _doc; }
    virtual ~KSpreadUndoAction() { }

    virtual void undo() = 0;
    virtual void redo() = 0;

    KSpreadDoc* doc() { return m_pDoc; }
    QString text() {return title ;}
// #### To be private
protected:
    KSpreadDoc *m_pDoc;
    QString title;
};

class KSpreadUndoRemoveColumn : public KSpreadUndoAction
{
public:
    KSpreadUndoRemoveColumn( KSpreadDoc *_doc, KSpreadTable *_table, int _column );
    virtual ~KSpreadUndoRemoveColumn();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
    QCString m_data;
    int m_iColumn;
};

class KSpreadUndoInsertColumn : public KSpreadUndoAction
{
public:
    KSpreadUndoInsertColumn( KSpreadDoc *_doc, KSpreadTable *_table, int _column );
    virtual ~KSpreadUndoInsertColumn();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
    int m_iColumn;
};

class KSpreadUndoRemoveRow : public KSpreadUndoAction
{
public:
    KSpreadUndoRemoveRow( KSpreadDoc *_doc, KSpreadTable *_table, int _row );
    virtual ~KSpreadUndoRemoveRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
    QCString m_data;
    int m_iRow;
};

class KSpreadUndoInsertRow : public KSpreadUndoAction
{
public:
    KSpreadUndoInsertRow( KSpreadDoc *_doc, KSpreadTable *_table, int _row );
    virtual ~KSpreadUndoInsertRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
    int m_iRow;
};

class KSpreadUndoSetText : public KSpreadUndoAction
{
public:
    KSpreadUndoSetText( KSpreadDoc *_doc, KSpreadTable *_table, const QString& _text, int _column, int _row,KSpreadCell::formatNumber _formatNumber );
    virtual ~KSpreadUndoSetText();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
    int m_iRow;
    int m_iColumn;
    QString m_strText;
    QString m_strRedoText;
    KSpreadCell::formatNumber m_eFormatNumber;
    KSpreadCell::formatNumber m_eFormatNumberRedo;
};

class KSpreadUndoCellLayout : public KSpreadUndoAction
{
public:
    KSpreadUndoCellLayout( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_selection, QString &_title );
    virtual ~KSpreadUndoCellLayout();

    virtual void undo();
    virtual void redo();

    void copyLayout( QValueList<layoutCell> &list,QValueList<layoutColumn> &listCol,QValueList<layoutRow> &listRow, KSpreadTable* table );

protected:
    QRect m_rctRect;
    QValueList<layoutCell> m_lstLayouts;
    QValueList<layoutCell> m_lstRedoLayouts;
    QValueList<layoutColumn> m_lstColLayouts;
    QValueList<layoutColumn> m_lstRedoColLayouts;
    QValueList<layoutRow> m_lstRowLayouts;
    QValueList<layoutRow> m_lstRedoRowLayouts;

    QString m_tableName;
};

class KSpreadUndoDelete : public KSpreadUndoAction
{
public:
    KSpreadUndoDelete( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_rect );
    virtual ~KSpreadUndoDelete();

    virtual void undo();
    virtual void redo();

    void createListCell( QCString &listCell,QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadTable* table );

protected:
    QRect m_selection;
    QCString m_data;
    QCString m_dataRedo;
    QValueList<columnSize> m_lstColumn;
    QValueList<columnSize> m_lstRedoColumn;
    QValueList<rowSize> m_lstRow;
    QValueList<rowSize> m_lstRedoRow;
    QString m_tableName;
};

class KSpreadUndoSetTableName : public KSpreadUndoAction
{
public:
    KSpreadUndoSetTableName( KSpreadDoc *doc, KSpreadTable *table, const QString& name );
    virtual ~KSpreadUndoSetTableName();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
    QString m_name;
    QString m_redoName;
};

class KSpreadUndoResizeColRow : public KSpreadUndoAction
{
public:
    KSpreadUndoResizeColRow( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_selection );
    virtual ~KSpreadUndoResizeColRow();

    virtual void undo();
    virtual void redo();

    void createList( QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadTable* table );

protected:
    QRect m_rctRect;
    QValueList<columnSize> m_lstColumn;
    QValueList<columnSize> m_lstRedoColumn;
    QValueList<rowSize> m_lstRow;
    QValueList<rowSize> m_lstRedoRow;
    QString m_tableName;
};

class KSpreadUndoChangeAreaTextCell : public KSpreadUndoAction
{
public:
    KSpreadUndoChangeAreaTextCell( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_selection );
    virtual ~KSpreadUndoChangeAreaTextCell();

    virtual void undo();
    virtual void redo();

    void createList( QValueList<textOfCell> &list, KSpreadTable* table );

protected:
    QRect m_rctRect;
    QValueList<textOfCell> m_lstTextCell;
    QValueList<textOfCell> m_lstRedoTextCell;
    QString m_tableName;
};

class KSpreadUndoMergedCell : public KSpreadUndoAction
{
public:
    KSpreadUndoMergedCell( KSpreadDoc *_doc, KSpreadTable *_table, int _column, int _row, int _extraX,int _extraY);
    virtual ~KSpreadUndoMergedCell();

    virtual void undo();
    virtual void redo();

protected:
    int m_iRow;
    int m_iCol;
    int m_iExtraX;
    int m_iExtraY;
    int m_iExtraRedoX;
    int m_iExtraRedoY;
    QString m_tableName;
};


class KSpreadUndoAutofill : public KSpreadUndoAction
{
public:
    KSpreadUndoAutofill( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_rect );
    virtual ~KSpreadUndoAutofill();

    virtual void undo();
    virtual void redo();
    void createListCell( QCString &list, KSpreadTable* table );
protected:
    QRect m_selection;
    QCString m_data;
    QCString m_dataRedo;
    QString m_tableName;
};

class KSpreadUndoInsertCellCol : public KSpreadUndoAction
{
public:
    KSpreadUndoInsertCellCol( KSpreadDoc *_doc, KSpreadTable *_table, int _column, int _row );
    virtual ~KSpreadUndoInsertCellCol();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
    int m_iRow;
    int m_iColumn;
};

class KSpreadUndoInsertCellRow : public KSpreadUndoAction
{
public:
    KSpreadUndoInsertCellRow( KSpreadDoc *_doc, KSpreadTable *_table, int _column, int _row );
    virtual ~KSpreadUndoInsertCellRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
    int m_iRow;
    int m_iColumn;
};

class KSpreadUndoRemoveCellCol : public KSpreadUndoAction
{
public:
    KSpreadUndoRemoveCellCol( KSpreadDoc *_doc, KSpreadTable *_table, int _column, int _row );
    virtual ~KSpreadUndoRemoveCellCol();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
    int m_iRow;
    int m_iColumn;
    QCString m_data;
};

class KSpreadUndoRemoveCellRow : public KSpreadUndoAction
{
public:
    KSpreadUndoRemoveCellRow( KSpreadDoc *_doc, KSpreadTable *_table, int _column, int _row );
    virtual ~KSpreadUndoRemoveCellRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
    int m_iRow;
    int m_iColumn;
    QCString m_data;
};

class KSpreadUndoConditional : public KSpreadUndoAction
{
public:
    KSpreadUndoConditional( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_rect );
    virtual ~KSpreadUndoConditional();

    virtual void undo();
    virtual void redo();
    void createListCell( QCString &list, KSpreadTable* table );
protected:
    QRect m_selection;
    QCString m_data;
    QCString m_dataRedo;
    QString m_tableName;
};

class KSpreadUndoHideTable : public KSpreadUndoAction
{
public:
    KSpreadUndoHideTable( KSpreadDoc *_doc, KSpreadTable *_table );
    virtual ~KSpreadUndoHideTable();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
};

class KSpreadUndoShowTable : public KSpreadUndoAction
{
public:
    KSpreadUndoShowTable( KSpreadDoc *_doc, KSpreadTable *_table );
    virtual ~KSpreadUndoShowTable();

    virtual void undo();
    virtual void redo();

protected:
    QString m_tableName;
};


class KSpreadUndoCellPaste : public KSpreadUndoAction
{
public:
    KSpreadUndoCellPaste( KSpreadDoc *_doc, KSpreadTable *_table,int _nbCol,int _nbRow, int _xshift,int _yshift, QRect &_selection );
    virtual ~KSpreadUndoCellPaste();

    virtual void undo();
    virtual void redo();
    void createListCell( QCString &listCell,QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadTable* table );

protected:
    QRect m_selection;
    QCString m_data;
    QCString m_dataRedo;
    QValueList<columnSize> m_lstColumn;
    QValueList<columnSize> m_lstRedoColumn;
    QValueList<rowSize> m_lstRow;
    QValueList<rowSize> m_lstRedoRow;
    int nbCol;
    int nbRow;
    int xshift;
    int yshift;
    QString m_tableName;
};


class KSpreadUndo
{
public:
    KSpreadUndo( KSpreadDoc *_doc );
    ~KSpreadUndo();

    void undo();
    void redo();
    void clear();

    void lock() { m_bLocked = TRUE; }
    void unlock() { m_bLocked = FALSE; }
    bool isLocked() { return m_bLocked; }

    bool hasUndoActions() { return !m_stckUndo.isEmpty(); }
    bool hasRedoActions() { return !m_stckRedo.isEmpty(); }

    void appendUndo( KSpreadUndoAction *_action );
    QString getUndoTitle();
    QString getRedoTitle();
protected:
    QStack<KSpreadUndoAction> m_stckUndo;
    QStack<KSpreadUndoAction> m_stckRedo;

    KSpreadDoc *m_pDoc;

    bool m_bLocked;
};

#endif
