/* This file is part of the KDE project
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

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

#include <pagelayoutdia_impl.h>

#include <qcombobox.h>
#include <kdebug.h>

PageLayoutDiaImpl::PageLayoutDiaImpl(Graphite::PageLayout &layout, QWidget *parent,
                  const char *name, bool modal, WFlags fl) : PageLayoutDia(parent, name, modal, fl),
    m_layout(layout) {

    // Initialize all that stuff
    unit->setCurrentItem(static_cast<int>(GraphiteGlobal::self()->unit()));
    connect(unit, SIGNAL(activated(int)), this, SLOT(unitChanged(int)));

    if(m_layout.layout==Graphite::PageLayout::Custom)
        format->setCurrentItem(0);
    else
        format->setCurrentItem(static_cast<int>(m_layout.size)+1);

}

void PageLayoutDiaImpl::pageLayoutDia(Graphite::PageLayout &layout, QWidget *parent) {

    Graphite::PageLayout tmp=layout;  // store in case of a Cancel operation
    PageLayoutDiaImpl dia(layout, parent, "pagelayoutdiaimpl", true);
    if(dia.exec()==QDialog::Accepted)
        GraphiteGlobal::self()->setUnit(static_cast<GraphiteGlobal::Unit>(dia.unit->currentItem()));
    else
        layout=tmp;  // The user cancelled -> restore the original state
}

void PageLayoutDiaImpl::unitChanged(int unit) {
    kdDebug() << "PageLayoutDiaImpl::unitChanged: " << unit << endl;
}

#include <pagelayoutdia_impl.moc>
