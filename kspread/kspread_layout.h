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

/**
 */
class KSpreadLayout
{
public:
    enum Align { Left = 1, Center = 2, Right = 3, Undefined = 4 };
    enum AlignY { Top = 1, Middle = 2, Bottom =3 };
    enum FloatFormat { AlwaysSigned = 1, AlwaysUnsigned = 2, OnlyNegSigned = 3 };
    enum FloatColor { NegRed = 1, AllBlack = 2 };

    KSpreadLayout( KSpreadTable *_table );
    virtual ~KSpreadLayout();

    void copy( KSpreadLayout &_l );

    virtual void setAlign( Align _align ) { m_eAlign = _align; }
    virtual void setAlignY( AlignY _alignY) { m_eAlignY = _alignY; }
    virtual void setFaktor( double _d ) { m_dFaktor = _d; }
    virtual void setPrefix( const char * _prefix ) { m_strPrefix = _prefix; }
    virtual void setPostfix( const char * _postfix ) { m_strPostfix = _postfix; }
    virtual void setPrecision( int _p ) { m_iPrecision = _p; }

    virtual void setLeftBorderStyle( Qt::PenStyle s ) { m_leftBorderPen.setStyle( s ); }
    virtual void setTopBorderStyle( Qt::PenStyle s ) { m_topBorderPen.setStyle( s ); }
    virtual void setFallDiagonalStyle( Qt::PenStyle s ) { m_fallDiagonalPen.setStyle( s ); }
    virtual void setGoUpDiagonalStyle( Qt::PenStyle s ) { m_goUpDiagonalPen.setStyle( s ); }

    virtual void setLeftBorderColor( const QColor & _c ) { m_leftBorderPen.setColor( _c ); }
    virtual void setTopBorderColor( const QColor & _c ) { m_topBorderPen.setColor( _c ); }
    virtual void setFallDiagonalColor( const QColor & _c ) { m_fallDiagonalPen.setColor( _c ); }
    virtual void setGoUpDiagonalColor( const QColor & _c ) { m_goUpDiagonalPen.setColor( _c ); }


    virtual void setLeftBorderWidth( int _w ) { m_iLeftBorderWidth = _w; }
    virtual void setTopBorderWidth( int _w ) { m_iTopBorderWidth = _w; }
    virtual void setFallDiagonalWidth( int _w ) { m_iFallDiagonalWidth = _w; }
    virtual void setGoUpDiagonalWidth( int _w ) { m_iGoUpDiagonalWidth = _w; }


    virtual void setTextFontSize( int _s ) { m_textFont.setPointSize( _s ); }
    virtual void setTextFontFamily( const char *_f ) { m_textFont.setFamily( _f ); }
    virtual void setTextFontBold( bool _b ) { m_textFont.setBold( _b ); }
    virtual void setTextFontItalic( bool _i ) { m_textFont.setItalic( _i ); }
    virtual void setTextFont( const QFont& _f ) { m_textFont = _f; }
    virtual void setTextColor( const QColor & _c ) { m_textPen.setColor( _c ); m_textColor = _c; }
    virtual void setBgColor( const QColor & _c ) { m_bgColor = _c; }

    virtual void setFloatFormat( FloatFormat _f ) { m_eFloatFormat = _f; }
    virtual void setFloatColor( FloatColor _c ) { m_eFloatColor = _c; }

    virtual void setMultiRow( bool _b ) { m_bMultiRow = _b; }

    /**
     * Since the @ref KSpreadCanvas  supports zooming, you can get the value zoomed
     * or not scaled. The not scaled value may be of interest in a
     * layout dialog for example.
     *
     * @return the border width of the left border
     */
    virtual int leftBorderWidth( KSpreadCanvas *_canvas = 0L );
    virtual int topBorderWidth( KSpreadCanvas *canvas = 0L );
    virtual int fallDiagonalWidth( KSpreadCanvas *canvas = 0L );
    virtual int goUpDiagonalWidth( KSpreadCanvas *canvas = 0L );

    /**
     * @return the style used to draw the left border.
     */
    virtual Qt::PenStyle leftBorderStyle() { return m_leftBorderPen.style(); }
    virtual Qt::PenStyle topBorderStyle() { return m_topBorderPen.style(); }
    virtual Qt::PenStyle fallDiagonalStyle() { return m_fallDiagonalPen.style(); }
    virtual Qt::PenStyle goUpDiagonalStyle() { return m_goUpDiagonalPen.style(); }

    /**
     * @return the precision of the floating point representation.
     */
    virtual int precision() { return m_iPrecision; }
    /**
     * @return the prefix of a numeric value ( for example "$" )
     */
    virtual QString prefix() const;
    /**
     * @return the postfix of a numeric value ( for example "DM" )
     */
    virtual QString postfix() const;
    /**
     * @return the way of formatting a floating point value
     */
    virtual FloatFormat floatFormat() { return m_eFloatFormat; }
    /**
     * @return the color format of a floating point value
     */
    virtual FloatColor floatColor() { return m_eFloatColor; }
    /**
     * @return the text color.
     */
    virtual const QColor& textColor() { return m_textColor; }
    /**
     * @return the background color
     */
    virtual const QColor& bgColor() { return m_bgColor; }

    /**
     * @return the color of the left color
     */
    virtual const QColor& leftBorderColor() { return m_leftBorderPen.color(); }
    virtual const QColor& topBorderColor() { return m_topBorderPen.color(); }
    virtual const QColor& fallDiagonalColor() { return m_fallDiagonalPen.color(); }
    virtual const QColor& goUpDiagonalColor() { return m_goUpDiagonalPen.color(); }

    virtual const QFont& textFont() { return m_textFont; }
    virtual int textFontSize() { return m_textFont.pointSize(); }
    virtual const char* textFontFamily() { return m_textFont.family(); }
    virtual bool textFontBold() { return m_textFont.bold(); }
    virtual bool textFontItalic() { return m_textFont.italic(); }

    virtual Align align() { return m_eAlign; }

    virtual AlignY alignY() { return m_eAlignY; }

    virtual double faktor() { return m_dFaktor; }

    virtual bool multiRow() { return m_bMultiRow; }

    virtual const QPen& leftBorderPen() { return m_leftBorderPen; }
    virtual const QPen& topBorderPen() { return m_topBorderPen; }
    virtual const QPen& fallDiagonalPen() { return m_fallDiagonalPen; }
    virtual const QPen& goUpDiagonalPen() { return m_goUpDiagonalPen; }
   
    virtual const QPen& textPen() { return m_textPen; }

    virtual void setTextPen( const QPen& _p ) { m_textPen = _p; m_textColor = _p.color(); }
    virtual void setLeftBorderPen( const QPen& _p ) { m_leftBorderPen = _p; m_iLeftBorderWidth = _p.width(); }
    virtual void setFallDiagonalPen( const QPen& _p ) { m_fallDiagonalPen = _p; m_iFallDiagonalWidth = _p.width(); }
    virtual void setGoUpDiagonalPen( const QPen& _p ) { m_goUpDiagonalPen = _p; m_iGoUpDiagonalWidth = _p.width(); }
    virtual void setTopBorderPen( const QPen& _p ) { m_topBorderPen = _p; m_iTopBorderWidth = _p.width(); }

    KSpreadTable* table() { return m_pTable; }

protected:
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
     * The color used to draw the text
     */
    QColor m_textColor;
    /**
     * The background color
     */
    QColor m_bgColor;

    /**
     * The not zoomed border width of the left border
     */
    int m_iLeftBorderWidth;
    /**
     * The pen used to draw the left border
     */
    QPen m_leftBorderPen;

    /**
     * The not zoomed border width of the top border
     */
    int m_iTopBorderWidth;
    /**
     * The pen used to draw the top border
     */
    QPen m_topBorderPen;
    /**
     * The not zoomed border width of the diagonal which fall
     */
    int m_iFallDiagonalWidth;
    /**
     * The pen used to draw the diagonal
     */
    QPen m_fallDiagonalPen;
    /**
     * The not zoomed border width of the digonal which go up
     */
    int m_iGoUpDiagonalWidth;
    /**
     * The pen used to draw the the diagonal which go up
     */
    QPen m_goUpDiagonalPen;
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

    KSpreadTable *m_pTable;
};

/**
 */
class RowLayout : public KSpreadLayout
{
public:
    RowLayout( KSpreadTable *_table, int _row );

    virtual QDomElement save( QDomDocument& );
    virtual bool load( const QDomElement& row );

    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the height in zoomed pixels.
     */
    int height( KSpreadCanvas *_canvas = 0L );
    /**
     * @return the width in millimeters.
     */
    float mmHeight() { return m_fHeight; }
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
     * @return TRUE if this is the default layout.
     */
    bool isDefault() { return m_bDefault; }

    /**
     * @return the last time this layout has been modified.
     *
     * @see #time
     */
    int time() { return m_iTime; }

    /**
     * @return the row for this RowLayout. May be 0 if this is the default layout.
     *
     * @see #row
     */
    int row() { return m_iRow; }

    void setRow( int _r ) { m_iRow = _r; }

    void setDisplayDirtyFlag() { m_bDisplayDirtyFlag = true; }
    void clearDisplayDirtyFlag() { m_bDisplayDirtyFlag = false; }

protected:
    /**
     * Width of the cell in unzoomed millimeters.
     */
    float m_fHeight;

    /**
     * The last time this layout has been modified
     */
    int m_iTime;
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
};

/**
 */
class ColumnLayout : public KSpreadLayout
{
public:
    ColumnLayout( KSpreadTable *_table, int _column );

    virtual QDomElement save( QDomDocument& );
    virtual bool load( const QDomElement& row );

    /**
     * @param _canvas is needed to get information about the zooming factor.
     *
     * @return the width in zoomed pixels.
     */
    int width( KSpreadCanvas *_canvas = 0L );
    /**
     * @return the width in millimeters.
     */
    float mmWidth() { return m_fWidth; }
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
     * @return TRUE if this is the default layout.
     */
    bool isDefault() { return m_bDefault; }

    /**
     * @return the last time this layout has been modified.
     */
    int time() { return m_iTime; }

    /**
     * @return the column of this ColumnLayout. May be 0 if this is the default layout.
     *
     * @see #column
     */
    int column() { return m_iColumn; }

    void setColumn( int _c ) { m_iColumn = _c; }

    void setDisplayDirtyFlag() { m_bDisplayDirtyFlag = true; }
    void clearDisplayDirtyFlag() { m_bDisplayDirtyFlag = false; }

protected:
    /**
     * Height of the cells in unzoomed millimeters.
     */
    float m_fWidth;

    /**
     * The last time this layout has been modified.
     */
    int m_iTime;
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
};

#endif


