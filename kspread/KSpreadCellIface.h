#ifndef KSPREAD_CELL_IFACE_H
#define KSPREAD_CELL_IFACE_H

class QPoint;
class KSpreadTable;
class KSpreadCell;
#include <qcolor.h>
#include <dcopobject.h>

class KSpreadCellIface
{
    K_DCOP
public:
    KSpreadCellIface();

    void setCell( KSpreadTable* table, const QPoint& point );
k_dcop:
    virtual QString text() const;
    virtual void setText( const QString& text );
    virtual void setValue( int value );
    virtual void setValue( double value );
    virtual double value() const;
    virtual void setBgColor(const QString& _c);
    virtual void setBgColor(int r,int g,int b);
    virtual QString bgColor() const;
    virtual void setTextColor(const QString& _c);
    virtual void setTextColor(int r,int g,int b);
    virtual QString textColor()const ;
    virtual void setComment(const QString &_comment);
    virtual QString comment()const;
    virtual void setAngle(int angle);
    virtual int angle() const ;
    virtual void setVerticalText(bool _vertical);
    virtual bool verticalText() const ;
    virtual void setMultiRow(bool _multi);
    virtual bool multiRow() const;
    virtual void setAlign(const QString &_align);
    virtual QString align() const;
    virtual void setAlignY(const QString &_alignY);
    virtual QString alignY() const;
    virtual void setPrefix(const QString &_prefix);
    virtual QString prefix() const;
    virtual void setPostfix(const QString &_postfix);
    virtual QString postfix() const;
    virtual void setFormatNumber(const QString &_formatNumber);
    virtual QString getFormatNumber() const;
    virtual void setPrecision(int  _p);
    virtual int precision() const;
    virtual void setFaktor( double _factor );
    virtual double faktor()const;
    //font
    virtual void setTextFontBold( bool _b );
    virtual bool textFontBold( ) const;
    virtual void setTextFontItalic( bool _b );
    virtual bool textFontItalic( ) const;
    virtual void setTextFontUnderline( bool _b );
    virtual bool textFontUnderline(  ) const;
    virtual void setTextFontStrike( bool _b );
    virtual bool textFontStrike( ) const;
    virtual void setTextFontSize( int _size );
    virtual int textFontSize() const;
    virtual void setTextFontFamily( const QString& _font );
    virtual QString textFontFamily() const;
    //border left
    virtual void setLeftBorderStyle( const QString& _style );
    virtual void setLeftBorderColor(const QString& _c);
    virtual void setLeftBorderColor(int r,int g,int b);
    virtual void setLeftBorderWidth( int _size );
    virtual int leftBorderWidth() const;
    virtual QString leftBorderColor() const;
    virtual QString leftBorderStyle() const;
    //border right
    virtual void setRightBorderStyle( const QString& _style );
    virtual void setRightBorderColor(const QString& _c);
    virtual void setRightBorderColor(int r,int g,int b);
    virtual void setRightBorderWidth( int _size );
    virtual int rightBorderWidth() const;
    virtual QString rightBorderColor() const;
    virtual QString rightBorderStyle() const;
    //border top
    virtual void setTopBorderStyle( const QString& _style );
    virtual void setTopBorderColor(const QString& _c);
    virtual void setTopBorderColor(int r,int g,int b);
    virtual void setTopBorderWidth( int _size );
    virtual int topBorderWidth() const;
    virtual QString topBorderColor() const;
    virtual QString topBorderStyle() const;
    //border bottom
    virtual void setBottomBorderStyle( const QString& _style );
    virtual void setBottomBorderColor(const QString& _c);
    virtual void setBottomBorderColor(int r,int g,int b);
    virtual void setBottomBorderWidth( int _size );
    virtual int bottomBorderWidth() const;
    virtual QString bottomBorderColor() const;
    virtual QString bottomBorderStyle() const;
    // fall back diagonal
    virtual void setFallDiagonalStyle( const QString& _style );
    virtual void setFallDiagonalColor(const QString& _c);
    virtual void setFallDiagonalColor(int r,int g,int b);
    virtual void setFallDiagonalWidth( int _size );
    virtual int fallDiagonalWidth() const;
    virtual QString fallDiagonalColor() const;
    virtual QString fallDiagonalStyle() const;
    //GoUpDiagonal
    virtual void setGoUpDiagonalStyle( const QString& _style );
    virtual void setGoUpDiagonalColor(const QString& _c);
    virtual void setGoUpDiagonalColor(int r,int g,int b);
    virtual void setGoUpDiagonalWidth( int _size );
    virtual int goUpDiagonalWidth() const;
    virtual QString goUpDiagonalColor() const;
    virtual QString goUpDiagonalStyle() const;
    //indent
    virtual void setIndent(int indent);
    virtual int getIndent() const ;

private:
    QPoint m_point;
    KSpreadTable* m_table;
};

#endif
