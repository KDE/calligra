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
#include <koUnit.h>

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
class KColorButton;
class KDoubleNumInput;

/******************************************************************/
/* Class: KWBrushStylePreview		                          */
/******************************************************************/
class KWBrushStylePreview : public QFrame
{
    Q_OBJECT
public:
    KWBrushStylePreview( QWidget*, const char* = 0L );
    ~KWBrushStylePreview() {}
    void setBrush(QBrush _brush){ brush=_brush;}
protected:
    void drawContents( QPainter* );
private:
    QBrush brush;
};


/******************************************************************/
/* Class: KWFrameDia                                              */
/******************************************************************/

class KWFrameDia : public KDialogBase
{
    Q_OBJECT
public:
    /** Contructor when the dialog is used on creation of frame */
    KWFrameDia( QWidget *parent, KWFrame *_frame,KWDocument *_doc,FrameSetType _ft);
    KWFrameDia( QWidget *parent, KWFrame *_frame);
    KWFrameDia( QWidget *parent, QPtrList<KWFrame> allFrames);
    //KWFrameDia( QWidget *parent, KWCanvas *_canvas);

    //void setCanvas(KWCanvas *_canvas) {canvas =_canvas;}

protected:
    void init();
    bool applyChanges();
    void setupTab1();
    void setupTab2();
    void setupTab3();
    void setupTab4();
    void setupTab5();
    void updateFrames();
    void enableOnNewPageOptions();
    void initComboStyleBrush();
    bool mayDeleteFrameSet(KWTextFrameSet *fs);

    QBrush frameBrushStyle();

protected slots:
    virtual void slotOk();
    void connectListSelected( QListViewItem * );
    void setFrameBehaviorInputOn();
    void setFrameBehaviorInputOff();
    void slotFloatingToggled(bool);
    void slotProtectSizeToggled(bool);
    void textNameFrameChanged ( const QString & );
    void updateBrushConfiguration();
    void slotMarginsChanged( double );
    void slotProtectContentChanged( bool );
    void enableRunAround();
    void selectExistingFrameset();
    void selectNewFrameset(bool on);

private:
    // TAB 1:
    QWidget *tab1;
    QGridLayout *grid1, *eofGrid, *onpGrid, *sideGrid;
    QGroupBox *endOfFrame, *onNewPage, *sideHeads;
    QCheckBox *cbCopy, *cbAspectRatio, *cbAllFrames;
    QCheckBox *cbProtectContent;
    QRadioButton *rResizeFrame, *rAppendFrame, *rNoShow, *reconnect, *noFollowup, *copyRadio;
    QLabel *sideTitle1, *sideTitle2;
    QLineEdit *sideWidth, *sideGap;
    QComboBox *sideAlign;

    // TAB 2:
    QWidget *tab2;
    QRadioButton *rRunNo, *rRunBounding, *rRunSkip;
    QRadioButton *rRunLeft, *rRunRight, *rRunBiggest;
    QGroupBox *runGroup, *runSideGroup;
    KDoubleNumInput *eRGap;

    // TAB 3:
    QWidget *tab3;
    QLineEdit *eFrameSetName;
    QListView *lFrameSList;
    QRadioButton *rExistingFrameset, *rNewFrameset;


    // TAB 4:
    QWidget *tab4;
    QGroupBox *grp1, *grp2;
    QGridLayout *grid4, *pGrid, *mGrid;
    QLabel *lx, *lml, *lmr, *lmt, *lmb, *ly, *lw, *lh;
    KDoubleNumInput *sx, *sy, *sw, *sh;
    KDoubleNumInput *sml, *smr, *smt, *smb;
    QCheckBox *synchronize;
    QCheckBox *floating;
    QCheckBox *protectSize;
    // TAB 5:
    QWidget *tab5;
    QGridLayout *grid5;
    QComboBox *brushStyle;
    KColorButton *brushColor;
    KWBrushStylePreview *brushPreview;
    QCheckBox *overwriteColor;

    KWFrame *frame;
    QPtrList<KWFrame> allFrames;
    KWDocument *doc;
    FrameSetType frameType;
    bool frameSetFloating;
    bool frameSetProtectedSize;
    double oldX, oldY, oldW, oldH;
    double oldMarginLeft, oldMarginRight, oldMarginTop, oldMarginBottom;

    int /*FrameBehavior*/ frameBehavior;
    KoUnit::Unit frameUnits;
    QString oldFrameSetName;
    QBrush newBrushStyle;
    bool noSignal;
};

#endif


