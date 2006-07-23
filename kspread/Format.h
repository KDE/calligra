/* This file is part of the KDE project
   Copyright (C) 1998, 1999  Torben Weis <weis@kde.org>
   Copyright (C) 2000 - 2003 The KSpread Team <koffice-devel@kde.org>

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

#ifndef __FORMAT_H__
#define __FORMAT_H__

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QPen>

#include <koffice_export.h>

#include "Global.h"
#include "Style.h"

class QDomElement;
class QDomDocument;
class DCOPObject;
class KLocale;
class KoGenStyle;
class KoGenStyles;
class KoOasisLoadingContext;
class KoOasisStyles;
class KoStyleStack;

namespace KSpread
{
class Canvas;
class Cell;
class Currency;
class Sheet;

/**
 */
class KSPREAD_EXPORT Format
{
public:
    // TODO Stefan: merge with Style::FlagsSet
/*    enum Properties{ PAlign  = 0x01,
		     PAlignY = 0x02,
         	//PFactor was here
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
                     PHideFormula = 0x10000000 }; */
    /**
     * Constructor.
     * @param sheet The sheet this format belongs to.
     * @todo The format should not belong to a sheet. The cell belongs to a sheet. Move
     * the sheet pointer to Cell.
     * @param style The style this format should use.
     * You should pass @c 0 in here, only if you copy another Format on
     * this directly after.
     */
    Format( Sheet* sheet, Style* style );
    /**
     * Destructor.
     * The associated style's reference counter is decremented and if it has no further
     * references, it is deleted.
     */
    virtual ~Format();

    void copy( const Format& other );

    void defaultStyleFormat();

    ////////////////////////////////
    //
    // Loading and saving
    //
    ////////////////////////////////

    bool load( const QDomElement & f, Paste::Mode pm, bool paste = false );
    bool loadFormat( const QDomElement & f, Paste::Mode pm = Paste::Normal, bool paste = false );
    QDomElement save( QDomDocument& doc,int _col, int _row,bool force = false, bool copy = false ) const;
    QDomElement saveFormat( QDomDocument& doc, bool force = false, bool copy = false ) const;
    QDomElement saveFormat( QDomDocument& doc, int _col, int _row, bool force = false, bool copy = false ) const;

    void loadOasisStyle(/* const QDomElement& element,*/ KoOasisLoadingContext& context );
    bool loadOasisStyleProperties(KoStyleStack & styleStack, const KoOasisStyles& oasisStyles );
    bool loadFontOasisStyle( KoStyleStack & font );
    /**
     * @return the OASIS style's name, if it was created. QString::null otherwise.
     */
    QString saveOasisCellStyle( KoGenStyle &currentCellStyle, KoGenStyles &mainStyle );

    ////////////////////////////////
    //
    // Style::FlagsSet
    //
    ////////////////////////////////
    uint propertiesMask() { return m_mask; } // For the cell inspector only.
    void clearProperties();
    void clearProperty( Style::FlagsSet p );

    void clearNoFallBackProperties(  ) ;
    void clearNoFallBackProperties( Style::FlagsSet p ) ;
    void setNoFallBackProperties(Style::FlagsSet p);
    bool hasNoFallBackProperties( Style::FlagsSet p ) const ;

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


    void setStyle( Style * style );
    void setCell( Cell* cell ) { m_pCell = cell; }

    /**
     * sets the format of the content, e.g. #.##0.00, dd/mmm/yyyy,...
     */
    void setFormatString( QString const & format );

    void setAlign( Style::HAlign _align );
    void setAlignY( Style::VAlign _alignY );
    void setPrefix( const QString& _prefix );
    void setPostfix( const QString& _postfix );
    void setPrecision( int _p );

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

    void setFallDiagonalPen( const QPen& _p );
    void setFallDiagonalStyle( Qt::PenStyle s );
    void setFallDiagonalColor( const QColor & _c );
    void setFallDiagonalWidth( int _w );

    void setGoUpDiagonalPen( const QPen& _p );
    void setGoUpDiagonalStyle( Qt::PenStyle s );
    void setGoUpDiagonalColor( const QColor & _c );
    void setGoUpDiagonalWidth( int _w );

    void setBackGroundBrush( const QBrush& _p);
    void setBackGroundBrushStyle( Qt::BrushStyle s);
    void setBackGroundBrushColor( const QColor & _c);

    void setTextFont( const QFont& _f );
    void setTextFontSize( int _s );
    void setTextFontFamily( const QString& _f );
    void setTextFontBold( bool _b );
    void setTextFontItalic( bool _i );
    void setTextFontUnderline( bool _i );
    void setTextFontStrike( bool _i );

    void setTextPen( const QPen& _p );
    void setTextColor( const QColor & _c );

    void setBgColor( const QColor & _c );

    void setFloatFormat( Style::FloatFormat _f );
    void setFloatColor( Style::FloatColor _c );

    void setMultiRow( bool _b );

    void setVerticalText( bool _b );

    void setFormatType(FormatType _format);

    void setAngle(int _angle);

    void setComment( const QString& c );

    void setIndent( double _indent );

    void setDontPrintText( bool _b );
    void setNotProtected( bool _b );
    void setHideAll( bool _b );
    void setHideFormula( bool _b );

    void setCurrency( Style::Currency const & c );
    void setCurrency( int type, QString const & symbol );

    ////////////////////////////////
    //
    // Methods for querying format stuff.
    //
    ////////////////////////////////

    QString const & getFormatString( int col, int row ) const;

    virtual const QPen& leftBorderPen( int col, int row ) const;
    int leftBorderWidth( int col, int row ) const;
    Qt::PenStyle leftBorderStyle( int col, int row ) const;
    const QColor leftBorderColor( int col, int row ) const;

    virtual const QPen& topBorderPen( int col, int row ) const;
    int topBorderWidth( int col, int row ) const;
    Qt::PenStyle topBorderStyle( int col, int row ) const;
    const QColor topBorderColor( int col, int row ) const;

    virtual const QPen& rightBorderPen( int col, int row ) const;
    int rightBorderWidth( int col, int row ) const;
    Qt::PenStyle rightBorderStyle( int col, int row ) const;
    const QColor rightBorderColor( int col, int row ) const;

    virtual const QPen& bottomBorderPen( int col, int row ) const;
    int bottomBorderWidth( int col, int row ) const;
    Qt::PenStyle bottomBorderStyle( int col, int row ) const;
    const QColor bottomBorderColor( int col, int row ) const;

    const QPen& fallDiagonalPen( int col, int row ) const;
    int fallDiagonalWidth( int col, int row ) const;
    Qt::PenStyle fallDiagonalStyle( int col, int row ) const;
    const QColor fallDiagonalColor( int col, int row ) const;

    const QPen& goUpDiagonalPen( int col, int row ) const;
    int goUpDiagonalWidth( int col, int row ) const;
    Qt::PenStyle goUpDiagonalStyle( int col, int row ) const;
    const QColor goUpDiagonalColor( int col, int row ) const;

    const QBrush& backGroundBrush( int col, int row ) const;
    Qt::BrushStyle backGroundBrushStyle( int col, int row ) const;
    const QColor backGroundBrushColor(int col, int row ) const;

    uint bottomBorderValue( int col, int row ) const;
    uint rightBorderValue( int col, int row ) const;
    uint leftBorderValue( int col, int row ) const;
    uint topBorderValue( int col, int row ) const;

    /**
     * @return the precision of the floating point representation.
     */
    int precision( int col, int row ) const;
    /**
     * @return the prefix of a numeric value ( for example "$" )
     */
    QString prefix( int col, int row ) const;
    /**
     * @return the postfix of a numeric value ( for example "DM" )
     */
    QString postfix( int col, int row ) const;
    /**
     * @return the way of formatting a floating point value
     */
    Style::FloatFormat floatFormat( int col, int row ) const;
    /**
     * @return the color format of a floating point value
     */
    Style::FloatColor floatColor( int col, int row ) const;

    const QPen& textPen( int col, int row ) const;
    /**
     * @return the text color.
     */
    const QColor textColor( int col, int row ) const;

    /**
     * @param col the column this cell is assumed to be in
     * @param row the row this cell is assumed to be in
     *
     * @return the background color.
     */
    const QColor bgColor( int col, int row ) const;

    const QFont textFont( int col, int row ) const;
    int textFontSize( int col, int row ) const;
    QString const & textFontFamily( int col, int row ) const;
    bool textFontBold( int col, int row ) const;
    bool textFontItalic( int col, int row ) const;
    bool textFontUnderline( int col, int row ) const;
    bool textFontStrike( int col, int row ) const;

    Style::HAlign align( int col, int row ) const;
    Style::VAlign alignY( int col, int row ) const;

    bool multiRow( int col, int row ) const;

    bool verticalText( int col, int row ) const;

    FormatType getFormatType(int col, int row )const ;

    int getAngle(int col, int row) const;

    const QString* comment() const { return m_strComment; };
    QString comment(int col, int row) const;
    QString * commentP( int col, int row ) const;

    double getIndent(int col, int row) const;

    bool getDontprintText( int col, int row) const;
    bool notProtected( int col, int row) const;
    bool isHideAll( int col, int row) const;
    bool isHideFormula( int col, int row) const;
    bool isProtected( int col, int row ) const;

    Style* style() const { return m_pStyle; }
    Sheet* sheet() { return m_pSheet; }
    const Sheet* sheet() const { return m_pSheet; }

    bool hasProperty( Style::FlagsSet p, bool withoutParent = false ) const;

    /**
     * returns false if no currency information is set or
     * doesn't apply
     */
    bool currencyInfo( Style::Currency & currency) const;

    QString getCurrencySymbol() const;
    QFont font() const;

protected:
    const QPen& rightBorderPen() const;
    const QPen& bottomBorderPen() const;

    /**
     * If a cell is set, marks its Layout and TextFormat as dirty.
     */
    void formatChanged();

    /**
     * If a cell is set, it returns the row format of the sheet. Otherwise, it returns 0.
     */
    virtual Format* fallbackFormat( int col, int row );
    /**
     * If a cell is set, it returns the row format of the sheet. Otherwise, it returns 0.
     */
    virtual const Format* fallbackFormat( int col, int row ) const;

    /**
     * Default implementation returns true.
     */
    virtual bool isDefault() const;

    Sheet * m_pSheet;
    Style * m_pStyle;

    uint m_mask;

    /**
    * used  m_bNoFallBack when you put default value in a cell and
    * not used column/row parameters
    */
    uint m_bNoFallBack;

    quint32 m_flagsMask;

     /**
     * Stores a comment string.
     */
    QString * m_strComment;

private:
    void setProperty( Style::FlagsSet p );

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

    Cell* m_pCell;
};

/**
 */
class KSPREAD_EXPORT RowFormat : public Format
{
public:
    RowFormat( Sheet * _sheet, int _row );
    ~RowFormat();

    DCOPObject* dcopObject();

    QDomElement save( QDomDocument&, int yshift = 0, bool copy = false ) const;
    bool load( const QDomElement& row, int yshift = 0, Paste::Mode sp = Paste::Normal, bool paste = false );
    bool loadOasis( const QDomElement& row, QDomElement * rowStyle );

    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the height in zoomed pixels as integer value.
     */
    int height( const Canvas *_canvas = 0 ) const;
    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the height in zoomed pixels as double value.
     * Use this function, if you want to work with height without having rounding problems.
     */
    double dblHeight( const Canvas *_canvas = 0 ) const;
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
    void setHeight( int _h, const Canvas *_canvas = 0 );
    /**
     * Sets the height to _h zoomed pixels.
     *
     * @param _h is calculated in display pixels as double value. The function cares for zooming.
     * Use this function when setting the height, to not get rounding problems.
     * @param _canvas is needed to get information about the zooming factor.
     */
    void setDblHeight( double _h, const Canvas *_canvas = 0 );
    /**
     * Sets the height.
     *
     * @param '_h' is assumed to be a unzoomed millimeter value.
     */
    void setMMHeight( double _h );

    /**
     * Use this function to tell this format that it is the default format.
     */
    void setDefault() { m_bDefault = true; }
    /**
     * @reimp
     */
    virtual bool isDefault() const;

    /**
     * @return the row for this RowFormat. May be 0 if this is the default format.
     *
     * @see #row
     */
    int row() const { return m_iRow; }

    void setRow( int _r ) { m_iRow = _r; }

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
    void setHide( bool _hide, bool repaint = true );
    bool isHide()const { return m_bHide;}

protected:
    /**
     * @reimp
     */
    virtual Format* fallbackFormat( int col, int row );
    /**
     * @reimp
     */
    virtual const Format* fallbackFormat( int col, int row ) const;

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

    bool m_bHide;
    RowFormat* m_next;
    RowFormat* m_prev;
    DCOPObject*m_dcop;
};

/**
 */
class KSPREAD_EXPORT ColumnFormat : public Format
{
public:
    ColumnFormat( Sheet *_sheet, int _column );
    ~ColumnFormat();

    QDomElement save( QDomDocument&, int xshift = 0, bool copy = false ) const;
    bool load( const QDomElement& row, int xshift = 0,Paste::Mode sp = Paste::Normal, bool paste = false );
    DCOPObject* dcopObject();

    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the width in zoomed pixels as integer.
     */
    int width( const Canvas *_canvas = 0 ) const;
    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the width in zoomed pixels as double.
     * Use this function, if you want to use the width and later restore it back,
     * so you don't get rounding problems
     */
    double dblWidth( const Canvas *_canvas = 0 ) const;
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
    void setWidth( int _w, const Canvas *_canvas = 0 );
    /**
     * Sets the width to _w zoomed pixels as double value.
     * Use this function to set the width without getting rounding problems.
     *
     * @param _w is calculated in display pixels. The function cares for
     *           zooming.
     * @param _canvas is needed to get information about the zooming factor.
     */
    void setDblWidth( double _w, const Canvas *_canvas = 0 );
    /**
     * Sets the width.
     *
     * @param _w is assumed to be a unzoomed millimeter value.
     */
    void setMMWidth( double _w );

    /**
     * Use this function to tell this format that it is the default format.
     */
    void setDefault() { m_bDefault = true; }
    /**
     * @reimp
     */
    virtual bool isDefault() const;

    /**
     * @return the column of this ColumnFormat. May be 0 if this is the default format.
     *
     * @see #column
     */
    int column() const { return m_iColumn; }

    void setColumn( int _c ) { m_iColumn = _c; }

    ColumnFormat* next() const { return m_next; }
    ColumnFormat* previous() const { return m_prev; }
    void setNext( ColumnFormat* c ) { m_next = c; }
    void setPrevious( ColumnFormat* c ) { m_prev = c; }

    /**
     * @reimp
     */
    virtual const QPen& rightBorderPen( int col, int row ) const;
    /**
     * @reimp
     */
    virtual void setRightBorderPen( const QPen& p );
    /**
     * @reimp
     */
    virtual const QPen& leftBorderPen( int col, int row ) const;
    /**
     * @reimp
     */
    virtual void setLeftBorderPen( const QPen& p );

    void setHide( bool _hide );
    bool isHide()const { return m_bHide;}


protected:
    /**
     * @reimp
     */
    Format* fallbackFormat( int col, int row );
    /**
     * @reimp
     */
    const Format* fallbackFormat( int col, int row ) const;

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

    bool m_bHide;

    ColumnFormat* m_next;
    ColumnFormat* m_prev;
    DCOPObject*m_dcop;
};

class KSPREAD_EXPORT Currency
{
 public:

  enum currencyFormat { Native, Gnumeric, OpenCalc, ApplixSpread,
                        GobeProductiveSpread, HancomSheet };

  Currency();
  ~Currency();

  Currency(int index);

  /**
   * If code doesn't fit to index the index gets ignored
   */
  Currency(int index, QString const & code);

  /**
   * code: e.g. EUR, USD,..
   * Looks up index, if code found more than once: saved without country info
   * currencyFormat: in Gnumeric the code is: [$EUR]
   *                 saves some work in the filter...
   */
  Currency(QString const & code, currencyFormat format = Native);
  Currency & operator=(int type);
  Currency & operator=(char const * code);
  bool operator==(Currency const & cur) const;
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

} // namespace KSpread

#endif
