/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

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

#include <qcheckbox.h>
#include <qlayout.h>

#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>

#include "kexisettings.h"
#include "kexi_global.h"

KexiSettings::KexiSettings(QWidget *parent)
 : KDialogBase(IconList, i18n("%1 Settings").arg(KEXI_APP_NAME), KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, parent, 0, true)
{
	addPage(i18n("Printing"), i18n("Default Printer Settings"), ListIcon("print_printer"));
	QFrame *lnf = addPage(i18n("Look & Feel"), i18n("Look & Feel"), ListIcon("looknfeel"));
	QCheckBox *showBGAltering = new QCheckBox(i18n("Use backgroundaltering for tables"), lnf);
	showBGAltering->setChecked(true);
	QGridLayout *gLnf = new QGridLayout(lnf);
	QSpacerItem *vSpace = new QSpacerItem(20, 60);
	gLnf->addWidget(showBGAltering,	0, 0);
	gLnf->addItem(vSpace,		1, 0);
}

QPixmap
KexiSettings::ListIcon(const char *i)
{
	return KGlobal::instance()->iconLoader()->loadIcon(i, KIcon::NoGroup, KIcon::SizeMedium);
}

KexiSettings::~KexiSettings()
{
}


#include "kexisettings.moc"

