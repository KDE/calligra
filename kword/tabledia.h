/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef tabledia_h
#define tabledia_h

#include <kdialogbase.h>
#include <qlist.h>

#include "defs.h"

class KWCanvas;
class KWDocument;
class QGridLayout;
class QLabel;
class QListBox;
class QWidget;
class QCheckBox;
class QButtonGroup;
class QComboBox;

/******************************************************************/
/* Class: KWTablePreview                                          */
/******************************************************************/

class KWTablePreview : public QWidget
{
    Q_OBJECT

public:
    KWTablePreview( QWidget *_parent, int _rows, int _cols )
        : QWidget( _parent ), rows( _rows ), cols( _cols ) {}

    void setRows( int _rows ) { rows = _rows; repaint( true ); }
    void setCols( int _cols ) { cols = _cols; repaint( true ); }

protected:
    void paintEvent( QPaintEvent *e );

    int rows, cols;

};

/******************************************************************/
/* Class: KWTableDia                                              */
/******************************************************************/

class KWTableDia : public KDialogBase
{
    Q_OBJECT

public:
    KWTableDia( QWidget *parent, const char *name, KWCanvas *_canvas, KWDocument *_doc,
		int rows, int cols, KWTblCellSize wid, KWTblCellSize hei );

protected:
    bool insertTable();
    void readTableStyles();
    void setupTab1( int rows, int cols, KWTblCellSize wid, KWTblCellSize hei );
    void setupTab2();

    QWidget *tab1;
    QLabel *lRows, *lCols, *lWid, *lHei;
    QSpinBox *nRows, *nCols;
    KWTablePreview *preview;
    QCheckBox *cbIsFloating;

    QWidget *tab2;
    QWidget *preview2;
    QComboBox *cWid, *cHei;
    QLabel *lStyles;
    QListBox *lbStyles;
    QCheckBox *cbHeaderOnAllPages;
    QButtonGroup *bgHeader, *bgFirstCol, *bgBody;
    QCheckBox *cbHBorder, *cbHBack, *cbHFormat;
    QCheckBox *cbFCBorder, *cbFCBack, *cbFCFormat;
    QCheckBox *cbBodyBorder, *cbBodyBack, *cbBodyFormat;

    struct TableStyle {
        bool hasHeader, hasFirstCol;

        Border hTop, hBottom, hRight, hLeft;
        QBrush hBack;
        Border frTop, frBottom, frRigfrt, frLeft;
        QBrush frBack;
        Border bTop, bBottom, bRigbt, bLeft;
        QBrush bBack;
    };

    QList<TableStyle> tableStyles;

    KWCanvas *canvas;
    KWDocument *doc;

protected slots:
    void rowsChanged( int );
    void colsChanged( int );
    virtual void slotOk();
    virtual void slotCancel();
};

#endif


