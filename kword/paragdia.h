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
#include <qgroupbox.h>
#include <koRuler.h>

#include "kwtextparag.h"
#include "kwunit.h"

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
class QSpinBox;
class QButtonGroup;
class QListBox;
class QLineEdit;
class KWParagLayout;

/******************************************************************/
/* class KWPagePreview                                            */
/******************************************************************/

class KWPagePreview : public QGroupBox
{
    Q_OBJECT

public:
    KWPagePreview( QWidget*, const char* = 0L );
    ~KWPagePreview() {}

    void setLeft( float _left )
    { left = _left; repaint( false ); }
    void setRight( float _right )
    { right = _right; repaint( false ); }
    void setFirst( float _first )
    { first = _first; repaint( false ); }
    void setSpacing( float _spacing )
    { spacing = _spacing; repaint( false ); }
    void setBefore( float _before )
    { before = _before; repaint( false ); }
    void setAfter( float _after )
    { after = _after; repaint( false ); }

protected:
    void drawContents( QPainter* );

    float left, right, first, spacing, before, after;

};

/******************************************************************/
/* class KWPagePreview2                                           */
/******************************************************************/

class KWPagePreview2 : public QGroupBox
{
    Q_OBJECT

public:
    KWPagePreview2( QWidget*, const char* = 0L );
    ~KWPagePreview2() {}

    void setAlign( int _align )
    { align = _align; repaint( false ); }

protected:
    void drawContents( QPainter* );

    int align;

};

/******************************************************************/
/* class KWBorderPreview                                          */
/******************************************************************/

class KWBorderPreview : public QGroupBox
{
    Q_OBJECT

public:
    KWBorderPreview( QWidget*, const char* = 0L );
    ~KWBorderPreview() {}

    Border getLeftBorder() { return leftBorder; }
    void setLeftBorder( Border _leftBorder ) { leftBorder = _leftBorder; repaint( true ); }
    Border getRightBorder() { return rightBorder; }
    void setRightBorder( Border _rightBorder ) { rightBorder = _rightBorder; repaint( true ); }
    Border getTopBorder() { return topBorder; }
    void setTopBorder( Border _topBorder ) { topBorder = _topBorder; repaint( true ); }
    Border getBottomBorder() { return bottomBorder; }
    void setBottomBorder( Border _bottomBorder ) { bottomBorder = _bottomBorder; repaint( true ); }

protected:
    void drawContents( QPainter* );
    QPen setBorderPen( Border _brd );

    Border leftBorder, rightBorder, topBorder, bottomBorder;

};

/******************************************************************/
/* class KWNumPreview                                             */
/******************************************************************/

class KWNumPreview : public QGroupBox
{
    Q_OBJECT

public:
    KWNumPreview( QWidget*, const char* = 0L );
    ~KWNumPreview() {}

    void setCounter( Counter _counter ) { counter = _counter; repaint( true ); }

protected:
    void drawContents( QPainter* );

    Counter counter;

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

    void setLeftIndent( KWUnit _left );
    void setRightIndent( KWUnit _right );
    void setFirstLineIndent( KWUnit _first );
    void setSpaceAfterParag( KWUnit _after );
    void setSpaceBeforeParag( KWUnit _before );
    void setLineSpacing( KWUnit _spacing );

    void setAlign( int align );

    void setTabList( const QList<KoTabulator> *tabList );

    KWUnit leftIndent() const;
    KWUnit rightIndent() const;
    KWUnit firstLineIndent() const;
    KWUnit spaceBeforeParag() const;
    KWUnit spaceAfterParag() const;
    KWUnit lineSpacing() const;

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

    const QList<KoTabulator>* tabListTabulator() const {return &_tabList;}


    bool isAlignChanged() const {return oldLayout.alignment!=align();}
    bool listTabulatorChanged() const {return m_bListTabulatorChanged;}

    bool isLineSpacingChanged() const {return oldLayout.lineSpacing.pt()!=lineSpacing().pt();}
    bool isLeftMarginChanged() const { return oldLayout.margins[QStyleSheetItem::MarginLeft].pt()!=leftIndent().pt(); }
    bool isRightMarginChanged() const { return oldLayout.margins[QStyleSheetItem::MarginRight].pt()!=rightIndent().pt();}
    bool isFirstLineChanged() const {return oldLayout.margins[ QStyleSheetItem::MarginFirstLine].pt()!=firstLineIndent().pt();}
    bool isSpaceBeforeChanged() const { return oldLayout.margins[QStyleSheetItem::MarginTop].pt()!=spaceBeforeParag().pt();}
    bool isSpaceAfterChanged() const {return oldLayout.margins[QStyleSheetItem::MarginBottom].pt()!=spaceAfterParag() .pt();}
    bool isBulletChanged() const {return !(oldLayout.counter==counter());}

    bool isBorderChanged() const { return (oldLayout.leftBorder!=leftBorder() ||
					 oldLayout.rightBorder!=rightBorder() ||
					 oldLayout.topBorder!=topBorder() ||
					 oldLayout.bottomBorder!=bottomBorder() ); }
    //necessary when you used just border dialog
    void setAfterInitBorder(bool _b){m_bAfterInitBorder=_b;}

protected:
    void setupTab1();
    void setupTab2();
    void setupTab3();
    void setupTab4();
    void setupTab5();
    void clearAligns();
    void updateBorders();
    void setActifItem(double value);
    bool findExistingValue(double val);
//    void enableUIForCounterType();

    // Tab4 data.
    QButtonGroup *gNumbering;
    QButtonGroup *gStyle;
    QLineEdit *eCustomNum;
    QLineEdit *ecLeft, *ecRight, *eStart;
    QLabel *lStart;

    QGridLayout *indentGrid, *spacingGrid,
        *pSpaceGrid, *tabGrid;
    QLineEdit *eLeft, *eRight, *eFirstLine, *eSpacing, *eBefore, *eAfter, *eTabPos;
    QLabel *lLeft, *lRight, *lFirstLine, *lBefore, *lAfter, *lAlign, *lStyle, *lWidth, *lColor, *lDepth;
    QGroupBox *indentFrame, *spacingFrame, *pSpaceFrame, *gText;
    QComboBox *cSpacing, *cStyle, *cWidth;
    QRadioButton *rLeft, *rCenter, *rRight, *rJustify;
    KWPagePreview *prev1;
    KWPagePreview2 *prev2;
    QPushButton *bLeft, *bRight, *bTop, *bBottom, *bBullets, *bFont, *bAdd, *bDel, *bModify;
    KWBorderPreview *prev3;
    KColorButton *bColor;
    QSpinBox *sDepth;
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
    QList<KoTabulator> _tabList;
    KWUnits unit;
    KWParagLayout oldLayout;

    bool m_bAfterInitBorder;
    bool m_bListTabulatorChanged;

protected slots:
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
};

#endif
