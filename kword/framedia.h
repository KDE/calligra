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

#ifndef framedia_h
#define framedia_h

#include <kdialogbase.h>

class KWFrame;
class KWordDocument;
class KWPage;
class QWidget;
class QGridLayout;
class QGroupBox;
class QRadioButton;
class QCheckBox;
class QLabel;
class QCloseEvent;
class QListView;
class QListViewItem;
class QLineEdit;

static const int FD_FRAME_SET      = 1;
static const int FD_FRAME          = 2;
static const int FD_FRAME_CONNECT  = 4;
static const int FD_PLUS_NEW_FRAME = 8;
static const int FD_GEOMETRY       = 16;
static const int FD_BORDERS        = 32;

/******************************************************************/
/* Class: KWFrameDia                                              */
/******************************************************************/

class KWFrameDia : public KDialogBase
{
    Q_OBJECT
public:
    KWFrameDia( QWidget *parent, const char *name, KWFrame *_frame, KWordDocument *_doc,
                KWPage *_page, int _flags, KWFrameSet *fs = 0 );

protected:
    bool applyChanges();
    void setupTab1TextFrameSet();
    void setupTab2TextFrame();
    void setupTab3ConnectTextFrames();
    void setupTab4Geometry();
    void uncheckAllRuns();

    QWidget *tab1, *tab2, *tab3, *tab4;
    QGridLayout *grid1, *grid2, *grid3, *grid4, *runGrid, *pGrid, *mGrid;
    QGroupBox *runGroup, *grp1, *grp2;
    QRadioButton *rRunNo, *rRunBounding, *rRunContur, *rResizeFrame, *rAppendFrame, *rNoShow;
    QCheckBox *cAutoCreateFrame;
    QLabel *lRunNo, *lRunBounding, *lRunContur, *lRGap, *lFrameSet, *lNewFrame;
    QLabel *lx, *ly, *lw, *lh, *lml, *lmr, *lmt, *lmb;
    QListView *lFrameSList;
    QLineEdit *sx, *sy, *sw, *sh, *sml, *smr, *smt, *smb;
    QLineEdit *eFrameSetName;

    KWFrame *frame;
    QLineEdit *eRGap;
    int flags;
    KWordDocument *doc;
    KWPage *page;
    KWFrameSet *frameset;

    float oldX, oldY, oldW, oldH;

signals:
    void changed();

protected slots:
    void runNoClicked();
    void runBoundingClicked();
    void runConturClicked();
    virtual void slotOk();
    void connectListSelected( QListViewItem * );
};

#endif


