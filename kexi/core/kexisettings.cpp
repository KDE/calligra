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
#include <qcombobox.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <kdeversion.h>

#include <kexiview.h>
#include "kexisettings.h"
#include "kexi_global.h"

KexiSettings::KexiSettings(QWidget *parent)
 : KDialogBase(IconList, i18n("%1 Settings").arg(KEXI_APP_NAME), KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, parent, 0, true)
{
	addPage(i18n("Printing"), i18n("Default Printer Settings"), ListIcon("print_printer"));
	QFrame *lnf = addPage(i18n("Look & Feel"), i18n("Look & Feel"), ListIcon("looknfeel"));
	QCheckBox *showBGAltering = new QCheckBox(i18n("Use background altering for tables"), lnf);
	showBGAltering->setChecked(true);
	QGridLayout *gLnf = new QGridLayout(lnf);
	QLabel *viewModeLabel=new QLabel(i18n("Document view mode"),lnf);

	QComboBox *viewModeCombo=new QComboBox(lnf);
	viewModeCombo->insertStringList(KexiView::possibleViewModes());

	QSpacerItem *vSpace = new QSpacerItem(20, 60);
	gLnf->addWidget(showBGAltering,	0, 0);
	gLnf->addWidget(viewModeLabel,	1, 0);
	gLnf->addWidget(viewModeCombo,	1, 1);
	gLnf->addItem(vSpace,		2, 0);

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

