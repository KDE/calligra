/* This file is part of the KDE project
   Copyright (C) 1998, 1999  Torben Weis <weis@kde.org>
   Copyright (C) 2000 - 2003 The KSpread Team
                              www.koffice.org/kspread

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

#ifndef __kspread_format_h__
#define __kspread_format_h__

class KSpreadSheet;
class KSpreadCanvas;

class QDomElement;
class QDomDocument;
class DCOPObject;
class KoOasisStyles;

#include <qbrush.h>
#include <qpen.h>
#include <qcolor.h>
#include <qfont.h>

#include "kspread_global.h"

class KLocale;
class KSpreadCurrency;
class KSpreadStyle;
class KoStyleStack;
class KoGenStyle;

/**
 */
class KSpreadFormat
{
public:
    enum Align { Left = 1, Center = 2, Right = 3, Undefined = 4 };
    enum AlignY { Top = 1, Middle = 2, Bottom = 3, UndefinedY = 4 };
    enum FloatFormat { AlwaysSigned = 1, AlwaysUnsigned = 2, OnlyNegSigned = 3 };
    enum FloatColor { NegRed = 1, AllBlack = 2, NegBrackets = 3, NegRedBrackets = 4 };
    enum FormatType { Number=0, Text_format=5, Money=10, Percentage=25, Scientific=30,
                      ShortDate=35, TextDate=36, Time=50, SecondeTime=51,
                      Time_format1=52, Time_format2=53, Time_format3=54, Time_format4=55,
                      Time_format5=56, Time_format6=57, Time_format7=58, Time_format8=59,
                      fraction_half=70,fraction_quarter=71,fraction_eighth=72,fraction_sixteenth=73,
                      fraction_tenth=74,fraction_hundredth=75,fraction_one_digit=76,
                      fraction_two_digits=77,fraction_three_digits=78,
                      date_format1=200,date_format2=201,date_format3=202,date_format4=203,
                      date_format5=204,date_format6=205,date_format7=206,date_format8=207,
                      date_format9=208,date_format10=209,date_format11=210,date_format12=211,
                      date_format13=212,date_format14=213,date_format15=214,date_format16=215,
                      date_format17=216,date_format18=217,date_format19=218,date_format20=219,
                      date_format21=220,date_format22=221,date_format23=222,date_format24=223,
                      date_format25=224,date_format26=225,
                      Custom = 300 };

    enum Properties{ PAlign  = 0x01,
		     PAlignY = 0x02,
		     PFactor = 0x04,
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
                     PFormatType = 0x100000,
                     PAngle = 0x200000,
                     PComment = 0x400000,
                     PIndent = 0x800000,
		     PDontPrintText = 0x1000000,
                     PCustomFormat = 0x2000000,
                     PNotProtected = 0x4000000,
                     PHideAll = 0x8000000,
                     PHideFormula = 0x10000000 };

    struct Currency
    {
      int type;
      QString symbol;
    };

    /**
     * don't pass in 0 for _style: only if you copy another format on this directly after...
     */
    KSpreadFormat( KSpreadSheet * _sheet, KSpreadStyle * _style );
    virtual ~KSpreadFormat();

    void copy( const KSpreadFormat & _l );

    void defaultStyleFormat();

    ////////////////////////////////
    //
    // Loading and saving
    //
    ////////////////////////////////

    bool load( const QDomElement & f, PasteMode pm, bool paste = false );
    bool loadFormat( const QDomElement & f, PasteMode pm = Normal, bool paste = false );
    QDomElement save( QDomDocument& doc,int _col, int _row,bool force = false, bool copy = false ) const;
    QDomElement saveFormat( QDomDocument& doc, bool force = false, bool copy = false ) const;
    QDomElement saveFormat( QDomDocument& doc, int _col, int _row, bool force = false, bool copy = false ) const;
    virtual bool loadOasisStyleProperties(KoStyleStack & styleStack, const KoOasisStyles& oasisStyles );
    virtual void saveOasisCellStyle( KoGenStyle &currentCellStyle, int _col, int _row );
    virtual void KSpreadFormat::saveOasisCellStyle( KoGenStyle &currentCellStyle );
    void saveOasisFontCellStyle( KoGenStyle &currentCellStyle, const QFont &_font );

    bool loadFontOasisStyle( KoStyleStack & font );
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

    ///////////////////////////////
    //
    // Flags
    //
    ///////////////////////////////
    enum FormatFlags{ Flag_MultiRow      = 0x00000001,
                      Flag_VerticalText  = 0x00000002,
                      Flag_DontPrintText = 0x00000004,
                      Flag_HideAll       = 0x00000008,
                      Flag_HideFormula   = 0x00000010,
                      Flag_NotProtected  = 0x00000020
                      /* reserve the bits 0xFFFF0000 for subclasses to use */
                    };
    void clearFlag( FormatFlags flag );
    void setFlag( FormatFlags flag );
    bool testFlag( FormatFlags flag ) const;


    ////////////////////////////////
    //
    // Methods for setting format stuff.
    //
    ////////////////////////////////

    static void setGlobalColWidth( double width );
    static void setGlobalRowHeight( double height );
    static double globalRowHeight();
    static double globalColWidth();


    virtual void setKSpreadStyle( KSpreadStyle * style );

    /**
     * sets the format of the content, e.g. #.##0.00, dd/mmm/yyyy,...
     */
    virtual void setFormatString( QString const & format );

    virtual void setAlign( Align _align );
    virtual void setAlignY( AlignY _alignY );
    virtual void setFactor( double _d );
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

    virtual void setFormatType(FormatType _format);

    virtual void setAngle(int _angle);

    virtual void setComment( const QString& c );

    virtual void setIndent( double _indent );

    virtual void setDontPrintText ( bool _b );
    virtual void setNotProtected ( bool _b );
    virtual void setHideAll( bool _b );
    virtual void setHideFormula( bool _b );

    virtual void setCurrency( Currency const & c );
    virtual void setCurrency( int type, QString const & symbol );

    ////////////////////////////////
    //
    // Methods for querying format stuff.
    //
    ////////////////////////////////

    QString const & getFormatString( int col, int row ) const;

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

    virtual uint bottomBorderValue( int col, int row ) const;
    virtual uint rightBorderValue( int col, int row ) const;
    virtual uint leftBorderValue( int col, int row ) const;
    virtual uint topBorderValue( int col, int row ) const;

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

    virtual const QFont textFont( int col, int row ) const;
    int textFontSize( int col, int row ) const;
    QString const & textFontFamily( int col, int row ) const;
    bool textFontBold( int col, int row ) const;
    bool textFontItalic( int col, int row ) const;
    bool textFontUnderline( int col, int row ) const;
    bool textFontStrike( int col, int row ) const;

    virtual Align align( int col, int row ) const;
    virtual AlignY alignY( int col, int row ) const;

    virtual double factor( int col, int row ) const;

    virtual bool multiRow( int col, int row ) const;

    virtual bool verticalText( int col, int row ) const;

    virtual FormatType getFormatType(int col, int row )const ;

    virtual int getAngle(int col, int row) const;

    virtual QString comment(int col, int row) const;
    virtual QString * commentP( int col, int row ) const;

    virtual double getIndent(int col, int row) const;

    virtual bool getDontprintText( int col, int row) const;
    virtual bool notProtected( int col, int row) const;
    virtual bool isHideAll( int col, int row) const;
    virtual bool isHideFormula( int col, int row) const;
    virtual bool isProtected( int col, int row ) const;

    KSpreadStyle * kspreadStyle() const { return m_pStyle; }
    KSpreadSheet* table() { return m_pTable; }
    const KSpreadSheet* table() const { return m_pTable; }

    KLocale* locale()const;

    virtual bool hasProperty( Properties p, bool withoutParent = false ) const;

    /**
     * returns false if no currency information is set or
     * doesn't apply
     */
    virtual bool currencyInfo( Currency & currency) const;

    QString getCurrencySymbol() const;
    QFont font() const;

protected:
    virtual const QPen& rightBorderPen() const;
    virtual const QPen& bottomBorderPen() const;

    /**
     * Default implementation does nothing.
     */
    virtual void formatChanged();

    /**
     * Default implementation returns 0.
     */
    virtual KSpreadFormat* fallbackFormat( int col, int row );
    /**
     * Default implementation returns 0.
     */
    virtual const KSpreadFormat* fallbackFormat( int col, int row ) const;

    /**
     * Default implementation returns TRUE.
     */
    virtual bool isDefault() const;

    KSpreadSheet * m_pTable;
    KSpreadStyle * m_pStyle;

    uint m_mask;

    /**
    * used  m_bNoFallBack when you put default value in a cell and
    * not used column/row parameters
    */
    uint m_bNoFallBack;

    Q_UINT32 m_flagsMask;

     /**
     * Stores a comment string.
     */
    QString * m_strComment;

private:
    void setProperty( Properties p );

    /**
     * Currently just used for better abstraction.
     */
    const QPen & leftBorderPen() const;
    const QPen & topBorderPen() const;
    const QPen & fallDiagonalPen() const;
    const QPen & goUpDiagonalPen() const;
    const QBrush & backGroundBrush() const;
    const QFont textFont() const;
    const QPen  & textPen() const;
};

/**
 */
class RowFormat : public KSpreadFormat
{
public:
    RowFormat( KSpreadSheet * _sheet, int _row );
    ~RowFormat();

    virtual DCOPObject* dcopObject();

    virtual QDomElement save( QDomDocument&, int yshift = 0, bool copy = false ) const;
    virtual bool load( const QDomElement& row, int yshift = 0, PasteMode sp = Normal, bool paste = false );
    virtual bool loadOasis( const QDomElement& row, QDomElement * rowStyle );

    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the height in zoomed pixels as integer value.
     */
    int height( const KSpreadCanvas *_canvas = 0L ) const;
    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the height in zoomed pixels as double value.
     * Use this function, if you want to work with height without having rounding problems.
     */
    double dblHeight( const KSpreadCanvas *_canvas = 0L ) const;
    /**
     * @return the height in millimeters.
     */
    double mmHeight() const;
    /**
     * Sets the height to _h zoomed pixels.
     *
     * @param _h is calculated in display pixels as integer value. The function cares for zooming.
     * @param _canvas is needed to get information about the zooming factor.
     */
    void setHeight( int _h, const KSpreadCanvas *_canvas = 0L );
    /**
     * Sets the height to _h zoomed pixels.
     *
     * @param _h is calculated in display pixels as double value. The function cares for zooming.
     * Use this function when setting the height, to not get rounding problems.
     * @param _canvas is needed to get information about the zooming factor.
     */
    void setDblHeight( double _h, const KSpreadCanvas *_canvas = 0L );
    /**
     * Sets the height.
     *
     * @param '_h' is assumed to be a unzoomed millimeter value.
     */
    void setMMHeight( double _h );

    /**
     * Use this function to tell this format that it is the default format.
     */
    void setDefault() { m_bDefault = TRUE; }
    /**
     * @reimp
     */
    bool isDefault() const;

    /**
     * @return the row for this RowFormat. May be 0 if this is the default format.
     *
     * @see #row
     */
    int row() const { return m_iRow; }

    void setRow( int _r ) { m_iRow = _r; }

    void setDisplayDirtyFlag() { m_bDisplayDirtyFlag = true; }
    void clearDisplayDirtyFlag() { m_bDisplayDirtyFlag = false; }

    RowFormat* next() const { return m_next; }
    RowFormat* previous() const { return m_prev; }
    void setNext( RowFormat* c ) { m_next = c; }
    void setPrevious( RowFormat* c ) { m_prev = c; }

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

    /**
     * Sets the hide flag
     */
    void setHide( bool _hide );
    bool isHide()const { return m_bHide;}

protected:
    /**
     * @reimp
     */
    KSpreadFormat* fallbackFormat( int col, int row );
    /**
     * @reimp
     */
    const KSpreadFormat* fallbackFormat( int col, int row ) const;

    /**
     * Width of the cell in unzoomed points.
     */
    double m_fHeight;

    /**
     * Flag that indicates whether this is the default format.
     *
     * @see #isDefault
     * @see #setDefault
     */
    bool m_bDefault;
    /**
     * This is the row to which this format belongs. If this value is 0, then
     * this might be the default format.
     *
     * @see #row
     */
    int m_iRow;

    bool m_bDisplayDirtyFlag;
    bool m_bHide;
    RowFormat* m_next;
    RowFormat* m_prev;
    DCOPObject*m_dcop;
};

/**
 */
class ColumnFormat : public KSpreadFormat
{
public:
    ColumnFormat( KSpreadSheet *_table, int _column );
    ~ColumnFormat();

    virtual QDomElement save( QDomDocument&, int xshift = 0, bool copy = false ) const;
    virtual bool load( const QDomElement& row, int xshift = 0,PasteMode sp = Normal, bool paste = false );
    virtual DCOPObject* dcopObject();

    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the width in zoomed pixels as integer.
     */
    int width( const KSpreadCanvas *_canvas = 0L ) const;
    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the width in zoomed pixels as double.
     * Use this function, if you want to use the width and later restore it back,
     * so you don't get rounding problems
     */
    double dblWidth( const KSpreadCanvas *_canvas = 0L ) const;
    /**
     * @return the width in millimeters.
     */
    double mmWidth() const;
    /**
     * Sets the width to _w zoomed pixels.
     *
     * @param _w is calculated in display pixels. The function cares for
     *           zooming.
     * @param _canvas is needed to get information about the zooming factor.
     */
    void setWidth( int _w, const KSpreadCanvas *_canvas = 0L );
    /**
     * Sets the width to _w zoomed pixels as double value.
     * Use this function to set the width without getting rounding problems.
     *
     * @param _w is calculated in display pixels. The function cares for
     *           zooming.
     * @param _canvas is needed to get information about the zooming factor.
     */
    void setDblWidth( double _w, const KSpreadCanvas *_canvas = 0L );
    /**
     * Sets the width.
     *
     * @param _w is assumed to be a unzoomed millimeter value.
     */
    void setMMWidth( double _w );

    /**
     * Use this function to tell this format that it is the default format.
     */
    void setDefault() { m_bDefault = TRUE; }
    /**
     * @reimp
     */
    bool isDefault() const;

    /**
     * @return the column of this ColumnFormat. May be 0 if this is the default format.
     *
     * @see #column
     */
    int column() const { return m_iColumn; }

    void setColumn( int _c ) { m_iColumn = _c; }

    void setDisplayDirtyFlag() { m_bDisplayDirtyFlag = true; }
    void clearDisplayDirtyFlag() { m_bDisplayDirtyFlag = false; }

    ColumnFormat* next() const { return m_next; }
    ColumnFormat* previous() const { return m_prev; }
    void setNext( ColumnFormat* c ) { m_next = c; }
    void setPrevious( ColumnFormat* c ) { m_prev = c; }

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

    void setHide( bool _hide );
    bool isHide()const { return m_bHide;}


protected:
    /**
     * @reimp
     */
    KSpreadFormat* fallbackFormat( int col, int row );
    /**
     * @reimp
     */
    const KSpreadFormat* fallbackFormat( int col, int row ) const;

    /**
     * Width of the cells in unzoomed pixels.
     */
    double m_fWidth;

    /**
     * Flag that indicates whether this is the default format.
     *
     * @see #isDefault
     * @see #setDefault
     */
    bool m_bDefault;
    /**
     * This is the column to which this format belongs. If this value is 0, then
     * this might be the default format.
     *
     * @see #column
     */
    int m_iColumn;

    bool m_bDisplayDirtyFlag;

    bool m_bHide;

    ColumnFormat* m_next;
    ColumnFormat* m_prev;
    DCOPObject*m_dcop;
};

class KSpreadCurrency
{
 public:

  enum currencyFormat { Gnumeric, OpenCalc, ApplixSpread,
                        GobeProductiveSpread, HancomSheet };

  KSpreadCurrency();
  ~KSpreadCurrency();

  KSpreadCurrency(int index);

  /**
   * If code doesn't fit to index the index gets ignored
   */
  KSpreadCurrency(int index, QString const & code);

  /**
   * code: e.g. EUR, USD,..
   * Looks up index, if code found more than once: saved without country info
   * currencyFormat: in Gnumeric the code is: [$EUR]
   *                 saves some work in the filter...
   */
  KSpreadCurrency(QString const & code, currencyFormat format);
  KSpreadCurrency & operator=(int type);
  KSpreadCurrency & operator=(char const * code);
  bool operator==(KSpreadCurrency const & cur) const;
  bool operator==(int type) const;
  operator int() const;

  QString getCode() const;
  QString getCountry() const;
  QString getName() const;
  QString getDisplayCode() const;
  int     getIndex() const;

  static QString getChooseString(int type, bool & ok);
  static QString getDisplaySymbol(int type);
  static QString getCurrencyCode( int type);

  /**
   * Code for use in Gnumeric export filter
   */
  QString getExportCode(currencyFormat format) const;

 private:
  int     m_type;
  QString m_code;
};

#endif
