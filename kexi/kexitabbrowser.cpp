/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include <kiconloader.h>

#include <qtabwidget.h>

#include "kexiapplication.h"
#include "kexibrowser.h"
#include "kexitabbrowser.h"

KexiTabBrowser::KexiTabBrowser(QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	QTabWidget *w = new QTabWidget(this);
	KexiBrowser *b = new KexiBrowser(this, KexiBrowser::SectionDB);
	w->addTab(b, kexi->iconLoader()->loadIcon("database", KIcon::Small), "");
}

KexiTabBrowser::~KexiTabBrowser()
{
}

#include "kexitabbrowser.moc"
