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

#ifndef paragdia_h
#define paragdia_h

#include <kdialogbase.h>
#include <qstringlist.h>
#include <qlist.h>
#include <koRuler.h>
#include <qspinbox.h>

#include "kwtextparag.h"
#include "counter.h"

class KWDocument;
class QWidget;
class QGridLayout;
class QLabel;
class QGroupBox;
class QComboBox;
class QRadioButton;
class KButtonBox;
class QPushButton;
class KColorDialog;
class KColorButton;
class QButtonGroup;
class QListBox;
class QLineEdit;
class KWParagLayout;
class KWPagePreview;
class KWPagePreview2;
class KWBorderPreview;
class KWNumPreview;
class QCheckBox;

/******************************************************************/
/* Class: KWSpinBox                                               */
/******************************************************************/
class KWSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    enum counterType{ NONE,NUM,ALPHAB_L,ALPHAB_U,ROM_NUM_L,ROM_NUM_U};

    KWSpinBox( int minValue, int maxValue, int step = 1,
           QWidget * parent = 0, const char * name = 0 );
    KWSpinBox( QWidget * parent = 0, const char * name = 0 );
    virtual ~KWSpinBox();    
    virtual QString mapValueToText( int value );
    void setCounterType(counterType _type);
 private:
    counterType m_Etype;

};


/******************************************************************/
/* Class: KWParagDia                                              */
/******************************************************************/

class KWParagDia : public KDialogBase
{
    Q_OBJECT

public:
    static const int PD_SPACING = 1;
    static const int PD_ALIGN = 2;
    static const int PD_BORDERS = 4;
    static const int PD_NUMBERING = 8;
    static const int PD_TABS = 16;

    KWParagDia( QWidget*, const char*, QStringList _fontList, int _flags, KWDocument *_doc );
    ~KWParagDia();

    int getFlags() { return flags; }

    // Set values (in pt)
    void setLeftIndent( double _left );
    void setRightIndent( double _right );
    void setFirstLineIndent( double _first );
    void setSpaceAfterParag( double _after );
    void setSpaceBeforeParag( double _before );
    void setLineSpacing( double _spacing );

    void setAlign( int align );

    void setTabList( const KoTabulatorList & tabList );

    // Get values (in pt)
    double leftIndent() const;
    double rightIndent() const;
    double firstLineIndent() const;
    double spaceBeforeParag() const;
    double spaceAfterParag() const;
    double lineSpacing() const;

    int align() const;

    Border leftBorder() const { return m_leftBorder; }
    void setLeftBorder( Border _leftBorder ) { m_leftBorder = _leftBorder; updateBorders(); }
    Border rightBorder() const { return m_rightBorder; }
    void setRightBorder( Border _rightBorder ) { m_rightBorder = _rightBorder; updateBorders(); }
    Border topBorder() const { return m_topBorder; }
    void setTopBorder( Border _topBorder ) { m_topBorder = _topBorder; updateBorders(); }
    Border bottomBorder() const { return m_bottomBorder; }
    void setBottomBorder( Border _bottomBorder ) { m_bottomBorder = _bottomBorder; updateBorders(); }

    void setCounter( Counter _counter );
    Counter counter() const { return m_counter; }

    void setParagLayout( const KWParagLayout & lay );
    void setPageBreaking( bool b);


    KoTabulatorList tabListTabulator() const { return _tabList; }


    bool isAlignChanged() const {return oldLayout.alignment!=align();}
    bool listTabulatorChanged() const {return oldLayout.tabList()!=tabListTabulator();}

    bool linesTogether() const ;

    bool isPageBreakingChanged() const { return  m_bPageBreakingChanged;}

    bool isLineSpacingChanged() const {return oldLayout.lineSpacing!=lineSpacing();}
    bool isLeftMarginChanged() const { return oldLayout.margins[QStyleSheetItem::MarginLeft]!=leftIndent(); }
    bool isRightMarginChanged() const { return oldLayout.margins[QStyleSheetItem::MarginRight]!=rightIndent();}
    bool isFirstLineChanged() const {return oldLayout.margins[ QStyleSheetItem::MarginFirstLine]!=firstLineIndent();}
    bool isSpaceBeforeChanged() const { return oldLayout.margins[QStyleSheetItem::MarginTop]!=spaceBeforeParag();}
    bool isSpaceAfterChanged() const {return oldLayout.margins[QStyleSheetItem::MarginBottom]!=spaceAfterParag();}
    bool isBulletChanged() const;

    bool isBorderChanged() const { return (oldLayout.leftBorder!=leftBorder() ||
					 oldLayout.rightBorder!=rightBorder() ||
					 oldLayout.topBorder!=topBorder() ||
					 oldLayout.bottomBorder!=bottomBorder() ); }
    //necessary when you used just border dialog
    void setAfterInitBorder(bool _b){m_bAfterInitBorder=_b;}
    void changeKWSpinboxType();
protected:
    void setupTab1();
    void setupTab2();
    void setupTab3();
    void setupTab4();
    void setupTab5();
    void clearAligns();
    void updateBorders();
    void setActiveItem(double value);
    bool findExistingValue(double val);
//    void enableUIForCounterType();

    // Tab4 data.
    QButtonGroup *gNumbering;
    QButtonGroup *gStyle;
    QLineEdit *eCustomNum;
    QLineEdit *ecLeft, *ecRight/*, *eStart*/;
    KWSpinBox* eStart;
    QLabel *lStart;
    QRadioButton *rDisc, *rSquare, *rCircle, *rCustom;
    QPushButton *bBullets;
    QSpinBox *sDepth;

    QGridLayout *indentGrid, *spacingGrid,
        *pSpaceGrid, *tabGrid, *endFramePageGrid;
    QLineEdit *eLeft, *eRight, *eFirstLine, *eSpacing, *eBefore, *eAfter, *eTabPos;
    QLabel *lLeft, *lRight, *lFirstLine, *lBefore, *lAfter, *lAlign, *lStyle, *lWidth, *lColor;
    QGroupBox *indentFrame, *spacingFrame, *pSpaceFrame, *gText, *endFramePage;
    QComboBox *cSpacing, *cStyle, *cWidth;

    QCheckBox *cEndOfFramePage;

    QRadioButton *rLeft, *rCenter, *rRight, *rJustify;
    KWPagePreview *prev1;
    KWPagePreview2 *prev2;
    QPushButton *bLeft, *bRight, *bTop, *bBottom, *bFont, *bAdd, *bDel, *bModify;
    KWBorderPreview *prev3;
    KColorButton *bColor;
    QButtonGroup *g2, *g3;
    KWNumPreview *prev4;
    QListBox *lTabs;
    QLabel *lTab;
    QRadioButton *rtLeft, *rtCenter, *rtRight, *rtDecimal;


    Border m_leftBorder, m_rightBorder, m_topBorder, m_bottomBorder;
    int flags;
    Counter m_counter;
    QStringList fontList;
    KWDocument *doc;
    KoTabulatorList _tabList;
    KWParagLayout oldLayout;

    bool m_bAfterInitBorder;
    bool m_bPageBreakingChanged;

protected slots:
    void slotPressEvent(QMouseEvent *_ev);
    void leftChanged( const QString & );
    void rightChanged( const QString & );
    void firstChanged( const QString & );
    void spacingActivated( int );
    void spacingChanged( const QString & );
    void beforeChanged( const QString & );
    void afterChanged( const QString & );
    void alignLeft();
    void alignCenter();
    void alignRight();
    void alignJustify();
    void brdLeftToggled( bool );
    void brdRightToggled( bool );
    void brdTopToggled( bool );
    void brdBottomToggled( bool );
    void brdStyleChanged( const QString & );
    void brdWidthChanged( const QString & );
    void brdColorChanged( const QColor& );

    // Tab 4 slots.
    void numChangeBullet();
    void numStyleChanged( int );
    void numCounterDefChanged( const QString& );
    void numTypeChanged( int );
    void numLeftTextChanged( const QString & );
    void numRightTextChanged( const QString & );
    void numStartChanged( const QString & );
    void numDepthChanged( int );

    void addClicked();
    void modifyClicked();
    void delClicked();
    void slotDoubleClicked( QListBoxItem * );
    void slotEndOfFramePageChanged();
};

#endif
