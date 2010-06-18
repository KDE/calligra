/* This file is part of the KDE project
   Copyright (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 1999-2002 Harri Porten <porten@kde.org>
             (C) 2000-2001 David Faure <faure@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>
             (C) 2000 Werner Trobin <trobin@kde.org>
             (C) 1999 Reginald Stadlbauer <reggie@kde.org>
             (C) 1998-1999 Stephan Kulow <coolo@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_LAYOUT_DIALOG
#define KSPREAD_LAYOUT_DIALOG

#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QPaintEvent>
#include <QPushButton>

#include <kcompletion.h>
#include <knuminput.h>
#include <kpagedialog.h>

#include "RowColumnFormat.h"
#include "Style.h"
#include "Value.h"

#include "ui_FontWidget.h"
#include "ui_PositionWidget.h"
#include "ui_ProtectionWidget.h"

class KoUnitDoubleSpinBox;
class QPixmap;
class QRadioButton;
class QPushButton;
class QLabel;
class QWidget;
class KLineEdit;
class QFrame;
class QListWidget;
class QCheckBox;
class KColorButton;
class KComboBox;
class KComboBox;
class KoUnitDoubleSpinBox;

namespace KSpread
{
class Sheet;
class CustomStyle;
class StyleManager;
class CellFormatDialog;
class Selection;
class StyleCommand;

enum BorderType {
    BorderType_Top = 0,
    BorderType_Bottom,
    BorderType_Left,
    BorderType_Right,
    BorderType_Vertical,
    BorderType_Horizontal,
    BorderType_FallingDiagonal,
    BorderType_RisingDiagonal,
    BorderType_END
};

enum BorderShortcutType {
    BorderShortcutType_Remove = 0,
    BorderShortcutType_All,
    BorderShortcutType_Outline,
    BorderShortcutType_END
};

/**
 * \ingroup UI
 * Widget to select a background pattern.
 */
class PatternSelect : public QFrame
{
    Q_OBJECT
public:
    PatternSelect(QWidget *parent, const char *_name);

    void setPenStyle(Qt::PenStyle _pat) {
        penStyle = _pat; repaint();
    }
    Qt::PenStyle getPenStyle() {
        return penStyle;
    }
    void setColor(const QColor &_col) {
        penColor = _col; repaint();
    }
    const QColor& getColor() {
        return penColor;
    }
    void setPenWidth(int _w) {
        penWidth = _w; repaint();
    }
    int getPenWidth() {
        return penWidth;
    }

    void setPattern(const QColor &_color, int _width, Qt::PenStyle _style);
    void setUndefined();
    void setDefined() {
        undefined = false; repaint();
    }

    bool isDefined() {
        return !undefined;
    }

signals:
    void clicked(PatternSelect *_this);

public slots:
    void slotUnselect();
    void slotSelect();

protected:
    virtual void paintEvent(QPaintEvent *_ev);
    virtual void mousePressEvent(QMouseEvent *_ev);

    Qt::PenStyle penStyle;
    QColor penColor;
    int penWidth;

    bool selected;
    bool undefined;
};

/**
 * \ingroup UI
 * Dialog page to select the cell style inheritance.
 * Only shown when modifying a named cell style.
 */
class GeneralTab : public QWidget
{
    Q_OBJECT

public:
    GeneralTab(QWidget * parent, CellFormatDialog * _dlg);
    ~GeneralTab();

    bool apply(CustomStyle * style);

protected slots:
    void parentChanged(const QString&);
    void styleNameChanged(const QString&);

private:
    CellFormatDialog * m_dlg;
    KComboBox     * m_parentBox;
    KLineEdit     * m_nameEdit;
    QLabel        * m_nameStatus;
    QLabel        * m_parentStatus;

    QString m_name;
    QString m_parent;

    bool checkParent(const QString & parentName);
    bool checkName();
};

/**
 * \ingroup UI
 * Dialog page to choose the used font.
 */
class CellFormatPageFont : public QWidget, public Ui::FontWidget
{
    Q_OBJECT
public:
    CellFormatPageFont(QWidget* parent, CellFormatDialog *_dlg);

    void apply(CustomStyle * style);
    void apply(StyleCommand *_obj);

signals:
    /**
     * Connect to this to monitor the font as it as selected if you are
     * not running modal.
     */
    void fontSelected(const QFont &font);

private slots:

    void      family_chosen_slot(const QString &);
    void      size_chosen_slot(const QString &);
    void      weight_chosen_slot(const QString &);
    void      style_chosen_slot(const QString &);
    void      underline_chosen_slot();
    void      strike_chosen_slot();
    void      display_example(const QFont &font);
    void      slotSetTextColor(const QColor &_color);

private:

    void setCombos();
    QColor textColor;
    QFont         selFont;
    bool fontChanged;
    bool bTextColorUndefined;
    CellFormatDialog *dlg;
};

/**
 * \ingroup UI
 * Dialog page to select the value formatting.
 */
class CellFormatPageFloat : public QWidget
{
    Q_OBJECT
public:
    CellFormatPageFloat(QWidget *parent, CellFormatDialog *_dlg);
    void apply(CustomStyle * style);
    void apply(StyleCommand *_obj);

public slots:
    void slotChangeState();
    void makeformat();
    void updateFormatType();
    void init();
    void datetimeInit();
    void slotChangeValue(int);
    void formatChanged(int);
    void currencyChanged(const QString &);
protected:
    KLineEdit* postfix;
    KIntNumInput* precision;
    KLineEdit* prefix;
    KComboBox *format;
    KComboBox *currency;
    QLabel    *currencyLabel;
    QRadioButton *generic;
    QRadioButton *number;
    QRadioButton *percent;
    QRadioButton *date;
    QRadioButton *datetime;
    QRadioButton *money;
    QRadioButton *scientific;
    QRadioButton *fraction;
    QRadioButton *time;
    QRadioButton *textFormat;
    QRadioButton *customFormat;
    QListWidget  *listFormat;
    KLineEdit* customFormatEdit;
    QLabel *exampleLabel;
    CellFormatDialog *dlg;
    Format::Type cellFormatType, newFormatType;
    //test if value changed
    bool m_bFormatTypeChanged;
    bool m_bFormatColorChanged;
};

/**
 * \ingroup UI
 * Dialog page to set up the cell dimension, value alignment, etc.
 */
class CellFormatPagePosition : public QWidget, public Ui::PositionWidget
{
    Q_OBJECT
public:
    CellFormatPagePosition(QWidget *parent, CellFormatDialog *_dlg);

    void apply(CustomStyle * style);
    void apply(StyleCommand *_obj);

    double getSizeHeight() const;
    double getSizeWidth() const;
    bool getMergedCellState() const;

public slots:
    void slotChangeHeightState();
    void slotChangeWidthState();
    void slotChangeAngle(int);
    void slotStateChanged(int);
    void slotChangeVerticalState();
    void slotChangeMultiState();
    void slotChangeShrinkToFitState();

protected:
    KoUnitDoubleSpinBox *width;
    KoUnitDoubleSpinBox *height;
    KoUnitDoubleSpinBox *m_indent;
    CellFormatDialog *dlg;
    bool m_bOptionText;
};



/**
 * \ingroup UI
 * The cell border preview.
 */
class Border : public QFrame
{
    Q_OBJECT
public:
    Border(QWidget *parent, const char *_name, bool _oneCol, bool _oneRow);
signals:
    void redraw();
    void choosearea(QMouseEvent * _ev);
protected:
    virtual void paintEvent(QPaintEvent *_ev);
    virtual void mousePressEvent(QMouseEvent* _ev);
    bool oneCol;
    bool oneRow;
};

/**
 * \ingroup UI
 * A button to set/unset a cell border.
 */
class BorderButton : public QPushButton
{
    Q_OBJECT
public:
    BorderButton(QWidget *parent, const char *_name);
    void setPenStyle(Qt::PenStyle _pat) {
        penStyle = _pat;
    }
    Qt::PenStyle getPenStyle() {
        return penStyle;
    }
    void setColor(const QColor &_col) {
        penColor = _col;
    }
    const QColor& getColor() {
        return penColor;
    }
    void setPenWidth(int _w) {
        penWidth = _w;
    }
    int getPenWidth() {
        return penWidth;
    }
    bool isChanged() {
        return changed;
    }
    void setChanged(bool _changed) {
        changed = _changed;
    }
    void setUndefined();
    void unselect();
signals:
    void clicked(BorderButton *);
protected:
    virtual void mousePressEvent(QMouseEvent *_ev);
    Qt::PenStyle penStyle;
    QColor penColor;
    int penWidth;
    bool changed;

};

/**
 * \ingroup UI
 * Dialog page to select the cell borders.
 */
class CellFormatPageBorder : public QWidget
{
    Q_OBJECT
public:
    CellFormatPageBorder(QWidget *parent, CellFormatDialog *_dlg);

    void apply(StyleCommand* obj);
    void invertState(BorderButton *_button);
    QPixmap paintFormatPixmap(Qt::PenStyle _style);

public slots:
    void changeState(BorderButton *_this);
    void preselect(BorderButton *_this);
    void draw();
    void slotSetColorButton(const QColor &_color);
    void slotUnselect2(PatternSelect *_select);
    void loadIcon(const QString& pix, BorderButton *_button);
    void slotPressEvent(QMouseEvent *_ev);
    void slotChangeStyle(int);
    void slotChangeStyle(const QString &);
    void cutomize_chosen_slot();

protected:

    Sheet* sheet;
    BorderButton* borderButtons[BorderType_END];
    BorderButton* shortcutButtons[BorderShortcutType_END];
#define NUM_BORDER_PATTERNS 10

    /* the patterns to choose from */
    PatternSelect* pattern[NUM_BORDER_PATTERNS];

    /* the pattern box that is the 'preview' of what is selected above. */
    PatternSelect* preview;
    KComboBox* size;
    KComboBox* style;
    KColorButton* color;
    QCheckBox* customize;
    QColor currentColor;
    Border *area;
    CellFormatDialog *dlg;
private:

    /*some helper functions to space some tasks apart */
    void InitializeGrids();
    void InitializeBorderButtons();
    void InitializePatterns();
    void SetConnections();
    void applyTopOutline(StyleCommand* obj);
    void applyBottomOutline(StyleCommand* obj);
    void applyLeftOutline(StyleCommand* obj);
    void applyRightOutline(StyleCommand* obj);
    void applyVerticalOutline(StyleCommand* obj);
    void applyHorizontalOutline(StyleCommand* obj);
    void applyDiagonalOutline(StyleCommand* obj);
};

/**
 * \ingroup UI
 * Widget to preview a background brush.
 */
class BrushSelect : public QFrame
{
    Q_OBJECT
public:
    BrushSelect(QWidget *parent, const char *_name);

    void setBrushStyle(Qt::BrushStyle _pat) {
        brushStyle = _pat; repaint();
    }
    Qt::BrushStyle getBrushStyle() const {
        return brushStyle;
    }
    QColor getBrushColor() const {
        return brushColor;
    }
    void setBrushColor(const QColor &_c) {
        brushColor = _c;
    }
    void setPattern(const QColor &_color, Qt::BrushStyle _style);

signals:
    void clicked(BrushSelect *_this);

public slots:
    void slotUnselect();
    void slotSelect();

protected:
    virtual void paintEvent(QPaintEvent *_ev);
    virtual void mousePressEvent(QMouseEvent *_ev);

    Qt::BrushStyle brushStyle;
    QColor brushColor;
    bool selected;
};


/**
 * \ingroup UI
 * Dialog page to set the cell background.
 */
class CellFormatPagePattern : public QWidget
{
    Q_OBJECT
public:
    CellFormatPagePattern(QWidget *parent, CellFormatDialog *_dlg);

    void apply(CustomStyle * style);
    void apply(StyleCommand *_obj);

    void init();
public slots:
    void slotUnselect2(BrushSelect *_select);
    void slotSetColorButton(const QColor &_color);
    void slotSetBackgroundColor(const QColor &_color);
    void slotNotAnyColor();
protected:
    BrushSelect *selectedBrush;
    BrushSelect *brush1;
    BrushSelect *brush2;
    BrushSelect *brush3;
    BrushSelect *brush4;
    BrushSelect *brush5;
    BrushSelect *brush6;
    BrushSelect *brush7;
    BrushSelect *brush8;
    BrushSelect *brush9;
    BrushSelect *brush10;
    BrushSelect *brush11;
    BrushSelect *brush12;
    BrushSelect *brush13;
    BrushSelect *brush14;
    BrushSelect *brush15;
    BrushSelect *current;
    KColorButton* color;
    QPushButton* notAnyColor;
    QColor currentColor;

    QColor bgColor;
    KColorButton *bgColorButton;
    bool b_notAnyColor;
    CellFormatDialog *dlg;
};


/**
 * \ingroup UI
 * Dialog page to set up cell protection.
 */
class CellFormatPageProtection : public QWidget, public Ui::ProtectionWidget
{
    Q_OBJECT

public:
    CellFormatPageProtection(QWidget * parent, CellFormatDialog * _dlg);
    ~CellFormatPageProtection();
    ///when protection is set through Style Manager
    void apply(CustomStyle * style);
    void apply(StyleCommand * _obj);

protected:
    CellFormatDialog * m_dlg;
    bool            m_isProtected;
    bool            m_hideFormula;
    bool            m_hideAll;
    bool            m_dontPrint;
};

/**
 * \ingroup UI
 * Dialog to set the cell style.
 */
class CellFormatDialog : public KPageDialog
{
    Q_OBJECT
public:
    /**
     * Create a format dlg for the rectangular area in '_sheet'.
     */
    CellFormatDialog(QWidget* parent, Selection* selection);
    CellFormatDialog(QWidget* parent, Selection* selection, CustomStyle* style, StyleManager* manager);

    ~CellFormatDialog();

    void init();
    void initGUI();
    void initMembers();

    void initParameters(const Style& _obj);
    void checkBorderRight(const Style& obj);
    void checkBorderLeft(const Style& obj);
    void checkBorderTop(const Style& obj);
    void checkBorderBottom(const Style& obj);
    void checkBorderVertical(const Style& obj);
    void checkBorderHorizontal(const Style& obj);

    Sheet * getSheet() const {
        return m_sheet;
    }
    Selection* selection() const {
        return m_selection;
    }
    CustomStyle * getStyle() const {
        return m_style;
    }
    StyleManager * getStyleManager() const {
        return m_styleManager;
    }

    bool isSingleCell() {
        return (left == right && top == bottom);
    }
    bool checkCircle(QString const & name, QString const & parent);

    KLocale * locale() const;


    struct CellBorderFormat {
        int width;
        bool bStyle;
        QColor color;
        bool bColor;
        Qt::PenStyle style;
    };

    // The format of the selected area
    CellBorderFormat borders[BorderType_END];

    Qt::BrushStyle brushStyle;
    QColor brushColor;

    bool oneCol;
    bool oneRow;

    QString prefix;
    QString postfix;
    int precision;
    Style::FloatFormat floatFormat;
    bool bFloatFormat;
    Style::FloatColor floatColor;
    Currency m_currency;
    bool bFloatColor;
    bool bCurrency;
    QColor textColor;
    bool bTextColor;
    bool bTextFontBold;
    bool fontBold;
    bool bTextFontItalic;
    bool fontItalic;
    bool bTextFontSize;
    int fontSize;
    bool bTextFontFamily;
    QString fontFamily;
    bool bStrike;
    bool strike;
    bool bUnderline;
    bool underline;
    QFont font;
    QColor bgColor;
    bool bBgColor;
    QString actionText;
    Style::HAlign alignX;
    Style::VAlign alignY;
    QString styleName;
    QString styleParent;

    bool bMultiRow;
    bool bVerticalText;
    bool bShrinkToFit;

    bool bDontPrintText;
    bool bHideFormula;
    bool bHideAll;
    bool bIsProtected;

    double defaultHeightSize;
    double defaultWidthSize;
    double heightSize;
    double widthSize;

    double indent;

    QPixmap* formatOnlyNegSignedPixmap;
    QPixmap* formatRedOnlyNegSignedPixmap;
    QPixmap* formatRedNeverSignedPixmap;
    QPixmap* formatAlwaysSignedPixmap;
    QPixmap* formatRedAlwaysSignedPixmap;

    int textRotation;
    bool bTextRotation;

    Format::Type formatType;
    bool bFormatType;

    Value value;

    bool isMerged;
    bool oneCell;

    bool isRowSelected;
    bool isColumnSelected;

    // The rectangular area for which this dlg has been opened.
    int left;
    int right;
    int top;
    int bottom;


public slots:
    void slotApply();

protected:

    /**
     * Draws a pixmap showing a text consisting of two parts, @p _string1 and @p _string2 .
     * The parts' colors are given by @p _color1 and @p _color2 .
     */
    QPixmap* paintFormatPixmap(const char *_string1, const QColor & _color1,
                               const char *_string2, const QColor & _color2);

    GeneralTab * generalPage;
    CellFormatPageFloat *floatPage;
    CellFormatPageBorder *borderPage;
    CellFormatPageFont *fontPage;
    CellFormatPagePosition *positionPage;
    CellFormatPagePattern *patternPage;
    CellFormatPageProtection *protectPage;

    Sheet * m_sheet;
    Selection  * m_selection;
    CustomStyle * m_style;
    StyleManager * m_styleManager;

    void applyStyle();
};

} // namespace KSpread

#endif // KSPREAD_LAYOUT_DIALOG
