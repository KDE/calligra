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
 * Boston, MA 02110-1301, USA.
*/

#ifndef __kspread_dlg_layout_h__
#define __kspread_dlg_layout_h__


#include <q3tabdialog.h>
#include <qapplication.h>
#include <q3buttongroup.h>
#include <q3groupbox.h>
//Added by qt3to4:
#include <QPixmap>
#include <QPaintEvent>
#include <Q3Frame>
#include <QLabel>
#include <QMouseEvent>
#include <knuminput.h>
#include <kcompletion.h>
#include "kspread_view.h"
#include "kspread_style.h"
#include "kspread_format.h"
#include "kspread_doc.h"
#include "kspread_value.h"
#include "position_cell_format.h"
#include "font_cell_format.h"
#include "protection_cell_format.h"

#include <qpushbutton.h>
#include <qcheckbox.h>

class KoUnitDoubleSpinBox;
class QPixmap;
class QRadioButton;
class QPushButton;
class QDate;
class QTime;
class QLabel;
class QWidget;
class KLineEdit;
class Q3Frame;
class Q3ListBox;
class QCheckBox;
class KColorButton;
class QComboBox;
class KComboBox;
class KoUnitDoubleSpinBox;

namespace KSpread
{
class Sheet;
class CustomStyle;
class StyleManager;
class CellFormatDialog;
class FormatManipulator;

enum BorderType
{
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

enum BorderShortcutType
{
  BorderShortcutType_Remove = 0,
  BorderShortcutType_All,
  BorderShortcutType_Outline,
  BorderShortcutType_END
};

class PatternSelect : public Q3Frame
{
    Q_OBJECT
public:
    PatternSelect( QWidget *parent, const char *_name );

    void setPenStyle( Qt::PenStyle _pat ) { penStyle = _pat; repaint(); }
    Qt::PenStyle getPenStyle() { return penStyle; }
    void setColor( const QColor &_col ) { penColor = _col; repaint(); }
    const QColor& getColor() { return penColor; }
    void setPenWidth( int _w ) { penWidth = _w; repaint(); }
    int getPenWidth() { return penWidth; }

    void setPattern( const QColor &_color, int _width, Qt::PenStyle _style );
    void setUndefined();
    void setDefined() { undefined = FALSE; repaint(); }

    bool isDefined() { return !undefined; }

signals:
    void clicked( PatternSelect *_this );

public slots:
    void slotUnselect();
    void slotSelect();

protected:
    virtual void paintEvent( QPaintEvent *_ev );
    virtual void mousePressEvent( QMouseEvent *_ev );

    PenStyle penStyle;
    QColor penColor;
    int penWidth;

    bool selected;
    bool undefined;
};

class GeneralTab : public QWidget
{
  Q_OBJECT

 public:
  GeneralTab( QWidget * parent, CellFormatDialog * _dlg );
  ~GeneralTab();

  bool apply( CustomStyle * style );

 protected slots:
  void slotNewParent( const QString & parentName );
  void slotNameChanged();

 private:
  CellFormatDialog * m_dlg;
  KComboBox     * m_parentBox;
  KLineEdit     * m_nameEdit;

  QString m_name;
  QString m_parent;

  bool checkParent( const QString & parentName );
  bool checkName();
};

/**
 */
class CellFormatPageFont : public FontTab
{
    Q_OBJECT
public:
    CellFormatPageFont( QWidget* parent, CellFormatDialog *_dlg );

    void apply( CustomStyle * style );
    void apply( FormatManipulator *_obj );

signals:
    /**
     * Connect to this to monitor the font as it as selected if you are
     * not running modal.
     */
    void fontSelected( const QFont &font );

private slots:

    void      family_chosen_slot(const QString & );
    void      size_chosen_slot(const QString & );
    void      weight_chosen_slot(const QString &);
    void      style_chosen_slot(const QString &);
    void      underline_chosen_slot();
    void      strike_chosen_slot();
    void      display_example(const QFont &font);
    void      slotSetTextColor( const QColor &_color );

private:

    void setCombos();
    QColor textColor;
    QFont         selFont;
    bool fontChanged;
    bool bTextColorUndefined;
    CellFormatDialog *dlg;
};

/**
 * Format of numbers.
 * This widget is part of the format dialog.
 * It allows the user to cinfigure the way numbers are formatted.
 */
class CellFormatPageFloat : public QWidget
{
    Q_OBJECT
public:
    CellFormatPageFloat( QWidget *parent, CellFormatDialog *_dlg );
    void apply( CustomStyle * style );
    void apply( FormatManipulator *_obj );

public slots:
    void slotChangeState();
    void makeformat();
    void updateFormatType();
    void init();
    void slotChangeValue(int);
    void formatChanged(int);
    void currencyChanged(const QString &);
protected:
    QLineEdit* postfix;
    KIntNumInput* precision;
    QLineEdit* prefix;
    QComboBox *format;
    QComboBox *currency;
    QLabel    *currencyLabel;
    QRadioButton *generic;
    QRadioButton *number;
    QRadioButton *percent;
    QRadioButton *date;
    QRadioButton *money;
    QRadioButton *scientific;
    QRadioButton *fraction;
    QRadioButton *time;
    QRadioButton *textFormat;
    QRadioButton *customFormat;
    Q3ListBox *listFormat;
    QLineEdit* customFormatEdit;
    QLabel *exampleLabel;
    CellFormatDialog *dlg;
    FormatType cellFormatType, newFormatType;
    //test if value changed
    bool m_bFormatTypeChanged;
    bool m_bFormatColorChanged;
};

class CellFormatPagePosition : public PositionTab
{
    Q_OBJECT
public:
    CellFormatPagePosition( QWidget *parent, CellFormatDialog *_dlg );

    void apply( CustomStyle * style );
    void apply( FormatManipulator *_obj );

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

protected:
    KoUnitDoubleSpinBox *width;
    KoUnitDoubleSpinBox *height;
    KoUnitDoubleSpinBox *m_indent;
    CellFormatDialog *dlg;
    bool m_bOptionText;
};



class Border : public Q3Frame
{
    Q_OBJECT
public:
    Border( QWidget *parent,const char *_name,bool _oneCol,bool _oneRow  );
signals:
    void redraw();
    void choosearea(QMouseEvent * _ev);
protected:
    virtual void paintEvent( QPaintEvent *_ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    bool oneCol;
    bool oneRow;
};

class BorderButton : public QPushButton
{
    Q_OBJECT
public:
    BorderButton( QWidget *parent, const char *_name );
    void setPenStyle( Qt::PenStyle _pat ) { penStyle = _pat;}
    Qt::PenStyle getPenStyle() { return penStyle; }
    void setColor( const QColor &_col ) { penColor = _col; }
    const QColor& getColor() { return penColor; }
    void setPenWidth( int _w ) { penWidth = _w; }
    int getPenWidth() { return penWidth; }
    bool isChanged() { return changed; }
    void setChanged(bool _changed ) { changed=_changed;}
    void setUndefined();
    void unselect();
 signals:
    void clicked(BorderButton *);
 protected:
    virtual void mousePressEvent( QMouseEvent *_ev );
    PenStyle penStyle;
    QColor penColor;
    int penWidth;
    bool changed;

};

class CellFormatPageBorder : public QWidget
{
  Q_OBJECT
public:
    CellFormatPageBorder( QWidget *parent, CellFormatDialog *_dlg );

    void apply(FormatManipulator* obj);
    void invertState(BorderButton *_button);
    QPixmap paintFormatPixmap(Qt::PenStyle _style);

public slots:
    void changeState(BorderButton *_this);
    void preselect( BorderButton *_this);
    void draw();
    void slotSetColorButton( const QColor &_color );
    void slotUnselect2( PatternSelect *_select );
    void loadIcon( QString pix,BorderButton *_button);
    void slotPressEvent(QMouseEvent *_ev);
    void slotChangeStyle(int );
    void slotChangeStyle(const QString & );
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
  QComboBox* size;
  QComboBox* style;
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
  void applyTopOutline(FormatManipulator* obj);
  void applyBottomOutline(FormatManipulator* obj);
  void applyLeftOutline(FormatManipulator* obj);
  void applyRightOutline(FormatManipulator* obj);
  void applyVerticalOutline(FormatManipulator* obj);
  void applyHorizontalOutline(FormatManipulator* obj);
  void applyDiagonalOutline(FormatManipulator* obj);
};

class BrushSelect : public Q3Frame
{
    Q_OBJECT
public:
    BrushSelect( QWidget *parent, const char *_name );

    void setBrushStyle( Qt::BrushStyle _pat ) { brushStyle = _pat; repaint(); }
    Qt::BrushStyle getBrushStyle() const { return brushStyle; }
    QColor getBrushColor() const { return brushColor; }
    void setBrushColor(const QColor &_c) { brushColor=_c;}
    void setPattern( const QColor &_color, Qt::BrushStyle _style );

signals:
    void clicked( BrushSelect *_this );

public slots:
    void slotUnselect();
    void slotSelect();

protected:
    virtual void paintEvent( QPaintEvent *_ev );
    virtual void mousePressEvent( QMouseEvent *_ev );

	Qt::BrushStyle brushStyle;
    QColor brushColor;
    bool selected;
};


class CellFormatPagePattern : public QWidget
{
    Q_OBJECT
public:
    CellFormatPagePattern( QWidget *parent, CellFormatDialog *_dlg );

    void apply( CustomStyle * style );
    void apply( FormatManipulator *_obj );

    void init();
public slots:
    void slotUnselect2( BrushSelect *_select );
    void slotSetColorButton( const QColor &_color );
    void slotSetBackgroundColor( const QColor &_color );
    void slotNotAnyColor( );
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
    bool bBgColorUndefined;
    KColorButton *bgColorButton;
    bool b_notAnyColor;
    CellFormatDialog *dlg;
};

class CellFormatPageProtection : public ProtectionTab
{
  Q_OBJECT

 public:
  CellFormatPageProtection( QWidget * parent, CellFormatDialog * _dlg );
  ~CellFormatPageProtection();
  ///when protection is set through Style Manager
  void apply( CustomStyle * style );
  void apply( FormatManipulator * _obj );

 protected:
  CellFormatDialog * m_dlg;
  bool            m_isProtected;
  bool            m_hideFormula;
  bool            m_hideAll;
  bool            m_dontPrint;
};

/**
 * Dialog for the "Cell Format..." action
 */
class CellFormatDialog : public QObject
{
    Q_OBJECT
public:
    /**
     * Create a format dlg for the rectangular area in '_sheet'.
     */
    CellFormatDialog( View * _view, Sheet * _sheet );
    CellFormatDialog( View * _view, CustomStyle * _style, StyleManager * _manager,
                   Doc * doc );

    ~CellFormatDialog();

    void init();
    void initGUI();
    void initMembers();

    void initParameters(Format *_obj,int column,int row);
    void checkBorderRight(Format *obj,int x,int y);
    void checkBorderLeft(Format *obj,int x,int y);
    void checkBorderTop(Format *obj,int x,int y);
    void checkBorderBottom(Format *obj,int x,int y);
    void checkBorderVertical(Format *obj,int x,int y);
    void checkBorderHorizontal(Format *obj,int x,int y);
    /**
     * Run the dialogs event loop and return when closed.
     */
    int exec();

    Doc * getDoc() const { return m_doc; }
    Sheet * getSheet() const { return m_sheet; }
    CustomStyle * getStyle() const { return m_style; }
    StyleManager * getStyleManager() const { return m_styleManager; }

    bool isSingleCell() { return ( left == right && top == bottom ); }
    bool checkCircle( QString const & name, QString const & parent );

    KLocale * locale() const { return m_pView->doc()->locale(); }


    struct CellBorderFormat
    {
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
    Style::Currency   cCurrency;
    bool bFloatColor;
    bool bCurrency;
    QColor textColor;
    bool bTextColor;
    bool bTextFontBold;
    bool textFontBold;
    bool bTextFontItalic;
    bool textFontItalic;
    bool bTextFontSize;
    int textFontSize;
    bool bTextFontFamily;
    QString textFontFamily;
    bool bStrike;
    bool strike;
    bool bUnderline;
    bool underline;
    QFont textFont;
    QColor bgColor;
    bool bBgColor;
    QString actionText;
    Style::HAlign alignX;
    Style::VAlign alignY;
    QString styleName;
    QString styleParent;

    bool bMultiRow;
    bool bVerticalText;

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

    FormatType formatType;
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
    QPixmap* paintFormatPixmap( const char *_string1, const QColor & _color1,
				const char *_string2, const QColor & _color2 );

    GeneralTab * generalPage;
    CellFormatPageFloat *floatPage;
    CellFormatPageBorder *borderPage;
    CellFormatPageFont *fontPage;
    CellFormatPagePosition *positionPage;
    CellFormatPagePattern *patternPage;
    CellFormatPageProtection *protectPage;
    Q3TabDialog *tab;

    Doc   * m_doc;
    Sheet * m_sheet;
    View  * m_pView;
    CustomStyle * m_style;
    StyleManager * m_styleManager;

    void applyStyle();
};

} // namespace KSpread

#endif
