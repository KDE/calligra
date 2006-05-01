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

#include <QLinkedList>
#include <QList>
#include <QMap>
#include <QRect>
#include <QStack>
#include <QString>
//Added by qt3to4:
#include <Q3CString>

#include <KoUnit.h>
#include <KoPageLayout.h>

#include "kspread_doc.h"
#include "region.h"

namespace KSpread
{
class ColumnFormat;
class Doc;
class Format;
class Region;
class RowFormat;
class Sheet;
class Undo;
class UndoResizeColRow;

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
Format * l;
QString text;
};

struct layoutCell {
int row;
int col;
Format *l;
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
class UndoAction
{
public:
    UndoAction( Doc *_doc ) { m_pDoc = _doc; m_pDoc->setModified(true); }
    virtual ~UndoAction() { }

    virtual void undo() = 0;
    virtual void redo() = 0;

    Doc* doc()const { return m_pDoc; }

    QString getName()const {return name ;}

protected:
    Doc *m_pDoc;
    QString name;
};

class MacroUndoAction : public UndoAction
{
public:
    MacroUndoAction( Doc * _doc, const QString & _name );
    virtual ~MacroUndoAction();

    void addCommand(UndoAction *command);

    virtual void undo();
    virtual void redo();

protected:
    QList<UndoAction*> m_commands;
};

class UndoInsertRemoveAction : public UndoAction
{
public:
    UndoInsertRemoveAction( Doc *_doc );
    virtual ~UndoInsertRemoveAction();

    void saveFormulaReference( Sheet *_sheet, int col, int row, QString & formula );

protected:
    void undoFormulaReference();
    QLinkedList<FormulaOfCell> m_lstFormulaCells;
};

class UndoRemoveColumn : public UndoInsertRemoveAction
{
public:
    UndoRemoveColumn( Doc *_doc, Sheet *_sheet, int _column,int _nbCol=0 );
    virtual ~UndoRemoveColumn();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    Q3CString m_data;
    int m_iColumn;
    int m_iNbCol;
    QRect m_printRange;
    QPair<int, int> m_printRepeatColumns;
};

class UndoInsertColumn : public UndoInsertRemoveAction
{
public:
    UndoInsertColumn( Doc *_doc, Sheet *_sheet, int _column,int _nbCol=0 );
    virtual ~UndoInsertColumn();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    int m_iColumn;
    int m_iNbCol;
};

class UndoRemoveRow : public UndoInsertRemoveAction
{
public:
    UndoRemoveRow( Doc *_doc, Sheet *_sheet, int _row,int _nbRow=0 );
    virtual ~UndoRemoveRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    Q3CString m_data;
    int m_iRow;
    int m_iNbRow;
    QRect m_printRange;
    QPair<int, int> m_printRepeatRows;
};

class UndoInsertRow : public UndoInsertRemoveAction
{
public:
    UndoInsertRow( Doc *_doc, Sheet *_sheet, int _row,int _nbRow=0 );
    virtual ~UndoInsertRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    int m_iRow;
    int m_iNbRow;
};


class UndoHideColumn : public UndoAction
{
public:
    UndoHideColumn( Doc *_doc, Sheet *_sheet, int _column,int _nbCol=0, QLinkedList<int>listCol=QLinkedList<int>() );
    virtual ~UndoHideColumn();

    virtual void undo();
    virtual void redo();
    void createList( QLinkedList<int>&list,Sheet *_tab );

protected:
    QString m_sheetName;
    int m_iColumn;
    int m_iNbCol;
    QLinkedList<int> listCol;
};

class UndoHideRow : public UndoAction
{
public:
    UndoHideRow( Doc *_doc, Sheet *_sheet, int _column,int _nbCol=0, QLinkedList<int>_listRow=QLinkedList<int>() );
    virtual ~UndoHideRow();

    virtual void undo();
    virtual void redo();
protected:
    void createList( QLinkedList<int>&list,Sheet *_tab );

    QString m_sheetName;
    int m_iRow;
    int m_iNbRow;
    QLinkedList<int> listRow;
};

class UndoShowColumn : public UndoAction
{
public:
    UndoShowColumn( Doc *_doc, Sheet *_sheet, int _column,int _nbCol=0, QLinkedList<int>_list=QLinkedList<int>() );
    virtual ~UndoShowColumn();

    virtual void undo();
    virtual void redo();
protected:
    void createList( QLinkedList<int>&list,Sheet *_tab );

    QString m_sheetName;
    int m_iColumn;
    int m_iNbCol;
    QLinkedList<int> listCol;
};

class UndoShowRow : public UndoAction
{
public:
    UndoShowRow( Doc *_doc, Sheet *_sheet, int _column,int _nbCol=0, QLinkedList<int>list=QLinkedList<int>() );
    virtual ~UndoShowRow();

    virtual void undo();
    virtual void redo();

protected:
    void createList( QLinkedList<int>&list,Sheet *_tab );
    QString m_sheetName;
    int m_iRow;
    int m_iNbRow;
    QLinkedList<int> listRow;
};


class UndoPaperLayout : public UndoAction
{
public:
    UndoPaperLayout( Doc *_doc, Sheet *_sheet );
    virtual ~UndoPaperLayout();

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


class UndoSetText : public UndoAction
{
public:
    UndoSetText( Doc *_doc, Sheet *_sheet, const QString& _text, int _column, int _row, FormatType _formatType );
    virtual ~UndoSetText();

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

class UndoCellFormat : public UndoAction
{
public:
    UndoCellFormat( Doc *_doc, Sheet *_sheet, const Region &_selection, const QString &_title );
    virtual ~UndoCellFormat();

    virtual void undo();
    virtual void redo();

protected:
    void copyFormat( QLinkedList<layoutCell> &list,QLinkedList<layoutColumn> &listCol,QLinkedList<layoutRow> &listRow, Sheet* sheet );

    Region m_region;
    QLinkedList<layoutCell> m_lstFormats;
    QLinkedList<layoutCell> m_lstRedoFormats;
    QLinkedList<layoutColumn> m_lstColFormats;
    QLinkedList<layoutColumn> m_lstRedoColFormats;
    QLinkedList<layoutRow> m_lstRowFormats;
    QLinkedList<layoutRow> m_lstRedoRowFormats;

    QString m_sheetName;
};

class UndoChangeAngle : public UndoAction
{
public:
    UndoChangeAngle( Doc *_doc, Sheet *_sheet, const Region &_selection );
    virtual ~UndoChangeAngle();

    virtual void undo();
    virtual void redo();

protected:

   UndoCellFormat* m_layoutUndo;
   UndoResizeColRow* m_resizeUndo;

};

class UndoDelete : public UndoAction
{
public:
    UndoDelete(Doc *_doc, Sheet *_sheet, const Region& region);
    virtual ~UndoDelete();

    virtual void undo();
    virtual void redo();

protected:
    void createListCell( Q3CString &listCell,QLinkedList<columnSize> &listCol,QLinkedList<rowSize> &listRow, Sheet* sheet );

    Region m_region;
    Q3CString m_data;
    Q3CString m_dataRedo;
    QLinkedList<columnSize> m_lstColumn;
    QLinkedList<columnSize> m_lstRedoColumn;
    QLinkedList<rowSize> m_lstRow;
    QLinkedList<rowSize> m_lstRedoRow;
    QString m_sheetName;
};

class UndoDragDrop : public UndoAction
{
public:
    UndoDragDrop( Doc * _doc, Sheet * _sheet, const Region& _source, const Region& _target );
    virtual ~UndoDragDrop();

    virtual void undo();
    virtual void redo();

protected:
    Region   m_selectionSource;
    Region   m_selectionTarget;
    Q3CString m_dataSource;
    Q3CString m_dataTarget;
    Q3CString m_dataRedoSource;
    Q3CString m_dataRedoTarget;
    QString  m_sheetName;

    void saveCellRect( Q3CString & cells, Sheet * sheet,
                       const Region& region );
};

class UndoResizeColRow : public UndoAction
{
public:
    UndoResizeColRow( Doc *_doc, Sheet *_sheet, const Region &_selection );
    virtual ~UndoResizeColRow();

    virtual void undo();
    virtual void redo();

protected:
    void createList( QLinkedList<columnSize> &listCol,QLinkedList<rowSize> &listRow, Sheet* sheet );

    Region m_region;
    QLinkedList<columnSize> m_lstColumn;
    QLinkedList<columnSize> m_lstRedoColumn;
    QLinkedList<rowSize> m_lstRow;
    QLinkedList<rowSize> m_lstRedoRow;
    QString m_sheetName;
};

class UndoChangeAreaTextCell : public UndoAction
{
public:
    UndoChangeAreaTextCell( Doc *_doc, Sheet *_sheet, const Region &_selection );
    virtual ~UndoChangeAreaTextCell();

    virtual void undo();
    virtual void redo();

protected:
    void createList( QMap<QPoint,QString> &list, Sheet* sheet );

    Region m_region;
    QMap<QPoint,QString> m_lstTextCell;
    QMap<QPoint,QString> m_lstRedoTextCell;
    QString m_sheetName;
};

class UndoSort : public UndoAction
{
public:
    UndoSort( Doc *_doc, Sheet *_sheet, const QRect &_selection);
    virtual ~UndoSort();

    virtual void undo();
    virtual void redo();

protected:
    void copyAll( QLinkedList<layoutTextCell> & list, QLinkedList<layoutColumn> & listCol,
                  QLinkedList<layoutRow> & listRow, Sheet * sheet );

    QRect m_rctRect;
    QLinkedList<layoutTextCell> m_lstFormats;
    QLinkedList<layoutTextCell> m_lstRedoFormats;
    QLinkedList<layoutColumn> m_lstColFormats;
    QLinkedList<layoutColumn> m_lstRedoColFormats;
    QLinkedList<layoutRow> m_lstRowFormats;
    QLinkedList<layoutRow> m_lstRedoRowFormats;

    QString m_sheetName;
};

class UndoMergedCell : public UndoAction
{
public:
    UndoMergedCell( Doc *_doc, Sheet *_sheet, int _column, int _row, int _extraX,int _extraY);
    virtual ~UndoMergedCell();

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


class UndoAutofill : public UndoAction
{
public:
    UndoAutofill( Doc *_doc, Sheet *_sheet, const QRect &_rect );
    virtual ~UndoAutofill();

    virtual void undo();
    virtual void redo();
protected:
    void createListCell( Q3CString &list, Sheet* sheet );
    QRect m_selection;
    Q3CString m_data;
    Q3CString m_dataRedo;
    QString m_sheetName;
};

class UndoInsertCellCol : public UndoInsertRemoveAction
{
public:
    UndoInsertCellCol( Doc *_doc, Sheet *_sheet, const QRect &_rect );
    virtual ~UndoInsertCellCol();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    QRect m_rect;
};

class UndoInsertCellRow : public UndoInsertRemoveAction
{
public:
    UndoInsertCellRow( Doc *_doc, Sheet *_sheet,const QRect &_rect );
    virtual ~UndoInsertCellRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    QRect m_rect;
};

class UndoRemoveCellCol : public UndoInsertRemoveAction
{
public:
    UndoRemoveCellCol( Doc *_doc, Sheet *_sheet, const QRect &_rect );
    virtual ~UndoRemoveCellCol();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    QRect m_rect;
    Q3CString m_data;
};

class UndoRemoveCellRow : public UndoInsertRemoveAction
{
public:
    UndoRemoveCellRow( Doc *_doc, Sheet *_sheet, const QRect &_rect );
    virtual ~UndoRemoveCellRow();

    virtual void undo();
    virtual void redo();

protected:
    QString m_sheetName;
    QRect m_rect;
    Q3CString m_data;
};

class UndoConditional : public UndoAction
{
public:
    UndoConditional( Doc *_doc, Sheet *_sheet, const Region & _selection );
    virtual ~UndoConditional();

    virtual void undo();
    virtual void redo();
protected:
    void createListCell( Q3CString &list, Sheet* sheet );
    Region m_region;
    Q3CString m_data;
    Q3CString m_dataRedo;
    QString m_sheetName;
};

class UndoCellPaste : public UndoAction
{
public:
    UndoCellPaste(Doc *_doc, Sheet *_sheet,
                  int _xshift, int _yshift,
                  const Region& _selection, bool insert, int insertTo = 0);
    virtual ~UndoCellPaste();

    virtual void undo();
    virtual void redo();

protected:
    void createListCell( Q3CString &listCell,QLinkedList<columnSize> &listCol,QLinkedList<rowSize> &listRow, Sheet* sheet );

    Region m_region;
    Q3CString m_data;
    Q3CString m_dataRedo;
    QLinkedList<columnSize> m_lstColumn;
    QLinkedList<columnSize> m_lstRedoColumn;
    QLinkedList<rowSize> m_lstRow;
    QLinkedList<rowSize> m_lstRedoRow;
    int xshift;
    int yshift;
    bool  b_insert;
    int m_iInsertTo;
    QString m_sheetName;
};


class UndoStyleCell : public UndoAction
{
public:
    UndoStyleCell( Doc *_doc, Sheet *_sheet, const QRect &_rect );
    virtual ~UndoStyleCell();

    virtual void undo();
    virtual void redo();

protected:
    void createListCell( QLinkedList<styleCell> &listCell, Sheet* sheet );
    QRect m_selection;
    QLinkedList<styleCell> m_lstStyleCell;
    QLinkedList<styleCell> m_lstRedoStyleCell;
    QString m_sheetName;
};

class UndoInsertData : public UndoChangeAreaTextCell
{
 public:
    UndoInsertData( Doc * _doc, Sheet * _sheet, QRect & _selection );
};


class Undo
{
public:
    Undo( Doc *_doc );
    ~Undo();

    void undo();
    void redo();
    void clear();

    void lock();
    void unlock();
    bool isLocked() const ;

    bool hasUndoActions()const { return !m_stckUndo.isEmpty(); }
    bool hasRedoActions()const { return !m_stckRedo.isEmpty(); }

    void appendUndo( UndoAction *_action );

    QString getUndoName();
    QString getRedoName();

protected:
    QStack<UndoAction*> m_stckUndo;
    QStack<UndoAction*> m_stckRedo;

    Doc *m_pDoc;
};

} // namespace KSpread

#endif
