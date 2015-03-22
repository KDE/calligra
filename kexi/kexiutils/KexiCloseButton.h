/* This file is part of the KDE project
   Copyright (C) 2014 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXICLOSEBUTTON_H
#define KEXICLOSEBUTTON_H

#include "kexiutils_export.h"

#include <QToolButton>

//! A flat button with a "close" icon, looking exactly like a "close" button of
//! a dock widget or a tab bar's tab.
class KEXIUTILS_EXPORT KexiCloseButton : public QToolButton
{
    Q_OBJECT
public:
    explicit KexiCloseButton(QWidget* parent = 0);

    virtual ~KexiCloseButton();

    //! Sets default margin (based on style), on by default.
    void setMarginEnabled(bool set);

protected:
    virtual void paintEvent(QPaintEvent *e);

private:
    void init();

    class Private;
    Private * const d;
};

#endif
