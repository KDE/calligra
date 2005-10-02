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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __kspread_undo_h__
#define __kspread_undo_h__

class KSpreadUndo;
class KSpreadUndoAction;
class KSpreadSheet;
class KSpreadFormat;
class KSpreadDoc;
class ColumnFormat;
class RowFormat;

#include "kspread_global.h"
#include "kspread_doc.h"

#include <koUnit.h>
#include <koPageLayout.h>

#include <qptrstack.h>
#include <qstring.h>
#include <qrect.h>
#include <qptrlist.h>
#include <qvaluelist.h>

struct rowSize {
int rowNumber;
double rowHeight;
};

struct columnSize {
int columnNumber;
double columnWidth;
};

struct textOfCell {
int row;
int col;
QString text;
};

struct layoutTextCell {
int row;
int col;
KSpreadFormat * l;
QString text;
};

struct layoutCell {
int row;
int col;
KSpreadFormat *l;
};

struct layoutColumn {
int col;
ColumnFormat *l;
};

struct layoutRow {
int row;
RowFormat *l;
};

struct styleCell {
  int row;
  int col;
  QString action;
};

class FormulaOfCell
{
public:
    FormulaOfCell(): m_sheetName(0) {}
    FormulaOfCell( QString & sheetName, int col, int row, QString & formula )
        : m_sheetName( sheetName ), m_col( col ), m_row( row ), m_formula( formula )
    {}

    QString sheetName() const { return m_sheetName; }
    QString formula() const { return m_formula; }
    int col() const { return m_col; }
    int row() const { return m_row; }

private:
    QString m_sheetName;
    int m_col;
    int m_row;
    QString m_formula;
};

/**
 * Abstract base class. Every undo/redo action must
 * derive from this class.
 */
class KSpreadUndoAction
{
public:
    KSpreadUndoAction( KSpreadDoc *_doc ) { m_pDoc = _doc; m_pDoc->setModified(true); }
    virtual ~KSpreadUndoAction() { }

    virtual void undo() = 0;
    virtual void redo() = 0;

    KSpreadDoc* doc()const { return m_pDoc; }

    QString getName()const {return name ;}

protected:
    KSpreadDoc *m_pDoc;
    QString name;
};

class KSpreadMacroUndoAction : public KSpreadUndoAction
{
public:
    KSpreadMacroUndoAction( KSpreadDoc * _doc, const QString & _name );
    virtual ~KSpreadMacroUndoAction();

    void addCommand(KSpreadUndoAction *command);

    virtual void undo();
    virtual void redo();

protected:
    QPtrList<KSpreadUndoAction> m_commands;
};

class KSpreadUndoInsertRemoveAction : public KSpreadUndoAction
{
public:
    KSpreadUndoInsertRemoveAction( KSpreadDoc *_doc );
    virtual ~KSpreadUndoInsertRemoveAction();

    void saveFormulaReference( KSpreadSheet *_sheet, int col, int row, QString & formula );

protected:
    void undoFormulaReference();
    QValueList<FormulaOfCell> m_lstFormulaCells;
};

class KSpreadUndoRemoveColumn : public KSpreadUndoInsertRemoveAction
{
public:
    KSpreadUndoRemoveColumn( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column,int _nbCol=0 );
    virtual ~KSpreadUndoRemoveColumn();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    QCString m_data;
    int m_iColumn;
    int m_iNbCol;
    QRect m_printRange;
    QPair<int, int> m_printRepeatColumns;
};

class KSpreadUndoInsertColumn : public KSpreadUndoInsertRemoveAction
{
public:
    KSpreadUndoInsertColumn( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column,int _nbCol=0 );
    virtual ~KSpreadUndoInsertColumn();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    int m_iColumn;
    int m_iNbCol;
};

class KSpreadUndoRemoveRow : public KSpreadUndoInsertRemoveAction
{
public:
    KSpreadUndoRemoveRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _row,int _nbRow=0 );
    virtual ~KSpreadUndoRemoveRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    QCString m_data;
    int m_iRow;
    int m_iNbRow;
    QRect m_printRange;
    QPair<int, int> m_printRepeatRows;
};

class KSpreadUndoInsertRow : public KSpreadUndoInsertRemoveAction
{
public:
    KSpreadUndoInsertRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _row,int _nbRow=0 );
    virtual ~KSpreadUndoInsertRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    int m_iRow;
    int m_iNbRow;
};


class KSpreadUndoHideColumn : public KSpreadUndoAction
{
public:
    KSpreadUndoHideColumn( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column,int _nbCol=0, QValueList<int>listCol=QValueList<int>() );
    virtual ~KSpreadUndoHideColumn();

    virtual void undo();
    virtual void redo();
    void createList( QValueList<int>&list,KSpreadSheet *_tab );

protected:
    QString m_sheetName;
    int m_iColumn;
    int m_iNbCol;
    QValueList<int> listCol;
};

class KSpreadUndoHideRow : public KSpreadUndoAction
{
public:
    KSpreadUndoHideRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column,int _nbCol=0, QValueList<int>_listRow=QValueList<int>() );
    virtual ~KSpreadUndoHideRow();

    virtual void undo();
    virtual void redo();
protected:
    void createList( QValueList<int>&list,KSpreadSheet *_tab );

    QString m_sheetName;
    int m_iRow;
    int m_iNbRow;
    QValueList<int> listRow;
};

class KSpreadUndoShowColumn : public KSpreadUndoAction
{
public:
    KSpreadUndoShowColumn( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column,int _nbCol=0, QValueList<int>_list=QValueList<int>() );
    virtual ~KSpreadUndoShowColumn();

    virtual void undo();
    virtual void redo();
protected:
    void createList( QValueList<int>&list,KSpreadSheet *_tab );

    QString m_sheetName;
    int m_iColumn;
    int m_iNbCol;
    QValueList<int> listCol;
};

class KSpreadUndoShowRow : public KSpreadUndoAction
{
public:
    KSpreadUndoShowRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column,int _nbCol=0, QValueList<int>list=QValueList<int>() );
    virtual ~KSpreadUndoShowRow();

    virtual void undo();
    virtual void redo();

protected:
    void createList( QValueList<int>&list,KSpreadSheet *_tab );
    QString m_sheetName;
    int m_iRow;
    int m_iNbRow;
    QValueList<int> listRow;
};


class KSpreadUndoPaperLayout : public KSpreadUndoAction
{
public:
    KSpreadUndoPaperLayout( KSpreadDoc *_doc, KSpreadSheet *_sheet );
    virtual ~KSpreadUndoPaperLayout();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    KoPageLayout m_pl;
    KoPageLayout m_plRedo;
    KoHeadFoot m_hf;
    KoHeadFoot m_hfRedo;
    KoUnit::Unit m_unit;
    KoUnit::Unit m_unitRedo;
    bool m_printGrid;
    bool m_printGridRedo;
    bool m_printCommentIndicator;
    bool m_printCommentIndicatorRedo;
    bool m_printFormulaIndicator;
    bool m_printFormulaIndicatorRedo;
    QRect m_printRange;
    QRect m_printRangeRedo;
    QPair<int, int> m_printRepeatColumns;
    QPair<int, int> m_printRepeatColumnsRedo;
    QPair<int, int> m_printRepeatRows;
    QPair<int, int> m_printRepeatRowsRedo;
    double m_dZoom;
    double m_dZoomRedo;
    int m_iPageLimitX;
    int m_iPageLimitXRedo;
    int m_iPageLimitY;
    int m_iPageLimitYRedo;
};


class KSpreadUndoSetText : public KSpreadUndoAction
{
public:
    KSpreadUndoSetText( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QString& _text, int _column, int _row, FormatType _formatType );
    virtual ~KSpreadUndoSetText();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    int m_iRow;
    int m_iColumn;
    QString m_strText;
    QString m_strRedoText;
    FormatType m_eFormatType;
    FormatType m_eFormatTypeRedo;
};

class KSpreadUndoCellFormat : public KSpreadUndoAction
{
public:
    KSpreadUndoCellFormat( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_selection, const QString &_title );
    virtual ~KSpreadUndoCellFormat();

    virtual void undo();
    virtual void redo();

protected:
    void copyFormat( QValueList<layoutCell> &list,QValueList<layoutColumn> &listCol,QValueList<layoutRow> &listRow, KSpreadSheet* sheet );

    QRect m_rctRect;
    QValueList<layoutCell> m_lstFormats;
    QValueList<layoutCell> m_lstRedoFormats;
    QValueList<layoutColumn> m_lstColFormats;
    QValueList<layoutColumn> m_lstRedoColFormats;
    QValueList<layoutRow> m_lstRowFormats;
    QValueList<layoutRow> m_lstRedoRowFormats;

    QString m_sheetName;
};

class KSpreadUndoResizeColRow;

class KSpreadUndoChangeAngle : public KSpreadUndoAction
{
public:
    KSpreadUndoChangeAngle( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_selection );
    virtual ~KSpreadUndoChangeAngle();

    virtual void undo();
    virtual void redo();

protected:

   KSpreadUndoCellFormat* m_layoutUndo;
   KSpreadUndoResizeColRow* m_resizeUndo;

};

class KSpreadUndoDelete : public KSpreadUndoAction
{
public:
    KSpreadUndoDelete( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_rect );
    virtual ~KSpreadUndoDelete();

    virtual void undo();
    virtual void redo();

protected:
    void createListCell( QCString &listCell,QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadSheet* sheet );

    QRect m_selection;
    QCString m_data;
    QCString m_dataRedo;
    QValueList<columnSize> m_lstColumn;
    QValueList<columnSize> m_lstRedoColumn;
    QValueList<rowSize> m_lstRow;
    QValueList<rowSize> m_lstRedoRow;
    QString m_sheetName;
};

class KSpreadUndoDragDrop : public KSpreadUndoAction
{
public:
    KSpreadUndoDragDrop( KSpreadDoc * _doc, KSpreadSheet * _sheet, const QRect & _source, const QRect & _target );
    virtual ~KSpreadUndoDragDrop();

    virtual void undo();
    virtual void redo();

protected:
    QRect    m_selectionSource;
    QRect    m_selectionTarget;
    QCString m_dataSource;
    QCString m_dataTarget;
    QCString m_dataRedoSource;
    QCString m_dataRedoTarget;
    QString  m_sheetName;

    void saveCellRect( QCString & cells, KSpreadSheet * sheet,
                       QRect const & rect );
};

class KSpreadUndoResizeColRow : public KSpreadUndoAction
{
public:
    KSpreadUndoResizeColRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_selection );
    virtual ~KSpreadUndoResizeColRow();

    virtual void undo();
    virtual void redo();

protected:
    void createList( QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadSheet* sheet );

    QRect m_rctRect;
    QValueList<columnSize> m_lstColumn;
    QValueList<columnSize> m_lstRedoColumn;
    QValueList<rowSize> m_lstRow;
    QValueList<rowSize> m_lstRedoRow;
    QString m_sheetName;
};

class KSpreadUndoChangeAreaTextCell : public KSpreadUndoAction
{
public:
    KSpreadUndoChangeAreaTextCell( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_selection );
    virtual ~KSpreadUndoChangeAreaTextCell();

    virtual void undo();
    virtual void redo();

protected:
    void createList( QValueList<textOfCell> &list, KSpreadSheet* sheet );

    QRect m_rctRect;
    QValueList<textOfCell> m_lstTextCell;
    QValueList<textOfCell> m_lstRedoTextCell;
    QString m_sheetName;
};

class KSpreadUndoSort : public KSpreadUndoAction
{
public:
    KSpreadUndoSort( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_selection);
    virtual ~KSpreadUndoSort();

    virtual void undo();
    virtual void redo();

protected:
    void copyAll( QValueList<layoutTextCell> & list, QValueList<layoutColumn> & listCol,
                  QValueList<layoutRow> & listRow, KSpreadSheet * sheet );

    QRect m_rctRect;
    QValueList<layoutTextCell> m_lstFormats;
    QValueList<layoutTextCell> m_lstRedoFormats;
    QValueList<layoutColumn> m_lstColFormats;
    QValueList<layoutColumn> m_lstRedoColFormats;
    QValueList<layoutRow> m_lstRowFormats;
    QValueList<layoutRow> m_lstRedoRowFormats;

    QString m_sheetName;
};

class KSpreadUndoMergedCell : public KSpreadUndoAction
{
public:
    KSpreadUndoMergedCell( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column, int _row, int _extraX,int _extraY);
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
    QString m_sheetName;
};


class KSpreadUndoAutofill : public KSpreadUndoAction
{
public:
    KSpreadUndoAutofill( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_rect );
    virtual ~KSpreadUndoAutofill();

    virtual void undo();
    virtual void redo();
protected:
    void createListCell( QCString &list, KSpreadSheet* sheet );
    QRect m_selection;
    QCString m_data;
    QCString m_dataRedo;
    QString m_sheetName;
};

class KSpreadUndoInsertCellCol : public KSpreadUndoInsertRemoveAction
{
public:
    KSpreadUndoInsertCellCol( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_rect );
    virtual ~KSpreadUndoInsertCellCol();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    QRect m_rect;
};

class KSpreadUndoInsertCellRow : public KSpreadUndoInsertRemoveAction
{
public:
    KSpreadUndoInsertCellRow( KSpreadDoc *_doc, KSpreadSheet *_sheet,const QRect &_rect );
    virtual ~KSpreadUndoInsertCellRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    QRect m_rect;
};

class KSpreadUndoRemoveCellCol : public KSpreadUndoInsertRemoveAction
{
public:
    KSpreadUndoRemoveCellCol( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_rect );
    virtual ~KSpreadUndoRemoveCellCol();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    QRect m_rect;
    QCString m_data;
};

class KSpreadUndoRemoveCellRow : public KSpreadUndoInsertRemoveAction
{
public:
    KSpreadUndoRemoveCellRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_rect );
    virtual ~KSpreadUndoRemoveCellRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    QRect m_rect;
    QCString m_data;
};

class KSpreadUndoConditional : public KSpreadUndoAction
{
public:
    KSpreadUndoConditional( KSpreadDoc *_doc, KSpreadSheet *_sheet, QRect const & _rect );
    virtual ~KSpreadUndoConditional();

    virtual void undo();
    virtual void redo();
protected:
    void createListCell( QCString &list, KSpreadSheet* sheet );
    QRect m_selection;
    QCString m_data;
    QCString m_dataRedo;
    QString m_sheetName;
};

class KSpreadUndoCellPaste : public KSpreadUndoAction
{
public:
    KSpreadUndoCellPaste( KSpreadDoc *_doc, KSpreadSheet *_sheet,int _nbCol,int _nbRow, int _xshift,int _yshift, QRect &_selection,bool insert,int insertTo=0 );
    virtual ~KSpreadUndoCellPaste();

    virtual void undo();
    virtual void redo();

protected:
    void createListCell( QCString &listCell,QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadSheet* sheet );

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
    bool  b_insert;
    int m_iInsertTo;
    QString m_sheetName;
};


class KSpreadUndoStyleCell : public KSpreadUndoAction
{
public:
    KSpreadUndoStyleCell( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_rect );
    virtual ~KSpreadUndoStyleCell();

    virtual void undo();
    virtual void redo();

protected:
    void createListCell( QValueList<styleCell> &listCell, KSpreadSheet* sheet );
    QRect m_selection;
    QValueList<styleCell> m_lstStyleCell;
    QValueList<styleCell> m_lstRedoStyleCell;
    QString m_sheetName;
};

class KSpreadUndoInsertData : public KSpreadUndoChangeAreaTextCell
{
 public:
    KSpreadUndoInsertData( KSpreadDoc * _doc, KSpreadSheet * _sheet, QRect & _selection );
};


class KSpreadUndo
{
public:
    KSpreadUndo( KSpreadDoc *_doc );
    ~KSpreadUndo();

    void undo();
    void redo();
    void clear();

    void lock();
    void unlock();
    bool isLocked() const ;

    bool hasUndoActions()const { return !m_stckUndo.isEmpty(); }
    bool hasRedoActions()const { return !m_stckRedo.isEmpty(); }

    void appendUndo( KSpreadUndoAction *_action );

    QString getUndoName();
    QString getRedoName();

protected:
    QPtrStack<KSpreadUndoAction> m_stckUndo;
    QPtrStack<KSpreadUndoAction> m_stckRedo;

    KSpreadDoc *m_pDoc;
};

#endif
