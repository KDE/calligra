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
class KSpreadCell;
class KSpreadDoc;
class ColumnLayout;
class RowLayout;

#include <qstack.h>
#include <qstring.h>
#include <qrect.h>
#include <qlist.h>

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

protected:
    KSpreadDoc *m_pDoc;
};

class KSpreadUndoRemoveColumn : public KSpreadUndoAction
{
public:
    KSpreadUndoRemoveColumn( KSpreadDoc *_doc, KSpreadTable *_table, int _column );
    virtual ~KSpreadUndoRemoveColumn();

    virtual void undo();
    virtual void redo();

    void appendCell( KSpreadCell *_cell );
    void setColumnLayout( ColumnLayout *l ) { m_pColumnLayout = l; }

protected:
    KSpreadTable *m_pTable;
    int m_iColumn;
    QList<KSpreadCell> m_lstCells;
    ColumnLayout *m_pColumnLayout;
};

class KSpreadUndoInsertColumn : public KSpreadUndoAction
{
public:
    KSpreadUndoInsertColumn( KSpreadDoc *_doc, KSpreadTable *_table, int _column );
    virtual ~KSpreadUndoInsertColumn();

    virtual void undo();
    virtual void redo();

protected:
    KSpreadTable *m_pTable;
    int m_iColumn;
};

class KSpreadUndoRemoveRow : public KSpreadUndoAction
{
public:
    KSpreadUndoRemoveRow( KSpreadDoc *_doc, KSpreadTable *_table, int _row );
    virtual ~KSpreadUndoRemoveRow();

    virtual void undo();
    virtual void redo();

    void appendCell( KSpreadCell *_cell );
    void setRowLayout( RowLayout *l ) { m_pRowLayout = l; }

protected:
    KSpreadTable *m_pTable;
    int m_iRow;
    QList<KSpreadCell> m_lstCells;
    RowLayout *m_pRowLayout;
};

class KSpreadUndoInsertRow : public KSpreadUndoAction
{
public:
    KSpreadUndoInsertRow( KSpreadDoc *_doc, KSpreadTable *_table, int _row );
    virtual ~KSpreadUndoInsertRow();

    virtual void undo();
    virtual void redo();

protected:
    KSpreadTable *m_pTable;
    int m_iRow;
};

class KSpreadUndoSetText : public KSpreadUndoAction
{
public:
    KSpreadUndoSetText( KSpreadDoc *_doc, KSpreadTable *_table, const QString& _text, int _column, int _row );
    virtual ~KSpreadUndoSetText();

    virtual void undo();
    virtual void redo();

protected:
    KSpreadTable *m_pTable;
    int m_iRow;
    int m_iColumn;
    QString m_strText;
    QString m_strRedoText;
};

class KSpreadUndoCellLayout : public KSpreadUndoAction
{
public:
    KSpreadUndoCellLayout( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_selection );
    virtual ~KSpreadUndoCellLayout();

    virtual void undo();
    virtual void redo();

    void copyLayout( QList<KSpreadLayout> &list);

protected:
    QRect m_rctRect;
    QList<KSpreadLayout> m_lstLayouts;
    QList<KSpreadLayout> m_lstRedoLayouts;
    KSpreadTable *m_pTable;
};

class KSpreadUndoDelete : public KSpreadUndoAction
{
public:
    KSpreadUndoDelete( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_rect );
    virtual ~KSpreadUndoDelete();

    virtual void undo();
    virtual void redo();

protected:
    QRect m_rctRect;
    QByteArray m_array;
    KSpreadTable *m_pTable;
};

class KSpreadUndoSetTableName : public KSpreadUndoAction
{
public:
    KSpreadUndoSetTableName( KSpreadDoc *doc, KSpreadTable *table, const QString& name );
    virtual ~KSpreadUndoSetTableName();

    virtual void undo();
    virtual void redo();

protected:
    KSpreadTable *m_pTable;
    QString m_name;
    QString m_redoName;
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

protected:
    QStack<KSpreadUndoAction> m_stckUndo;
    QStack<KSpreadUndoAction> m_stckRedo;

    KSpreadDoc *m_pDoc;

    bool m_bLocked;
};

#endif
