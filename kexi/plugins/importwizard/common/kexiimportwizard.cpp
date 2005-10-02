/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "kexiimportwizard.h"
#include "kexiimportwizard.moc"
#include "kexiimportwizardform.h"
#include <qlayout.h>

class KexiImportWizardPrivate
{
public:
	KexiImportWizardPrivate():widget(0){}
	~KexiImportWizardPrivate(){delete widget;}
	KexiTableImportForm *widget;
};

KexiImportWizard::KexiImportWizard(QObject *parent,
	const char *name,const QStringList &list):KexiFilterWizardBase(KEXIFILTERMANAGER(parent),name,list),d(0) {
	d=new KexiImportWizardPrivate;
}

KexiImportWizard::~KexiImportWizard() {
	delete d;
}

void KexiImportWizard::exec(const QString& dialogfilter, const QMap<QString,QString> mimePluginMap, unsigned long importTypes, bool modal){
	d->widget=new KexiTableImportForm(KEXIFILTERMANAGER(parent()),this,dialogfilter,mimePluginMap);
	d->widget->exec();
	delete d->widget;
	d->widget=0;
	deleteLater();
}

void KexiImportWizard::setMode(unsigned long mode) {
	d->widget->setMode(mode);
}
