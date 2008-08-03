/* This file is part of the KDE project
   Copyright (C) 2002   Joseph Wenninger <jowenn@kde.org>

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

#include <klocale.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <q3vbox.h>
#include <kexidataprovider.h>
#include "kexiaddparamdialog.h"
#include "kexiaddparamdialog.moc"
#include "kexiaddparamwidget.h"

KexiAddParamDialog::KexiAddParamDialog(QWidget *parent)
        : KDialog(parent, "kexiaddparamdialog", true, i18n("Add Parameter"), KDialog::Ok | KDialog::Cancel, KDialog::Ok, true)
{
    m_wid = new KexiAddParamWidget(makeVBoxMainWidget());
    for (int i = 1;i <= KexiDataProvider::Parameter::maxType;i++)
        m_wid->typecombo->insertItem(KexiDataProvider::Parameter::typeDescription[i]);
}

KexiAddParamDialog::~KexiAddParamDialog()
{
}

QString KexiAddParamDialog::parameterName()
{
    return m_wid->paramname->text();
}

int KexiAddParamDialog::parameterType()
{
    return m_wid->typecombo->currentItem() + 1;
}
