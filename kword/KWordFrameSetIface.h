/* This file is part of the KDE project
   Copyright (C) 2002, Laurent MONTEL <lmontel@mandrakesoft.com>

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

#ifndef KWORD_FRAMESET_IFACE_H
#define KWORD_FRAMESET_IFACE_H

#include <KoDocumentIface.h>
#include <dcopref.h>

#include <qstring.h>
#include <qcolor.h>
#include <qbrush.h>
class KWFrameSet;

class KWordFrameSetIface : public DCOPObject
{
    K_DCOP
public:
    KWordFrameSetIface( KWFrameSet *_frame );

k_dcop:
    virtual bool isAHeader() const;
    virtual bool isAFooter() const;
    virtual bool isHeaderOrFooter() const;
    virtual bool isFootEndNote() const;

    virtual bool isMainFrameset() const;
    virtual bool isMoveable() const;
    virtual bool isVisible() const;
    virtual bool isFloating() const;
    //use frame(0)
    virtual double ptWidth() const;
    virtual double ptHeight() const;
    virtual double ptPosX() const;
    virtual double ptPosY() const;
    virtual int zOrder() const;
    virtual int pageNumber() const;
    virtual QBrush backgroundColor() const;
    virtual void setBackgroundColor( const QString &_color );

    virtual double ptMarginLeft()const;
    virtual double ptMarginRight()const;
    virtual double ptMarginTop()const;
    virtual double ptMarginBottom()const;

    virtual void setPtMarginLeft(double val);
    virtual void setPtMarginRight(double val);
    virtual void setPtMarginTop(double val);
    virtual void setPtMarginBottom(double val);

    virtual QColor leftBorderColor() const;
    virtual QColor rightBorderColor() const;
    virtual QColor topBorderColor() const;
    virtual QColor bottomBorderColor() const;

    virtual bool isCopy()const;
    virtual bool isProtectSize()const;
    virtual void setProtectSize( bool _b );
    //it's for frame 0
    QString bottomBorderStyle()const;
    QString topBorderStyle()const;
    QString leftBorderStyle()const;
    QString rightBorderStyle()const;

    double rightBorderWidth() const;
    double topBorderWidth() const;
    double bottomBorderWidth() const;
    double leftBorderWidth() const;

    void setBottomBorderWitdh( double _width );
    void setTopBorderWitdh( double _width );
    void setLeftBorderWitdh( double _width );
    void setRightBorderWitdh( double _width );

    void setBottomBorderStyle(const QString & _style);
    void setTopBorderStyle(const QString & _style);
    void setLeftBorderStyle(const QString & _style);
    void setRightBorderStyle(const QString & _style);

    QString frameSetInfo() const;
    void setFrameSetInfo( const QString & _type);


private:
    KWFrameSet *m_frame;
};

#endif
