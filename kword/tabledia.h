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

#include <qtabdialog.h>
#include <qlist.h>

#include "paraglayout.h"
#include "format.h"
#include "defs.h"

class KWPage;
class KWordDocument;
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
/* Class: KWTableConf                                             */
/******************************************************************/

class KWTableConf : public QWidget
{
    Q_OBJECT

public:
    KWTableConf( QWidget *_parent, KWordDocument *_doc );

protected:
    struct TableStyle {
        bool hasHeader, hasFirstCol;

        KWParagLayout::Border hTop, hBottom, hRight, hLeft;
        QBrush hBack;
        KWParagLayout::Border frTop, frBottom, frRigfrt, frLeft;
        QBrush frBack;
        KWParagLayout::Border bTop, bBottom, bRigbt, bLeft;
        QBrush bBack;

        KWFormat header, firstRow, Body;
    };

    void readTableStyles();
    void setupPage();

    QGridLayout *grid1;
    QLabel *lStyles;
    QListBox *lbStyles;
    QWidget *preview;
    QCheckBox *cbHeaderOnAllPages;
    QButtonGroup *bgHeader, *bgFirstCol, *bgBody;
    QCheckBox *cbHBorder, *cbHBack, *cbHFormat;
    QCheckBox *cbFCBorder, *cbFCBack, *cbFCFormat;
    QCheckBox *cbBodyBorder, *cbBodyBack, *cbBodyFormat;

    KWordDocument *doc;
    QList<TableStyle> tableStyles;

};

/******************************************************************/
/* Class: KWTableDia                                              */
/******************************************************************/

class KWTableDia : public QTabDialog
{
    Q_OBJECT

public:
    KWTableDia( QWidget *parent, const char *name, KWPage *_page, KWordDocument *_doc,
		int rows, int cols, KWTblCellSize wid, KWTblCellSize hei );

protected:
    void setupTab1( int rows, int cols, KWTblCellSize wid, KWTblCellSize hei );
    void setupTab2();
    void closeEvent( QCloseEvent * ) { emit cancelButtonPressed(); }

    QWidget *tab1;
    QGridLayout *grid1;
    QLabel *lRows, *lCols, *lWid, *lHei;
    QSpinBox *nRows, *nCols;
    KWTablePreview *preview;
    KWTableConf *tab2;
    QComboBox *cWid, *cHei;

    KWPage *page;
    KWordDocument *doc;

protected slots:
    void insertTable();
    void rowsChanged( int );
    void colsChanged( int );

};

#endif


