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

#ifndef __kspread_layout_h__
#define __kspread_layout_h__

class KSpreadTable;
class KSpreadCanvas;

class QDomElement;
class QDomDocument;

#include <qpen.h>
#include <qcolor.h>
#include <qfont.h>

#include "kspread_global.h"

class KLocale;

/**
 */
class KSpreadLayout
{
public:
    enum Align { Left = 1, Center = 2, Right = 3, Undefined = 4 };
    enum AlignY { Top = 1, Middle = 2, Bottom =3 };
    enum FloatFormat { AlwaysSigned = 1, AlwaysUnsigned = 2, OnlyNegSigned = 3 };
    enum FloatColor { NegRed = 1, AllBlack = 2 };
    enum formatNumber { Number=0,Money=10,Percentage=25,Scientific=30,ShortDate=35,TextDate=36, Time=50,
    SecondeTime=51,Time_format1=52,Time_format2=53,Time_format3=54,
    fraction_half=70,fraction_quarter=71,fraction_eighth=72,fraction_sixteenth=73,
     fraction_tenth=74,fraction_hundredth=75,fraction_one_digit=76,
     fraction_two_digits=77,fraction_three_digits=78,
     date_format1=200,date_format2=201,date_format3=202,date_format4=203,
     date_format5=204,date_format6=205,date_format7=206,date_format8=207,
     date_format9=208,date_format10=209,date_format11=210,date_format12=211,
     date_format13=212,date_format14=213,date_format15=214,date_format16=215,
     date_format17=216 };

    enum Properties{ PAlign  = 0x01,
		     PAlignY = 0x02,
		     PFaktor = 0x04,
		     PPrefix = 0x08,
		     PPostfix = 0x10,
		     PLeftBorder = 0x20,
		     PRightBorder = 0x40,
		     PTopBorder = 0x80,
		     PBottomBorder = 0x100,
		     PFallDiagonal = 0x200,
		     PGoUpDiagonal = 0x400,
		     PBackgroundBrush = 0x800,
		     PFont = 0x1000,
		     PTextPen = 0x2000,
		     PBackgroundColor = 0x4000,
		     PFloatFormat = 0x8000,
		     PFloatColor = 0x10000,
		     PMultiRow = 0x20000,
		     PVerticalText = 0x40000,
                     PPrecision = 0x80000,
                     PFormatNumber = 0x100000,
                     PAngle = 0x200000,
                     PComment = 0x400000,
                     PIndent = 0x800000,
		     PDontPrintText = 0x1000000};

    KSpreadLayout( KSpreadTable *_table );
    virtual ~KSpreadLayout();

    void copy( KSpreadLayout &_l );

    void defaultStyleLayout();

    ////////////////////////////////
    //
    // Loading and saving
    //
    ////////////////////////////////

    bool load( const QDomElement& f,PasteMode pm );
    bool loadLayout( const QDomElement& f,PasteMode pm=Normal);
    QDomElement save( QDomDocument& doc ) const;

    QDomElement saveLayout( QDomDocument& doc ) const;

    ////////////////////////////////
    //
    // Properties
    //
    ////////////////////////////////


    void clearProperties();
    void clearProperty( Properties p );

    void clearNoFallBackProperties(  ) ;
    void clearNoFallBackProperties( Properties p ) ;
    void setNoFallBackProperties(Properties p);
    bool hasNoFallBackProperties( Properties p ) const ;

    ////////////////////////////////
    //
    // Methods for setting layout stuff.
    //
    ////////////////////////////////

    virtual void setAlign( Align _align );
    virtual void setAlignY( AlignY _alignY );
    virtual void setFaktor( double _d );
    virtual void setPrefix( const QString& _prefix );
    virtual void setPostfix( const QString& _postfix );
    virtual void setPrecision( int _p );

    virtual void setLeftBorderPen( const QPen& _p );
    void setLeftBorderStyle( Qt::PenStyle s );
    void setLeftBorderColor( const QColor & _c );
    void setLeftBorderWidth( int _w );

    virtual void setTopBorderPen( const QPen& _p );
    void setTopBorderStyle( Qt::PenStyle s );
    void setTopBorderColor( const QColor & _c );
    void setTopBorderWidth( int _w );

    virtual void setRightBorderPen( const QPen& p );
    void setRightBorderStyle( Qt::PenStyle _s );
    void setRightBorderColor( const QColor & _c );
    void setRightBorderWidth( int _w );

    virtual void setBottomBorderPen( const QPen& p );
    void setBottomBorderStyle( Qt::PenStyle _s );
    void setBottomBorderColor( const QColor & _c );
    void setBottomBorderWidth( int _w );

    virtual void setFallDiagonalPen( const QPen& _p );
    void setFallDiagonalStyle( Qt::PenStyle s );
    void setFallDiagonalColor( const QColor & _c );
    void setFallDiagonalWidth( int _w );

    virtual void setGoUpDiagonalPen( const QPen& _p );
    void setGoUpDiagonalStyle( Qt::PenStyle s );
    void setGoUpDiagonalColor( const QColor & _c );
    void setGoUpDiagonalWidth( int _w );

    virtual void setBackGroundBrush( const QBrush& _p);
    void setBackGroundBrushStyle( Qt::BrushStyle s);
    void setBackGroundBrushColor( const QColor & _c);

    virtual void setTextFont( const QFont& _f );
    void setTextFontSize( int _s );
    void setTextFontFamily( const QString& _f );
    void setTextFontBold( bool _b );
    void setTextFontItalic( bool _i );
    void setTextFontUnderline( bool _i );
    void setTextFontStrike( bool _i );

    virtual void setTextPen( const QPen& _p );
    void setTextColor( const QColor & _c );

    virtual void setBgColor( const QColor & _c );

    virtual void setFloatFormat( FloatFormat _f );
    virtual void setFloatColor( FloatColor _c );

    virtual void setMultiRow( bool _b );

    virtual void setVerticalText( bool _b );

    virtual void setFormatNumber(formatNumber _format);

    virtual void setAngle(int _angle);

    virtual void setComment( const QString& c );

    virtual void setIndent( int _indent );

    virtual void setDontPrintText ( bool _b);
    ////////////////////////////////
    //
    // Methods for querying layout stuff.
    //
    ////////////////////////////////

    virtual const QPen& leftBorderPen( int col, int row ) const;
    int leftBorderWidth( int col, int row ) const;
    Qt::PenStyle leftBorderStyle( int col, int row ) const;
    const QColor& leftBorderColor( int col, int row ) const;

    virtual const QPen& topBorderPen( int col, int row ) const;
    int topBorderWidth( int col, int row ) const;
    Qt::PenStyle topBorderStyle( int col, int row ) const;
    const QColor& topBorderColor( int col, int row ) const;

    virtual const QPen& rightBorderPen( int col, int row ) const;
    int rightBorderWidth( int col, int row ) const;
    Qt::PenStyle rightBorderStyle( int col, int row ) const;
    const QColor& rightBorderColor( int col, int row ) const;

    virtual const QPen& bottomBorderPen( int col, int row ) const;
    int bottomBorderWidth( int col, int row ) const;
    Qt::PenStyle bottomBorderStyle( int col, int row ) const;
    const QColor& bottomBorderColor( int col, int row ) const;

    virtual const QPen& fallDiagonalPen( int col, int row ) const;
    int fallDiagonalWidth( int col, int row ) const;
    Qt::PenStyle fallDiagonalStyle( int col, int row ) const;
    const QColor& fallDiagonalColor( int col, int row ) const;

    virtual const QPen& goUpDiagonalPen( int col, int row ) const;
    int goUpDiagonalWidth( int col, int row ) const;
    Qt::PenStyle goUpDiagonalStyle( int col, int row ) const;
    const QColor& goUpDiagonalColor( int col, int row ) const;

    virtual const QBrush& backGroundBrush( int col, int row ) const;
    Qt::BrushStyle backGroundBrushStyle( int col, int row ) const;
    const QColor& backGroundBrushColor(int col, int row ) const;

    /**
     * @return the precision of the floating point representation.
     */
    virtual int precision( int col, int row ) const;
    /**
     * @return the prefix of a numeric value ( for example "$" )
     */
    virtual QString prefix( int col, int row ) const;
    /**
     * @return the postfix of a numeric value ( for example "DM" )
     */
    virtual QString postfix( int col, int row ) const;
    /**
     * @return the way of formatting a floating point value
     */
    virtual FloatFormat floatFormat( int col, int row ) const;
    /**
     * @return the color format of a floating point value
     */
    virtual FloatColor floatColor( int col, int row ) const;

    virtual const QPen& textPen( int col, int row ) const;
    /**
     * @return the text color.
     */
    const QColor& textColor( int col, int row ) const;

    /**
     * @param _col the column this cell is assumed to be in
     * @param _row the row this cell is assumed to be in
     *
     * @return the background color.
     */
    virtual const QColor& bgColor( int col, int row ) const;

    virtual const QFont& textFont( int col, int row ) const;
    int textFontSize( int col, int row ) const;
    QString textFontFamily( int col, int row ) const;
    bool textFontBold( int col, int row ) const;
    bool textFontItalic( int col, int row ) const;
    bool textFontUnderline( int col, int row ) const;
    bool textFontStrike( int col, int row ) const;

    virtual Align align( int col, int row ) const;
    virtual AlignY alignY( int col, int row ) const;

    virtual double faktor( int col, int row ) const;

    virtual bool multiRow( int col, int row ) const;

    virtual bool verticalText( int col, int row ) const;

    virtual formatNumber getFormatNumber(int col, int row )const ;

    virtual int getAngle(int col, int row) const;

    virtual QString comment(int col, int row) const;

    virtual int getIndent(int col, int row) const;

    virtual bool getDontprintText( int col, int row) const;

    KSpreadTable* table() { return m_pTable; }
    const KSpreadTable* table() const { return m_pTable; }

    KLocale* locale();

    virtual bool hasProperty( Properties p ) const;




protected:
    virtual const QPen& rightBorderPen() const;
    virtual const QPen& bottomBorderPen() const;

    /**
     * Default implementation does nothing.
     */
    virtual void layoutChanged();

    /**
     * Default implementation returns 0.
     */
    virtual KSpreadLayout* fallbackLayout( int col, int row );
    /**
     * Default implementation returns 0.
     */
    virtual const KSpreadLayout* fallbackLayout( int col, int row ) const;

    /**
     * Default implementation returns TRUE.
     */
    virtual bool isDefault() const;

    QDomElement createElement( const QString &tagName, const QFont &font, QDomDocument &doc ) const;
    QDomElement createElement( const QString& tagname, const QPen& pen, QDomDocument &doc ) const;
    QFont toFont(QDomElement &element) const;
    QPen toPen(QDomElement &element) const;

    /**
     * Tells whether text may be broken into multiple lines.
     */
    bool m_bMultiRow;

    /**
     * Alignment of the text
     */
    Align m_eAlign;
    /**
     * Aligment of the text at top middle or bottom
     */
    AlignY m_eAlignY;

    /**
     * The font used to draw the text
     */
    QFont m_textFont;
    /**
     * The pen used to draw the text
     */
    QPen m_textPen;
    /**
     * The background color
     */
    QColor m_bgColor;

    /**
     * The pen used to draw the right border
     */
    QPen m_rightBorderPen;

    /**
     * The pen used to draw the bottom border
     */
    QPen m_bottomBorderPen;

    /**
     * The pen used to draw the left border
     */
    QPen m_leftBorderPen;

    /**
     * The pen used to draw the top border
     */
    QPen m_topBorderPen;

    /**
     * The pen used to draw the diagonal
     */
    QPen m_fallDiagonalPen;
    /**
     * The pen used to draw the the diagonal which go up
     */
    QPen m_goUpDiagonalPen;

    /**
     * The brush used to draw the background.
     */
    QBrush m_backGroundBrush;

    /**
     * The precision of the floatinf point representation
     * If precision is -1, this means that no precision is specified.
     */
    int m_iPrecision;
    /**
     * The prefix of a numeric value ( for example "$" )
     * May be empty.
     */
    QString m_strPrefix;
    /**
     * The postfix of a numeric value ( for example "DM" )
     * May be empty.
     */
    QString m_strPostfix;
    /**
     * The way of formatting a floating point value
     */
    FloatFormat m_eFloatFormat;
    /**
     * The color format of a floating point value
     */
    FloatColor m_eFloatColor;

    /**
     * Used to display 0.15 as 15% for example.
     */
    double m_dFaktor;

    bool m_bVerticalText;

    KSpreadTable *m_pTable;

    uint m_mask;

    /**
    * used  m_bNoFallBack when you put default value in a cell and
    * not used column/row parameters
    */
    uint m_bNoFallBack;

    formatNumber m_eFormatNumber;

    /**
    * give angle of rotation
    * default is null
    */
    int m_rotateAngle;

     /**
     * Stores a comment string.
     */
    QString m_strComment;

    /**
    * Give indent
    */
    int m_indent;
    /** 
     * Don't print text
     */
    bool m_bDontPrintText;

private:
    void setProperty( Properties p );

    /**
     * Currently just used for better abstraction.
     */
    const QPen& leftBorderPen() const;
    const QPen& topBorderPen() const;
    const QPen& fallDiagonalPen() const;
    const QPen& goUpDiagonalPen() const;
    const QBrush& backGroundBrush() const;
    const QFont& textFont() const;
    const QPen& textPen() const;
};

/**
 */
class RowLayout : public KSpreadLayout
{
public:
    RowLayout( KSpreadTable *_table, int _row );
    ~RowLayout();

    virtual QDomElement save( QDomDocument&, int yshift = 0 );
    virtual bool load( const QDomElement& row, int yshift = 0, PasteMode sp=Normal );

    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the height in zoomed pixels.
     */
    int height( KSpreadCanvas *_canvas = 0L );
    /**
     * @return the width in millimeters.
     */
    float mmHeight() { return m_bHide ? 0 : m_fHeight; }
    /**
     * Sets the height to _h zoomed pixels.
     *
     * @param _h is calculated in display pixels. The function cares for zooming.
     * @param _canvas is needed to get information about the zooming factor.
     */
    void setHeight( int _h, KSpreadCanvas *_canvas = 0L );
    /**
     * Sets the height.
     *
     * @param '_h' is assumed to be a unzoomed millimeter value.
     */
    void setMMHeight( float _h );

    /**
     * Use this function to tell this layout that it is the default layout.
     */
    void setDefault() { m_bDefault = TRUE; }
    /**
     * @reimp
     */
    bool isDefault() const;

    /**
     * @return the row for this RowLayout. May be 0 if this is the default layout.
     *
     * @see #row
     */
    int row() { return m_iRow; }

    void setRow( int _r ) { m_iRow = _r; }

    void setDisplayDirtyFlag() { m_bDisplayDirtyFlag = true; }
    void clearDisplayDirtyFlag() { m_bDisplayDirtyFlag = false; }

    RowLayout* next() { return m_next; }
    RowLayout* previous() { return m_prev; }
    void setNext( RowLayout* c ) { m_next = c; }
    void setPrevious( RowLayout* c ) { m_prev = c; }

    /**
     * @reimp
     */
    const QPen& bottomBorderPen( int col, int row ) const;
    /**
     * @reimp
     */
    void setBottomBorderPen( const QPen& p );
    /**
     * @reimp
     */
    const QPen& topBorderPen( int col, int row ) const;
    /**
     * @reimp
     */
    void setTopBorderPen( const QPen& p );

    void setHide(bool _hide) { m_bHide=_hide;}
    bool isHide() { return m_bHide;}

protected:
    /**
     * @reimp
     */
    KSpreadLayout* fallbackLayout( int col, int row );
    /**
     * @reimp
     */
    const KSpreadLayout* fallbackLayout( int col, int row ) const;

    /**
     * Width of the cell in unzoomed millimeters.
     */
    float m_fHeight;

    /**
     * Flag that indicates whether this is the default layout.
     *
     * @see #isDefault
     * @see #setDefault
     */
    bool m_bDefault;
    /**
     * This is the row to which this layout belongs. If this value is 0, then
     * this might be the default layout.
     *
     * @see #row
     */
    int m_iRow;

    bool m_bDisplayDirtyFlag;
    bool m_bHide;
    RowLayout* m_next;
    RowLayout* m_prev;
};

/**
 */
class ColumnLayout : public KSpreadLayout
{
public:
    ColumnLayout( KSpreadTable *_table, int _column );
    ~ColumnLayout();

    virtual QDomElement save( QDomDocument&, int xshift = 0 );
    virtual bool load( const QDomElement& row, int xshift = 0,PasteMode sp=Normal );

    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the width in zoomed pixels.
     */
    int width( KSpreadCanvas *_canvas = 0L );
    /**
     * @return the width in millimeters.
     */
    float mmWidth() { return m_bHide ? 0 : m_fWidth; }
    /**
     * Sets the width to _w zoomed pixels.
     *
     * @param _w is calculated in display pixels. The function cares for
     *           zooming.
     * @param _canvas is needed to get information about the zooming factor.
     */
    void setWidth( int _w, KSpreadCanvas *_canvas = 0L );
    /**
     * Sets the width.
     *
     * @param _w is assumed to be a unzoomed millimeter value.
     */
    void setMMWidth( float _w );

    /**
     * Use this function to tell this layout that it is the default layout.
     */
    void setDefault() { m_bDefault = TRUE; }
    /**
     * @reimp
     */
    bool isDefault() const;

    /**
     * @return the column of this ColumnLayout. May be 0 if this is the default layout.
     *
     * @see #column
     */
    int column() { return m_iColumn; }

    void setColumn( int _c ) { m_iColumn = _c; }

    void setDisplayDirtyFlag() { m_bDisplayDirtyFlag = true; }
    void clearDisplayDirtyFlag() { m_bDisplayDirtyFlag = false; }

    ColumnLayout* next() { return m_next; }
    ColumnLayout* previous() { return m_prev; }
    void setNext( ColumnLayout* c ) { m_next = c; }
    void setPrevious( ColumnLayout* c ) { m_prev = c; }

    /**
     * @reimp
     */
    const QPen& rightBorderPen( int col, int row ) const;
    /**
     * @reimp
     */
    void setRightBorderPen( const QPen& p );
    /**
     * @reimp
     */
    const QPen& leftBorderPen( int col, int row ) const;
    /**
     * @reimp
     */
    void setLeftBorderPen( const QPen& p );

    void setHide(bool _hide) { m_bHide=_hide;}
    bool isHide() { return m_bHide;}


protected:
    /**
     * @reimp
     */
    KSpreadLayout* fallbackLayout( int col, int row );
    /**
     * @reimp
     */
    const KSpreadLayout* fallbackLayout( int col, int row ) const;

    /**
     * Height of the cells in unzoomed millimeters.
     */
    float m_fWidth;

    /**
     * Flag that indicates whether this is the default layout.
     *
     * @see #isDefault
     * @see #setDefault
     */
    bool m_bDefault;
    /**
     * This is the column to which this layout belongs. If this value is 0, then
     * this might be the default layout.
     *
     * @see #column
     */
    int m_iColumn;

    bool m_bDisplayDirtyFlag;

    bool m_bHide;

    ColumnLayout* m_next;
    ColumnLayout* m_prev;
};

#endif
