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

#include "../lib/kotext/koParagDia_p.h"
#include <kdialogbase.h>
#include <koUnit.h>
#include <koUnitWidgets.h>
#include <qgroupbox.h>

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
    KWBrushStylePreview( QWidget* );
    ~KWBrushStylePreview() {}
    void setBrush( const QBrush& _brush ) { brush = _brush; }
protected:
    void drawContents( QPainter* );
private:
    QBrush brush;
};

/**
 * A widget showing unitwidgets for 4 doubles (for left/right/top/bottom values)
 * and a checkbox to synchronize changes to all values
 */
class KWFourSideConfigWidget : public QGroupBox
{
    Q_OBJECT
public:
    KWFourSideConfigWidget( KWDocument* doc, const QString& title, QWidget*, const char* name = 0 );

    bool changed() const { return m_changed; }

    void setValues( double left, double right, double top, double bottom ); // in pt
    double leftValue() const; // in pt
    double rightValue() const; // in pt
    double topValue() const; // in pt
    double bottomValue() const; // in pt

signals:

public slots:
    void slotValueChanged( double );

private:
    KoUnitDoubleSpinBox *m_inputLeft, *m_inputRight, *m_inputTop, *m_inputBottom;
    QCheckBox *m_synchronize;
    KWDocument *doc;
    bool m_changed;
    bool noSignal;
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
    void setupTab6();
    void initBorderSettings();
    void initGeometrySettings();
    
    void updateFrames();
    void enableOnNewPageOptions();
    void initBrush();
    bool mayDeleteFrameSet(KWTextFrameSet *fs);
    void enableSizeAndPosition();

    QBrush frameBrushStyle() const;
    void calcRatio();

protected slots:
    virtual void slotApply();
    virtual void slotOk();
    void connectListSelected( QListViewItem * );
    void setFrameBehaviorInputOn();
    void setFrameBehaviorInputOff();
    void slotFloatingToggled(bool);
    void slotProtectSizeToggled(bool);
    void textNameFrameChanged ( const QString & );
    //void updateBrushPreview();
    void slotProtectContentChanged( bool );
    void enableRunAround();
    void selectExistingFrameset();
    void selectNewFrameset(bool on);
    void slotUpdateWidthForHeight(double height);
    void slotUpdateHeightForWidth( double width );
    void slotKeepRatioToggled(bool on);
    void ensureValidFramesetSelected();
    void brdLeftToggled( bool );
    void brdRightToggled( bool );
    void brdTopToggled( bool );
    void brdBottomToggled( bool );
    void slotPressEvent(QMouseEvent *_ev);
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
    KWFourSideConfigWidget* m_raDistConfigWidget;

    // TAB 3:
    QWidget *tab3;
    QLineEdit *eFrameSetName;
    QListView *lFrameSList;
    QRadioButton *rExistingFrameset, *rNewFrameset;


    // TAB 4:
    QWidget *tab4;
    QGroupBox *grp1;
    QLabel *lx, *ly, *lw, *lh;
    KoUnitDoubleSpinBox *sx, *sy, *sw, *sh;
    KWFourSideConfigWidget* m_paddingConfigWidget;
    QCheckBox *floating;
    QCheckBox *protectSize;

    // TAB 5:
    QWidget *tab5;

    // TAB 6:
    QWidget *tab6;
    QComboBox *cWidth, *cStyle;
    QPushButton *bLeft, *bRight, *bTop, *bBottom;
    KColorButton *bColor;
    KoBorder m_leftBorder, m_rightBorder, m_topBorder, m_bottomBorder;
    KoBorderPreview *prev3;

    //QComboBox *brushStyle;
    QCheckBox* transparentCB;
    KColorButton *brushColor;
    //KWBrushStylePreview *brushPreview;
    QCheckBox *overwriteColor;

    KWFrame *frame;
    KWFrame *mainFrame;
    QPtrList<KWFrame> allFrames;
    KWDocument *doc;
    FrameSetType frameType;
    bool frameSetFloating;
    bool frameSetProtectedSize;
    bool m_mainFrameSetIncluded;
    bool m_defaultFrameSetIncluded; // header, footer or footnote
    double oldX, oldY, oldW, oldH;
    double heightByWidthRatio;
    int /*FrameBehavior*/ frameBehavior;
    KoUnit::Unit frameUnits;
    QString oldFrameSetName;
    QBrush newBrushStyle;
    bool noSignal;
};

#endif
