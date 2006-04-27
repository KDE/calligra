/* This file is part of the KDE project

   Copyright 2002-2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2002 Ariya Hidayat <ariya@kde.org>
   Copyright 2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2001 David Faure <faure@kde.org
   Copyright 2001 Simon Hausmann <hausmann@kde.org>
   Copyright 1999-2001 Laurent Montel <montel@kde.org>
   Copyright 1999 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_CELL_IFACE_H
#define KSPREAD_CELL_IFACE_H

#include <qpoint.h>
#include <qstringlist.h>

#include <dcopobject.h>

class QPoint;

namespace KSpread
{
class Sheet;

class CellIface : virtual public DCOPObject
{
    K_DCOP
public:
    CellIface();

    void setCell( Sheet* sheet, const QPoint& point );
k_dcop:

    virtual bool isDefault() const;
    virtual bool isEmpty() const;
    virtual QString text() const;
    virtual void setText( const QString& text );
    virtual void setValue( int value );
    virtual void setValue( double value );
    virtual double value() const;
    virtual QString visibleContentAsString() const;
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
    virtual void setFormatType(const QString &formatType);
    virtual QString getFormatType() const;
    virtual void setPrecision(int  _p);
    virtual int precision() const;
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
    virtual void setIndent(double indent);
    virtual double getIndent() const ;
    //don't print text
    virtual void setDontPrintText ( bool _b);
    virtual bool getDontprintText() const ;

    virtual bool hasValidation() const;
    virtual void removeValidity();
    virtual QString validationTitle() const;
    virtual QString validationMessage() const;
    virtual bool displayValidationMessage() const;
    virtual bool validationAllowEmptyCell() const;
    virtual bool displayValidationInformation() const;
    virtual QStringList listValidation() const;

    virtual QString validationTitleInfo() const;
    virtual QString validationMessageInfo() const;

private:
    QPoint m_point;
    Sheet* m_sheet;
};

} // namespace KSpread

#endif
