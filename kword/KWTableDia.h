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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef tabledia_h
#define tabledia_h

#include <kpagedialog.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3GridLayout>
#include <QPaintEvent>

#include "defs.h"
#include <KoBorder.h>

class KWCanvas;
class KWDocument;
class KWTableTemplateSelector;

class Q3GridLayout;
class QLabel;
class Q3ListBox;
class QWidget;
class QCheckBox;
class Q3ButtonGroup;
class QComboBox;
class QSpinBox;

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

class KWTableDia : public KPageDialog
{
    Q_OBJECT

public:
    typedef int CellSize; // in fact KWTableFrameSet::CellSize, which is an enum
    enum UseMode{ NEW, EDIT };
    KWTableDia( QWidget *parent, const char *name, UseMode _useMode, KWCanvas *_canvas,
            KWDocument *_doc, int rows, int cols, CellSize wid, CellSize hei, bool floating,
            const QString & _templateName,  int format);

protected:
    void setupTab1( int rows, int cols, CellSize wid, CellSize hei, bool floating );
    void setupTab2( const QString &_templateName,  int format);

    QWidget *tab1;
    QLabel *lRows, *lCols;
#ifdef ALLOW_NON_INLINE_TABLES
    QLabel *lWid, *lHei;
    QComboBox *cHei, *cWid;
    QCheckBox *cbIsFloating;
#endif
    QSpinBox *nRows, *nCols;
    KWTablePreview *preview;
    QCheckBox *cbReapplyTemplate1, *cbReapplyTemplate2;

    Q3GridLayout *grid;
    KWTableTemplateSelector *tableTemplateSelector;

    UseMode m_useMode;
    KWCanvas *canvas;
    KWDocument *doc;

    int oldRowCount, oldColCount;
    QString oldTemplateName;
protected slots:
    void rowsChanged( int );
    void colsChanged( int );
    void slotSetReapply( bool );
    virtual void slotOk();
    void slotInlineTable( bool );
};

#endif


