/* This file is part of the KDE project
   Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXILINKBUTTON_H
#define KEXILINKBUTTON_H

#include "kexiutils_export.h"

#include <KPushButton>

//! A flat icon-based button without background that behaves like a link
class KEXIUTILS_EXPORT KexiLinkButton : public KPushButton
{
    Q_OBJECT
public:
    explicit KexiLinkButton(QWidget* parent = 0);

    explicit KexiLinkButton(const KIcon &icon, QWidget* parent = 0);

    explicit KexiLinkButton(const KGuiItem &item, QWidget *parent = 0);

    explicit KexiLinkButton(const QPixmap &pixmap, QWidget* parent = 0);

    virtual ~KexiLinkButton();
    
    /*! If true, foreground color of the current palette is always used for painting
        the button's icon. This is done by replacing color.
        The foreground color is QPalette::Text by default, and can be changed 
        using setForegroundRole().
        The icon is expected to be monochrome.
        Works well also after palette change.
        False by default. */
    void setUsesForegroundColor(bool set);

    /*! @return true if foreground color of the current palette is always used for painting
        the button's icon. */
    bool usesForegroundColor() const;

    void setIcon(const KIcon &icon);
    
protected:
    virtual void changeEvent(QEvent* event);

private:
    void init();
    void updateIcon(const KIcon &icon);

    class Private;
    Private * const d;
};

#endif
