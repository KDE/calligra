#ifndef __kspread_layout_h__
#define __kspread_layout_h__

class KSpreadTable;
class KSpreadView;

#include <qpen.h>
#include <qcolor.h>
#include <qfont.h>

/**
 */
class KSpreadLayout
{
public:
    enum Align { Left = 1, Center = 2, Right = 3, Undefined = 4 };
    enum FloatFormat { AlwaysSigned = 1, AlwaysUnsigned = 2, OnlyNegSigned = 3 };
    enum FloatColor { NegRed = 1, AllBlack = 2 };

    KSpreadLayout( KSpreadTable *_table );
    ~KSpreadLayout();

    void KSpreadLayout::copy( KSpreadLayout &_l );
    
    virtual void setAlign( Align _align ) { m_eAlign = _align; }
    virtual void setFaktor( double _d ) { m_dFaktor = _d; }
    virtual void setPrefix( const char * _prefix ) { m_strPrefix = _prefix; }
    virtual void setPostfix( const char * _postfix ) { m_strPostfix = _postfix; }
    virtual void setPrecision( int _p ) { m_iPrecision = _p; }
    
    virtual void setLeftBorderStyle( PenStyle s ) { m_leftBorderPen.setStyle( s ); }
    virtual void setTopBorderStyle( PenStyle s ) { m_topBorderPen.setStyle( s ); }

    virtual void setLeftBorderColor( const QColor & _c ) { m_leftBorderPen.setColor( _c ); }
    virtual void setTopBorderColor( const QColor & _c ) { m_topBorderPen.setColor( _c ); }

    virtual void setLeftBorderWidth( int _w ) { m_iLeftBorderWidth = _w; }
    virtual void setTopBorderWidth( int _w ) { m_iTopBorderWidth = _w; }

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
     * Since the @ref KSpreadView  supports zooming, you can get the value zoomed
     * or not scaled. The not scaled value may be of interest in a
     * layout dialog for example.
     *
     * @return the border width of the left border
     */
    virtual int leftBorderWidth( KSpreadView *_view = 0L );
    virtual int topBorderWidth( KSpreadView *view = 0L );

    /**
     * @return the style used to draw the left border.
     */
    virtual PenStyle leftBorderStyle() { return m_leftBorderPen.style(); }
    virtual PenStyle topBorderStyle() { return m_topBorderPen.style(); }

    /**
     * @return the precision of the floating point representation.
     */
    virtual int precision() { return m_iPrecision; }
    /**
     * @return the prefix of a numeric value ( for example "$" )
     */
    virtual const char* prefix();
    /**
     * @return the postfix of a numeric value ( for example "DM" )
     */
    virtual const char* postfix();
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

    virtual const QFont& textFont() { return m_textFont; }
    virtual int textFontSize() { return m_textFont.pointSize(); }
    virtual const char* textFontFamily() { return m_textFont.family(); }
    virtual bool textFontBold() { return m_textFont.bold(); }
    virtual bool textFontItalic() { return m_textFont.italic(); }

    virtual Align align() { return m_eAlign; }

    virtual double faktor() { return m_dFaktor; }
    
    virtual bool multiRow() { return m_bMultiRow; }
    
protected:
    /**
     * Tells wether text may be broken into multiple lines.
     */
    bool m_bMultiRow;
    
    /**
     * Alignment of the text
     */
    Align m_eAlign;

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
    
    /**
     * @param _view is needed to get information about the zooming factor.
     *
     * @return the height in zoomed pixels.
     */
    int height( KSpreadView *_view = 0L );
    /**
     * @return the width in millimeters.
     */
    float mmHeight() { return m_fHeight; }
    /**
     * Sets the height to _h zoomed pixels.
     *
     * @param _h is calculated in display pixels. The function cares for zooming.
     * @param _view is needed to get information about the zooming factor.
     */
    void setHeight( int _h, KSpreadView *_view = 0L );
    /**
     * Sets the height.
     *
     * @param '_h' is assumed to be a unzoomed millimeter value.
     */
    void setMMHeight( float _h ) { m_fHeight = _h; }

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
     * Flag that indicates wether this is the default layout.
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
};

/**
 */
class ColumnLayout : public KSpreadLayout
{
public:
    ColumnLayout( KSpreadTable *_table, int _column );
    
    /**
     * @param _view is needed to get information about the zooming factor.
     *
     * @return the width in zoomed pixels.
     */
    int width( KSpreadView *_view = 0L );
    /**
     * @return the width in millimeters.
     */
    float mmWidth() { return m_fWidth; }
    /**
     * Sets the width to _w zoomed pixels.
     *
     * @param _w is calculated in display pixels. The function cares for
     *           zooming.
     * @param _view is needed to get information about the zooming factor.
     */
    void setWidth( int _w, KSpreadView *_view = 0L );
    /**
     * Sets the width.
     *
     * @param _w is assumed to be a unzoomed millimeter value.
     */
    void setMMWidth( float _w ) { m_fWidth = _w; }
    
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
     * Flag that indicates wether this is the default layout.
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
};

#endif


