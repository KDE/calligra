/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#ifndef __kspread_dlg_layout_h__
#define __kspread_dlg_layout_h__

#include <qwidget.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qtabdialog.h>
#include <qcolor.h>
#include <qfont.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlcdnumber.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qscrollbar.h>
#include <qtooltip.h>
#include <qstring.h>
#include <qdatetime.h>
#include <kcolordlg.h>
#include <kcolorbtn.h>
#include <knuminput.h>
#include <kcompletion.h>
#include <klineedit.h>

class CellLayoutDlg;
class KSpreadTable;
class KSpreadView;

#include "kspread_cell.h"

class KSpreadPatternSelect : public QFrame
{
    Q_OBJECT
public:
    KSpreadPatternSelect( QWidget *parent, const char *_name );

    void setPenStyle( PenStyle _pat ) { penStyle = _pat; repaint(); }
    PenStyle getPenStyle() { return penStyle; }
    void setColor( const QColor &_col ) { penColor = _col; repaint(); }
    const QColor& getColor() { return penColor; }
    void setPenWidth( int _w ) { penWidth = _w; repaint(); }
    int getPenWidth() { return penWidth; }

    void setPattern( const QColor &_color, int _width, PenStyle _style );
    void setUndefined();
    void setDefined() { undefined = FALSE; repaint(); }

    bool isDefined() { return !undefined; }

signals:
    void clicked( KSpreadPatternSelect *_this );

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

/**
 */
class CellLayoutPageFont : public QWidget
{
    Q_OBJECT
public:
    CellLayoutPageFont( QWidget* parent, CellLayoutDlg *_dlg );

    void apply( KSpreadCell *_cell );
    void apply( ColumnLayout *_col );
    void apply( RowLayout *_row );
    void applyLayout( KSpreadLayout *_obj );

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
    void      slotSearchFont(const QString &);

private:

    void setCombos();

    QGroupBox	 *box1;
    QGroupBox	 *box2;

    QLabel	 *family_label;
    QLabel	 *size_label;
    QLabel       *weight_label;
    QLabel       *style_label;

    QLabel	 *actual_family_label;
    QLabel	 *actual_size_label;
    QLabel       *actual_weight_label;
    QLabel       *actual_style_label;

    QLabel	 *actual_family_label_data;
    QLabel	 *actual_size_label_data;
    QLabel       *actual_weight_label_data;
    QLabel       *actual_style_label_data;

    //QComboBox    *family_combo;
    QListBox     *family_combo;
    QComboBox    *size_combo;
    QComboBox    *weight_combo;
    QComboBox    *style_combo;

    QLabel       *example_label;
    QFont         selFont;
    QCheckBox* strike;
    QCheckBox* underline;
    CellLayoutDlg *dlg;
    QColor textColor;
    bool bTextColorUndefined;
    KColorButton *textColorButton;
    KLineEdit *searchFont;
    KCompletion listFont;
    bool fontChanged;
};

class CellLayoutPageMisc : public QWidget
{
    Q_OBJECT
public:
    CellLayoutPageMisc( QWidget *parent, CellLayoutDlg *_dlg );

    void apply( KSpreadCell *_cell );
    void applyColumn();
    void applyRow();
    void applyLayout( KSpreadCell *_obj );

public slots:
    void slotStyle( int );
protected:
    QComboBox* styleButton;
    int idStyleNormal;
    int idStyleUndef;
    int idStyleButton;
    int idStyleSelect;

    QLineEdit* actionText;
    CellLayoutDlg *dlg;
};

/**
 * KSpreadLayout of numbers.
 * This widget is part of the layout dialog.
 * It allows the user to cinfigure the way numbers are formatted.
 */
class CellLayoutPageFloat : public QWidget
{
    Q_OBJECT
public:
    CellLayoutPageFloat( QWidget *parent, CellLayoutDlg *_dlg );

    void apply( KSpreadCell *_cell );
    void apply( ColumnLayout *_col );
    void apply( RowLayout *_row );
    void applyLayout( KSpreadLayout *_obj );

public slots:
    void slotChangeState();
    void makeformat();
    void makeDateFormat();
    void init();
    void slotChangeValue(int);
    void formatChanged(int);
protected:
    QLineEdit* postfix;
    KIntNumInput* precision;
    QLineEdit* prefix;
    QComboBox *format;
    QRadioButton *number;
    QRadioButton *percent;
    QRadioButton *date;
    QRadioButton *money;
    QRadioButton *scientific;
    QRadioButton *fraction;
    QRadioButton *time;
    QListBox *listFormat;
    QLabel *exampleLabel;
    CellLayoutDlg *dlg;

    //test if value changed
    bool m_bFormatNumberChanged;
    bool m_bFormatColorChanged;
};



class CellLayoutPagePosition : public QWidget
{
    Q_OBJECT
public:
    CellLayoutPagePosition( QWidget *parent, CellLayoutDlg *_dlg );

    void apply( KSpreadCell *_cell );
    void apply( ColumnLayout *_col );
    void apply( RowLayout *_row );
    void applyLayout( KSpreadLayout *_obj );

    int getSizeHeight();
    int getSizeWidth();
    bool getMergedCellState();
public slots:
    void slotChangeHeightState();
    void slotChangeWidthState();
    void slotChangeAngle(int);
    void slotStateChanged(int);
    void slotChangeVerticalState();
    void slotChangeMultiState();
protected:
    QRadioButton *bottom;
    QRadioButton *top;
    QRadioButton *middle;
    QRadioButton *left;
    QRadioButton *right;
    QRadioButton *center;
    QRadioButton *standard;
    QCheckBox *multi;
    QCheckBox *vertical;
    KIntNumInput *width;
    KIntNumInput *height;
    CellLayoutDlg *dlg;
    QCheckBox *defaultWidth;
    QCheckBox *defaultHeight;
    QCheckBox *mergeCell;
    KIntNumInput *angleRotation;
    KIntNumInput *indent;
    bool m_bOptionText;
};



class KSpreadBorder : public QFrame
{
    Q_OBJECT
public:
    KSpreadBorder( QWidget *parent,const char *_name,bool _oneCol,bool _oneRow  );
signals:
    void redraw();
    void choosearea(QMouseEvent * _ev);
protected:
    virtual void paintEvent( QPaintEvent *_ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    bool oneCol;
    bool oneRow;
};

class KSpreadBorderButton : public QPushButton
{
    Q_OBJECT
public:
    KSpreadBorderButton( QWidget *parent, const char *_name );
    void setPenStyle( PenStyle _pat ) { penStyle = _pat;}
    PenStyle getPenStyle() { return penStyle; }
    void setColor( const QColor &_col ) { penColor = _col; }
    const QColor& getColor() { return penColor; }
    void setPenWidth( int _w ) { penWidth = _w; }
    int getPenWidth() { return penWidth; }
    bool isChanged() { return changed; }
    void setChanged(bool _changed ) { changed=_changed;}
    void setUndefined();
    void unselect();
 signals:
    void clicked(KSpreadBorderButton *);
 protected:
    virtual void mousePressEvent( QMouseEvent *_ev );
    PenStyle penStyle;
    QColor penColor;
    int penWidth;
    bool changed;

};

class CellLayoutPageBorder : public QWidget
{
  Q_OBJECT
public:
    CellLayoutPageBorder( QWidget *parent, CellLayoutDlg *_dlg );

    void applyOutline( int _left, int _top, int _right, int _bottom );
    void invertState(KSpreadBorderButton *_button);
    QPixmap paintFormatPixmap(PenStyle _style);

public slots:
    void changeState(KSpreadBorderButton *_this);
    void preselect( KSpreadBorderButton *_this);
    void draw();
    void slotSetColorButton( const QColor &_color );
    void slotUnselect2( KSpreadPatternSelect *_select );
    void loadIcon( QString pix,KSpreadBorderButton *_button);
    void slotPressEvent(QMouseEvent *_ev);
    void slotChangeStyle(int );
    void slotChangeStyle(const QString & );
    void cutomize_chosen_slot();

protected:
    KSpreadBorderButton *top;
    KSpreadBorderButton *bottom;
    KSpreadBorderButton *left;
    KSpreadBorderButton *right;
    KSpreadBorderButton *vertical;
    KSpreadBorderButton *horizontal;
    KSpreadBorderButton *fallDiagonal;
    KSpreadBorderButton *goUpDiagonal;
    KSpreadBorderButton *remove;
    KSpreadBorderButton *all;
    KSpreadBorderButton *outline;
    KSpreadPatternSelect* pattern1;
    KSpreadPatternSelect* pattern2;
    KSpreadPatternSelect* pattern3;
    KSpreadPatternSelect* pattern4;
    KSpreadPatternSelect* pattern5;
    KSpreadPatternSelect* pattern6;
    KSpreadPatternSelect* pattern7;
    KSpreadPatternSelect* pattern8;
    KSpreadPatternSelect* pattern9;
    KSpreadPatternSelect* pattern10;
    KSpreadPatternSelect* preview;
    QComboBox* size;
    QComboBox* style;
    KColorButton* color;
    QCheckBox* customize;
    QColor currentColor;
    KSpreadBorder *area;
    CellLayoutDlg *dlg;
};

class KSpreadBrushSelect : public QFrame
{
    Q_OBJECT
public:
    KSpreadBrushSelect( QWidget *parent, const char *_name );

    void setBrushStyle( BrushStyle _pat ) { brushStyle = _pat; repaint(); }
    BrushStyle getBrushStyle() { return brushStyle; }
    QColor getBrushColor() { return brushColor; }
    void setBrushColor(QColor _c) { brushColor=_c;}
    void setPattern( const QColor &_color, BrushStyle _style );

signals:
    void clicked( KSpreadBrushSelect *_this );

public slots:
    void slotUnselect();
    void slotSelect();

protected:
    virtual void paintEvent( QPaintEvent *_ev );
    virtual void mousePressEvent( QMouseEvent *_ev );

    BrushStyle brushStyle;
    QColor brushColor;
    bool selected;
};


class CellLayoutPagePattern : public QWidget
{
    Q_OBJECT
public:
    CellLayoutPagePattern( QWidget *parent, CellLayoutDlg *_dlg );

    void apply( KSpreadCell *_cell );
    void apply( ColumnLayout *_col );
    void apply( RowLayout *_row );
    void applyLayout( KSpreadLayout *_obj );

    void init();
public slots:
    void slotUnselect2( KSpreadBrushSelect *_select );
    void slotSetColorButton( const QColor &_color );
    void slotSetBackgroundColor( const QColor &_color );
    void slotNotAnyColor( );
protected:
    KSpreadBrushSelect *selectedBrush;
    KSpreadBrushSelect *brush1;
    KSpreadBrushSelect *brush2;
    KSpreadBrushSelect *brush3;
    KSpreadBrushSelect *brush4;
    KSpreadBrushSelect *brush5;
    KSpreadBrushSelect *brush6;
    KSpreadBrushSelect *brush7;
    KSpreadBrushSelect *brush8;
    KSpreadBrushSelect *brush9;
    KSpreadBrushSelect *brush10;
    KSpreadBrushSelect *brush11;
    KSpreadBrushSelect *brush12;
    KSpreadBrushSelect *brush13;
    KSpreadBrushSelect *brush14;
    KSpreadBrushSelect *brush15;
    KSpreadBrushSelect *current;
    KColorButton* color;
    QPushButton* notAnyColor;
    QColor currentColor;

    QColor bgColor;
    bool bBgColorUndefined;
    KColorButton *bgColorButton;
    bool b_notAnyColor;
    CellLayoutDlg *dlg;
};

/**
 */
class CellLayoutDlg : public QObject
{
    Q_OBJECT
public:
    /**
     * Create a layout dlg for the rectangular area in '_table'.
     */
    CellLayoutDlg( KSpreadView *_view, KSpreadTable *_table, int _left, int _top, int _right, int _bottom );

    ~CellLayoutDlg();

    void init();
    void initParameters(KSpreadLayout *_obj,int column,int row);
    /**
     * Run the dialogs event loop and return when closed.
     */
    int exec();

    KSpreadTable* getTable() {	return table; }

    bool isSingleCell() { return ( left == right && top == bottom ); }

    // The layout of the selected area
    PenStyle leftBorderStyle;
    int leftBorderWidth;
    bool bLeftBorderStyle;
    QColor leftBorderColor;
    bool bLeftBorderColor;
    PenStyle rightBorderStyle;
    int rightBorderWidth;
    bool bRightBorderStyle;
    QColor rightBorderColor;
    bool bRightBorderColor;
    PenStyle topBorderStyle;
    int topBorderWidth;
    bool bTopBorderStyle;
    QColor topBorderColor;
    bool bTopBorderColor;
    PenStyle bottomBorderStyle;
    int bottomBorderWidth;
    bool bBottomBorderStyle;
    QColor bottomBorderColor;
    bool bBottomBorderColor;

    PenStyle verticalBorderStyle;
    int verticalBorderWidth;
    bool bVerticalBorderStyle;
    QColor verticalBorderColor;
    bool bVerticalBorderColor;

    PenStyle horizontalBorderStyle;
    int horizontalBorderWidth;
    bool bHorizontalBorderStyle;
    QColor horizontalBorderColor;
    bool bHorizontalBorderColor;

    PenStyle fallDiagonalStyle;
    int fallDiagonalWidth;
    bool bFallDiagonalStyle;
    QColor fallDiagonalColor;
    bool bfallDiagonalColor;

    PenStyle goUpDiagonalStyle;
    int goUpDiagonalWidth;
    bool bGoUpDiagonalStyle;
    QColor goUpDiagonalColor;
    bool bGoUpDiagonalColor;
    BrushStyle brushStyle;
    QColor brushColor;


    bool oneCol;
    bool oneRow;

    QString prefix;
    QString postfix;
    int precision;
    KSpreadCell::FloatFormat floatFormat;
    bool bFloatFormat;
    KSpreadCell::FloatColor floatColor;
    bool bFloatColor;
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
    KSpreadCell::Style eStyle;
    QString actionText;
    KSpreadCell::Align alignX;
    KSpreadCell::AlignY alignY;

    bool bMultiRow;
    bool bVerticalText;

    int heigthSize;
    int widthSize;

    int indent;

    QPixmap* formatOnlyNegSignedPixmap;
    QPixmap* formatRedOnlyNegSignedPixmap;
    QPixmap* formatRedNeverSignedPixmap;
    QPixmap* formatAlwaysSignedPixmap;
    QPixmap* formatRedAlwaysSignedPixmap;
    QPixmap* undefinedPixmap;

    int textRotation;
    bool bTextRotation;

    KSpreadCell::formatNumber formatNumber;
    bool bFormatNumber;

    bool m_bValue;
    bool m_bDate;
    bool m_bTime;

    QTime m_time;
    QDate m_date;
    QString cellText;
    double m_value;

    bool isMerged;
    bool oneCell;


    // The rectangular area for which this dlg has been opened.
    int left;
    int right;
    int top;
    int bottom;


public slots:
    void slotApply();

protected:

    /**
     * Used to draw the @ref #formatPixmap and friends.
     */
    QPixmap* paintFormatPixmap( const char *_string1, const QColor & _color1,
				const char *_string2, const QColor & _color2 );

    CellLayoutPageFloat *floatPage;
    CellLayoutPageBorder *borderPage;
    CellLayoutPageMisc *miscPage;
    CellLayoutPageFont *fontPage;
    CellLayoutPagePosition *positionPage;
    CellLayoutPagePattern *patternPage;
    QTabDialog *tab;

    /**
     * The table that opened this dlg.
     */
    KSpreadTable *table;

    KSpreadView *m_pView;


};

#endif
