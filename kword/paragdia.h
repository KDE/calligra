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
#include <kwunit.h>

#include "kwtextparag.h"
#include "counter.h"

class KButtonBox;
class KColorButton;
class KColorDialog;
class KWBorderPreview;
class KWDocument;
class KWNumPreview;
class KWPagePreview2;
class KWPagePreview;
class KWParagLayout;
class KWSpinBox;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListBox;
class QPushButton;
class QRadioButton;
class QWidget;

/**
 * This is the base class for any widget [usually used in a tab]
 * that handles modifying a part of a KWParagLayout.
 * Used by the paragraph dialog (this file) and by the style editor.
 */
class KWParagLayoutWidget : public QWidget
{
    Q_OBJECT
public:
    // Constructor: parent widget, flag (PD_SOMETHING) and name
    KWParagLayoutWidget( int flag, QWidget * parent, const char * name = 0 )
        : QWidget( parent, name ), m_flag( flag )
    {
    }
    virtual ~KWParagLayoutWidget() {}

    // Display settings, from the paragLayout
    virtual void display( const KWParagLayout & lay ) = 0;

    // Save the settings, into the paragLayout
    // This is only used by the stylist, not by paragdia (which needs undo/redo, applying partially etc.)
    virtual void save( KWParagLayout & lay ) = 0;

    // Return true if the settings where modified
    // ## maybe too global, but how to do it differently? We'll see if we need this.
    //virtual bool isModified() = 0;

    /**  return the (i18n-ed) name of the tab */
    virtual QString tabName() = 0;

    // Return the part of the paraglayout that this widget cares about
    int flag() const { return m_flag; }

private:
    int m_flag;
};

/**
 * The widget for editing idents and spacings (tab 1)
 */
class KWIndentSpacingWidget : public KWParagLayoutWidget
{
    Q_OBJECT
public:
    KWIndentSpacingWidget( KWUnit::Unit unit, QWidget * parent, const char * name = 0 );
    virtual ~KWIndentSpacingWidget() {}

    virtual void display( const KWParagLayout & lay );
    virtual void save( KWParagLayout & lay );
    //virtual bool isModified();
    virtual QString tabName();

    double leftIndent() const;
    double rightIndent() const;
    double firstLineIndent() const;
    double spaceBeforeParag() const;
    double spaceAfterParag() const;
    double lineSpacing() const;
    bool linesTogether() const;

private slots:
    void leftChanged( const QString & );
    void rightChanged( const QString & );
    void firstChanged( const QString & );
    void spacingActivated( int );
    void spacingChanged( const QString & );
    void beforeChanged( const QString & );
    void afterChanged( const QString & );
private:
    QLineEdit *eLeft, *eRight, *eFirstLine, *eBefore, *eAfter, *eSpacing;
    QComboBox *cSpacing;
    QCheckBox *cEndOfFramePage;
    KWPagePreview *prev1;
    KWUnit::Unit m_unit;
};

/**
 * The widget for editing paragraph alignment (tab 2)
 */
class KWParagAlignWidget : public KWParagLayoutWidget
{
    Q_OBJECT
public:
    KWParagAlignWidget( QWidget * parent, const char * name = 0 );
    virtual ~KWParagAlignWidget() {}

    virtual void display( const KWParagLayout & lay );
    virtual void save( KWParagLayout & lay );
    //virtual bool isModified();
    virtual QString tabName();

    int align() const;

protected slots:
    void alignLeft();
    void alignCenter();
    void alignRight();
    void alignJustify();

protected:
    void clearAligns();

private:
    QRadioButton *rLeft, *rCenter, *rRight, *rJustify;
    KWPagePreview2 *prev2;
};

/**
 * The widget for editing paragraph borders (tab 3)
 */
class KWParagBorderWidget : public KWParagLayoutWidget
{
    Q_OBJECT
public:
    KWParagBorderWidget( QWidget * parent, const char * name = 0 );
    virtual ~KWParagBorderWidget() {}

    virtual void display( const KWParagLayout & lay );
    virtual void save( KWParagLayout & lay );
    //virtual bool isModified();
    virtual QString tabName();

    Border leftBorder() const { return m_leftBorder; }
    Border rightBorder() const { return m_rightBorder; }
    Border topBorder() const { return m_topBorder; }
    Border bottomBorder() const { return m_bottomBorder; }

protected:
    void updateBorders();

protected slots:
    void brdLeftToggled( bool );
    void brdRightToggled( bool );
    void brdTopToggled( bool );
    void brdBottomToggled( bool );
    //void brdStyleChanged( const QString & );
    //void brdWidthChanged( const QString & );
    //void brdColorChanged( const QColor& );
    void slotPressEvent(QMouseEvent *_ev);

private:
    QComboBox *cWidth, *cStyle;
    QPushButton *bLeft, *bRight, *bTop, *bBottom;
    KColorButton *bColor;
    Border m_leftBorder, m_rightBorder, m_topBorder, m_bottomBorder;
    KWBorderPreview *prev3;
};

/**
 * The complete(*) dialog for changing attributes of a paragraph
 *
 * (*) the flags (to only show parts of it) have been kept just in case
 * but are not currently used.
 */
class KWParagDia : public KDialogBase
{
    Q_OBJECT

public:
    static const int PD_SPACING = 1;
    static const int PD_ALIGN = 2;
    static const int PD_BORDERS = 4;
    static const int PD_NUMBERING = 8;
    static const int PD_TABS = 16;

    KWParagDia( QWidget*, const char*, int _flags, KWDocument *_doc );
    ~KWParagDia();

    int getFlags() { return flags; }

    // Get values (in pt)
    double leftIndent() const { return m_indentSpacingWidget->leftIndent(); }
    double rightIndent() const { return m_indentSpacingWidget->rightIndent(); }
    double firstLineIndent() const { return m_indentSpacingWidget->firstLineIndent(); }
    double spaceBeforeParag() const { return m_indentSpacingWidget->spaceBeforeParag(); }
    double spaceAfterParag() const { return m_indentSpacingWidget->spaceAfterParag(); }
    double lineSpacing() const { return m_indentSpacingWidget->lineSpacing(); }
    bool linesTogether() const { return m_indentSpacingWidget->linesTogether(); }

    int align() const { return m_alignWidget->align(); }

    Border leftBorder() const { return m_borderWidget->leftBorder(); }
    Border rightBorder() const { return m_borderWidget->rightBorder(); }
    Border topBorder() const { return m_borderWidget->topBorder(); }
    Border bottomBorder() const { return m_borderWidget->bottomBorder(); }

    void setCounter( Counter _counter );
    Counter counter() const { return m_counter; }

    void setParagLayout( const KWParagLayout & lay );

    KoTabulatorList tabListTabulator() const { return _tabList; }
    void setTabList( const KoTabulatorList & tabList );

    bool isAlignChanged() const {return oldLayout.alignment!=align();}
    bool listTabulatorChanged() const {return oldLayout.tabList()!=tabListTabulator();}

    bool isLineSpacingChanged() const {return oldLayout.lineSpacing!=lineSpacing();}
    bool isLeftMarginChanged() const { return oldLayout.margins[QStyleSheetItem::MarginLeft]!=leftIndent(); }
    bool isRightMarginChanged() const { return oldLayout.margins[QStyleSheetItem::MarginRight]!=rightIndent();}
    bool isFirstLineChanged() const {return oldLayout.margins[ QStyleSheetItem::MarginFirstLine]!=firstLineIndent();}
    bool isSpaceBeforeChanged() const { return oldLayout.margins[QStyleSheetItem::MarginTop]!=spaceBeforeParag();}
    bool isSpaceAfterChanged() const {return oldLayout.margins[QStyleSheetItem::MarginBottom]!=spaceAfterParag();}
    bool isPageBreakingChanged() const { return oldLayout.linesTogether!=linesTogether(); }
    bool isBulletChanged() const;

    bool isBorderChanged() const { return (oldLayout.leftBorder!=leftBorder() ||
					 oldLayout.rightBorder!=rightBorder() ||
					 oldLayout.topBorder!=topBorder() ||
					 oldLayout.bottomBorder!=bottomBorder() ); }
    void changeKWSpinboxType();
protected:
    void setupTab4();
    void setupTab5();
    void setActiveItem(double value);
    bool findExistingValue(double val);
//    void enableUIForCounterType();

private:
    KWIndentSpacingWidget * m_indentSpacingWidget;
    KWParagAlignWidget * m_alignWidget;
    KWParagBorderWidget * m_borderWidget;

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

    QLineEdit *eTabPos;
    QGroupBox *gText;

    QPushButton *bFont, *bAdd, *bDel, *bModify;
    KWNumPreview *prev4;
    QListBox *lTabs;
    QLabel *lTab;
    QRadioButton *rtLeft, *rtCenter, *rtRight, *rtDecimal;

    int flags;
    Counter m_counter;
    KWDocument *doc;
    KoTabulatorList _tabList;
    KWParagLayout oldLayout;

protected slots:
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
