/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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


#include "kexidynamicqueryparameterdialog.h"
#include "kexidynamicqueryparameterdialog.moc"

#include <q3vbox.h>
#include <klocale.h>
#include <kdebug.h>
#include <qlineedit.h>
#include <qobject.h>

KexiDynamicQueryParameterDialog::KexiDynamicQueryParameterDialog(QWidget *parent,
	KexiDataProvider::Parameters *values, const KexiDataProvider::ParameterList &list):
		KDialogBase(parent, "paramddialog", true, i18n("Query Parameters"),
		KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true )
{
	m_values=values;
	int y;
	m_mainView=new Q3VBox(this);

	for (KexiDataProvider::ParameterList::const_iterator it=list.begin();
		it!=list.end();++it) {
		QLineEdit *le=new QLineEdit(m_mainView,(*it).name.toUtf8());
		le->setText((*values)[(*it).name]);
	}

	setMainWidget(m_mainView);
}

KexiDynamicQueryParameterDialog::~KexiDynamicQueryParameterDialog() {}

void KexiDynamicQueryParameterDialog::slotOk() {
	QObjectList *l=queryList(0,"kexi_.*",true,true);
	QObjectListIt it(*l);
	QObject *obj;
	kDebug()<<"KexiDynamicQueryParameterDialog::slotOk()"<<endl;
	while ((obj=it.current())!=0) {
		kDebug()<<"KexiDynamicQueryParameterDialog::slotOk()::loop"<<endl;
		(*m_values)[QString().fromUtf8(obj->name())]=
			(dynamic_cast<QLineEdit*>(obj))->text();
		++it;
	}
	delete l;
	KDialogBase::slotOk();
}
