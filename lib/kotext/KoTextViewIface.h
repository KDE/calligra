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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KOTEXTVIEW_IFACE_H
#define KOTEXTVIEW_IFACE_H

#include <KoDocumentIface.h>
#include <dcopref.h>

#include <qstring.h>
#include <qcolor.h>
class KoTextView;

class KoTextViewIface :  virtual public DCOPObject
{
    K_DCOP
public:
    KoTextViewIface( KoTextView *_textview );

k_dcop:
    void insertSoftHyphen();
    void insertText( const QString &text );
    void setBold(bool b);
    void setItalic(bool on);
    void setUnderline(bool on);
    void setStrikeOut(bool on);
    void setPointSize( int s );
    void setTextSubScript(bool on);
    void setTextSuperScript(bool on);

    void setDefaultFormat();

    QColor textColor() const;
    QString textFontFamily()const;
    QColor textBackgroundColor()const;

    void setTextColor(const QColor &color);
    void setTextBackgroundColor(const QColor &);
    void setAlign(int align);

private:
    KoTextView *m_textView;

};

#endif
