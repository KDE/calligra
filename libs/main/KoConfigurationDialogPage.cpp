
/* This file is part of the KDE project
   Copyright (C) 2011 Shantanu Tushar <jhahoneyk@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/


#include "KoConfigurationDialogPage.h"

KoConfigurationDialogPage::KoConfigurationDialogPage(QWidget* parent)
        : QWidget(parent)
{

}


KoConfigurationDialogPage::~KoConfigurationDialogPage()
{

}

QString KoConfigurationDialogPage::title() const
{
    return m_title;
}

void KoConfigurationDialogPage::setTitle(const QString& title)
{
    m_title = title;
}

KIcon KoConfigurationDialogPage::icon() const
{
    return m_icon;
}

void KoConfigurationDialogPage::setIcon(const KIcon& icon)
{
    m_icon = icon;
}
