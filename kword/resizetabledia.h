/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#ifndef resizetabledia_h
#define resizetabledia_h

#include <kdialogbase.h>

class KWTableFrameSet;
class KWDocument;
class KWCanvas;
class QWidget;
class QGridLayout;
class QButtonGroup;
class QRadioButton;
class QLabel;
class QSpinBox;
class KoUnitDoubleSpinBox;

class KWResizeTableDia : public KDialogBase
{
    Q_OBJECT
public:
    enum ResizeType {ROW, COL};
    KWResizeTableDia( QWidget *parent, const char *name, KWTableFrameSet *_grpMgr, KWDocument *_doc, ResizeType _type, KWCanvas *_canvas );

protected:
    void setupTab1();
    bool doResize();
    QSpinBox *value;
    KWTableFrameSet *table;
    KWDocument *doc;
    ResizeType type;
    KWCanvas *canvas;
    KoUnitDoubleSpinBox *position;
protected slots:
    virtual void slotOk();
    virtual void slotApply();
    void slotValueChanged( int pos);
};

#endif


