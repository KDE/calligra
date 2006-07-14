/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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

#include "KivioPage.h"

#include <KoPageLayout.h>

#include "KivioMasterPage.h"

KivioPage::KivioPage(KivioMasterPage* master, const QString& title)
  : KivioAbstractPage(master->document(), title), m_masterPage(master)
{
}

KivioPage::~KivioPage()
{
}

void KivioPage::setMasterPage(KivioMasterPage* master)
{
  if(master == 0) return;

  m_masterPage = master;
}

KivioMasterPage* KivioPage::masterPage() const
{
  return m_masterPage;
}

KoPageLayout KivioPage::pageLayout() const
{
  if(m_masterPage) {
    return m_masterPage->pageLayout();
  } else {
    return KoPageLayout::standardLayout();
  }
}
