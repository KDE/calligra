/* This file is part of the KDE project
   Copyright (C) 2002   Joseph Wenninger <jowenn@kde.org>

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

#include <klocale.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <qvbox.h>
#include <kexidataprovider.h>
#include "kexiaddparamdialog.h"
#include "kexiaddparamdialog.moc"
#include "kexiaddparamwidget.h"

KexiAddParamDialog::KexiAddParamDialog(QWidget *parent)
 : KDialogBase(parent, "kexiaddparamdialog", true, i18n("Add Parameter"), KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true)
{
	m_wid=new KexiAddParamWidget(makeVBoxMainWidget());
	for (int i=1;i<=KexiDataProvider::Parameter::maxType;i++)
		m_wid->typecombo->insertItem(KexiDataProvider::Parameter::typeDescription[i]);
}

KexiAddParamDialog::~KexiAddParamDialog()
{
}

QString KexiAddParamDialog::parameterName() {
	return m_wid->paramname->text();
}

int KexiAddParamDialog::parameterType() {
	return m_wid->typecombo->currentItem()+1;
}


#include "kexiprojectproperties.moc"
