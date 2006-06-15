// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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

#ifndef KPRESENTER_TEXTOBJECT_IFACE_H
#define KPRESENTER_TEXTOBJECT_IFACE_H

#include <KoDocumentAdaptor.h>
#include "KPrObject2DAdaptor.h"
#include <QString>
#include <QColor>
#include <QFont>
class KPrTextObject;

class KPrTextObjectAdaptor : public KPrObject2DAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.presentation.object.text")

public:
    KPrTextObjectAdaptor( KPrTextObject *_textobject );

public Q_SLOTS:
    QString startEditing();
    bool hasSelection() const;
    QString selectedText() const;
    void selectAll( bool select );
    void recalcPageNum( );

    void setBoldText( bool b );
    void setItalicText( bool b );
    void setUnderlineText( bool b );
    void setStrikeOutText( bool b );
    void setTextColor( const QColor &col );
    void setTextPointSize( int s );
    void setTextSubScript( bool b );
    void setTextSuperScript( bool b );
    void setTextDefaultFormat();
    void setTextBackgroundColor(const QColor & col);
    void setTextFamilyFont(const QString &font);
    void changeCaseOfText( const QString & caseType);
    void setDoubleUnderlineText(bool on);
    void setUnderlineColor( const QColor & color );

    QColor textColor() const;
    QFont textFont() const;
    QString textFontFamily()const;
    QColor textBackgroundColor()const;
    QColor textUnderlineColor() const;

    bool textItalic() const;
    bool textBold() const;
    bool textUnderline()const;
    bool textStrikeOut()const;
    bool textSubScript() const;
    bool textSuperScript() const;

    void extendTextContentsToHeight();
    void extendTextObjectToContents();
    void setProtectContent ( bool _protect );
    bool isProtectContent() const;
    bool textDoubleUnderline()const;

    double ptMarginLeft()const;
    double ptMarginRight()const;
    double ptMarginTop()const;
    double ptMarginBottom()const;

    void setPtMarginLeft(double val);
    void setPtMarginRight(double val);
    void setPtMarginTop(double val);
    void setPtMarginBottom(double val);

    void setVerticalAligment( const QString & type);
    QString verticalAlignment() const;

    //bool textShadow() const;
    //void setTextShadow( bool b );

    double relativeTextSize() const;
    void setRelativeTextSize( double _size );

    bool wordByWord() const;
    void setWordByWord( bool _b );

    QString fontAttribute()const;
    QString underlineLineStyle() const;
    QString strikeOutLineStyle()const;


private:
    KPrTextObject *m_textobject;
};

#endif
