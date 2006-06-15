/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

   This library is free software; you can redistribute it and/m_or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, m_or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY m_or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef framedia_h
#define framedia_h

#include "defs.h"
#include "KWTextFrameSet.h"
#include <kpagedialog.h>
#include <KoUnit.h>
#include <KoUnitWidgets.h>
#include <q3groupbox.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <Q3GridLayout>
#include <Q3PtrList>
#include <Q3Frame>
#include <QLabel>
#include <QMouseEvent>

class KoBorderPreview;
class KWFrame;
class KWDocument;
class KWPage;
class QWidget;
class Q3GridLayout;
class Q3GroupBox;
class QRadioButton;
class QCheckBox;
class QLabel;
class QCloseEvent;
class Q3ListView;
class Q3ListViewItem;
class QLineEdit;
class KColorButton;
class KDoubleNumInput;

/******************************************************************/
/* Class: KWBrushStylePreview                                     */
/******************************************************************/
class KWBrushStylePreview : public Q3Frame
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
class KWFourSideConfigWidget : public Q3GroupBox
{
    Q_OBJECT
public:
    KWFourSideConfigWidget( KWDocument* m_doc, const QString& title, QWidget*, const char* name = 0 );

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
    KWDocument *m_doc;
    bool m_changed;
    bool m_noSignal;
};

/******************************************************************/
/* Class: KWFrameDia                                              */
/******************************************************************/
class KWFrameDia : public KPageDialog
{
    Q_OBJECT
public:
    /** Contructor when the dialog is used on creation of m_frame */
    KWFrameDia( QWidget *parent, KWFrame *_frame,KWDocument *_doc,FrameSetType _ft);
    KWFrameDia( QWidget *parent, KWFrame *_frame);
    KWFrameDia( QWidget *parent, Q3PtrList<KWFrame> allFrames);
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
    void connectListSelected( Q3ListViewItem * );
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
    QWidget *m_tab1;
    Q3GridLayout *m_grid1, *m_eofGrid, *m_onpGrid, *m_sideGrid;
    Q3GroupBox *m_endOfFrame, *m_onNewPage, *m_sideHeads;
    QCheckBox *m_cbCopy, *m_cbAspectRatio, *m_cbAllFrames;
    QCheckBox *m_cbProtectContent;
    QRadioButton *m_rResizeFrame, *m_rAppendFrame, *m_rNoShow, *m_reconnect, *m_noFollowup, *m_copyRadio;
    QLabel *sideTitle1, *sideTitle2;
    QLineEdit *m_sideWidth, *m_sideGap;
    QComboBox *m_sideAlign;

    // TAB 2:
    QWidget *m_tab2;
    QRadioButton *m_rRunNo, *m_rRunBounding, *m_rRunSkip;
    QRadioButton *m_rRunLeft, *m_rRunRight, *m_rRunBiggest;
    Q3GroupBox *m_runGroup, *m_runSideGroup;
    KWFourSideConfigWidget* m_raDistConfigWidget;

    // TAB 3:
    QWidget *m_tab3;
    QLineEdit *m_eFrameSetName;
    Q3ListView *m_lFrameSList;
    QRadioButton *m_rExistingFrameset, *m_rNewFrameset;


    // TAB 4:
    QWidget *m_tab4;
    Q3GroupBox *m_grp1;
    QLabel *m_lx, *m_ly, *m_lw, *m_lh;
    KoUnitDoubleSpinBox *m_sx, *m_sy, *m_sw, *m_sh;
    KWFourSideConfigWidget* m_paddingConfigWidget;
    QCheckBox *m_floating;
    QCheckBox *m_protectSize;

    // TAB 5:
    QWidget *m_tab5;

    // TAB 6:
    QWidget *m_tab6;
    QComboBox *m_cWidth, *m_cStyle;
    QPushButton *m_bLeft, *m_bRight, *m_bTop, *m_bBottom;
    KColorButton *m_bColor;
    KoBorder m_leftBorder, m_rightBorder, m_topBorder, m_bottomBorder;
    KoBorderPreview *m_prev3;

    //QComboBox *brushStyle;
    QCheckBox* m_transparentCB;
    KColorButton *m_brushColor;
    //KWBrushStylePreview *brushPreview;
    QCheckBox *m_overwriteColor;

    KWFrame *m_frame;
    KWFrame *m_mainFrame;
    Q3PtrList<KWFrame> m_allFrames;
    KWDocument *m_doc;
    FrameSetType m_frameType;
    bool m_frameSetFloating;
    bool m_frameSetProtectedSize;
    bool m_mainFrameSetIncluded;
    bool m_defaultFrameSetIncluded; // header, m_footer m_or m_footnote
    double m_oldX, m_oldY, m_oldW, m_oldH;
    double m_heightByWidthRatio;
    int /*FrameBehavior*/ m_frameBehavior;
    KoUnit::Unit m_frameUnits;
    QString m_oldFrameSetName;
    QBrush m_newBrushStyle;
    bool m_noSignal;
};

#endif
