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
class KWDocument;
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

/******************************************************************/
/* Class: KWFrameDia                                              */
/******************************************************************/

class KWFrameDia : public KDialogBase
{
    Q_OBJECT
public:
    KWFrameDia( QWidget *parent, KWFrame *_frame,KWDocument *_doc,FrameType _ft);
    KWFrameDia( QWidget *parent, KWFrame *_frame);
    //KWFrameDia( QWidget *parent, KWCanvas *_canvas);

    //void setCanvas(KWCanvas *_canvas) {canvas =_canvas;}

protected:
    void init();
    bool applyChanges();
    void setupTab1();
    void setupTab2();
    void setupTab3();
    void setupTab4();
    void uncheckAllRuns();
    void updateFrames();
    // TAB 1:
    QWidget *tab1;
    QGridLayout *grid1, *eofGrid, *onpGrid, *sideGrid;
    QGroupBox *endOfFrame, *onNewPage, *sideHeads;
    QCheckBox *autofit, *aspectRatio, *floating;
    QRadioButton *rResizeFrame, *rAppendFrame, *rNoShow, *reconnect, *noFollowup, *copyRadio;
    QLabel *sideTitle1, *sideTitle2;
    QLineEdit *sideWidth, *sideGap;
    QComboBox *sideAlign;

    // TAB 2:
    QWidget *tab2;
    QGridLayout *grid2, *runGrid;
    QRadioButton *rRunNo, *rRunBounding, *rRunContur;
    QLabel *runAroundLabel, *lRunNo, *lRunBounding, *lRunContur, *lRGap;
    QGroupBox *runGroup;
    QLineEdit *eRGap;

    // TAB 3:
    QWidget *tab3;
    QGridLayout *grid3;
    QLineEdit *eFrameSetName;
    QLabel *lFrameSet;
    QListView *lFrameSList;


    // TAB 4:
    QWidget *tab4;
    QGroupBox *grp1, *grp2;
    QGridLayout *grid4, *pGrid, *mGrid;
    QLabel *lx, *lml, *lmr, *lmt, *lmb, *ly, *lw, *lh;
    QLineEdit *sx, *sy, *sw, *sh, *sml, *smr, *smt, *smb;

    KWFrame *frame;
    KWDocument *doc;
    //KWCanvas *canvas;
    FrameType frameType;

    double oldX, oldY, oldW, oldH;

    FrameBehaviour frameBehaviour;
    KWUnit::Unit frameUnits;
    QString oldFrameName;

protected slots:
    void runNoClicked();
    void runBoundingClicked();
    void runConturClicked();
    virtual void slotOk();
    void connectListSelected( QListViewItem * );
    void setFrameBehaviourInputOn();
    void setFrameBehaviourInputOff();
};

#endif


