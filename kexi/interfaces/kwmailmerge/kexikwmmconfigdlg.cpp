/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include <klocale.h>
#include <kdebug.h>

#include <kexidatasourcecombobox.h>
#include <kexiproject.h>
#include <kexiprojecthandler.h>
#include <kexiprojecthandlerproxy.h>
#include <kexidataprovider.h>

#include "kexiview.h"
#include "kexikwmmconnection.h"
#include "kexikwmmconfigdlg.h"

KexiKWMMConfigDlg::KexiKWMMConfigDlg(QWidget *parent, KexiKWMMConnection *c)
 :
KDialogBase( Plain, i18n( "Mail Merge - Editor" ), Ok | Cancel, Ok, parent, "kexikwmmconfig", true )
{
//	c->project()->initDBConnection(c->project()->dbConnection());
	QFrame *frame=plainPage();

	resize(400, 300);
	m_connection = c;
	QLabel *lDs = new QLabel(i18n("Datasource:"), frame);
	m_dataSource = new KexiDataSourceComboBox(frame, "", c->project());
	connect(m_dataSource, SIGNAL(activated(int)), this, SLOT(slotSourceChanged(int)));
	KexiDataSourceComboBox::ItemList i;
	m_dataSource->fillList(c->project(), i);
	m_view = new KexiView(KexiView::EmbeddedMode, c->project(), frame);
	m_view->resize(400, 300);


	QGridLayout *g = new QGridLayout(frame);
	g->addWidget(lDs,			0, 0);
	g->addMultiCellWidget(m_dataSource,	0, 0, 1, 2);
	g->addMultiCellWidget(m_view,		1, 1, 0, 2);
	lDs->show();
	m_dataSource->show();
	m_view->show();
}

void
KexiKWMMConfigDlg::slotSourceChanged(int)
{
	KexiProjectHandler *tables = m_connection->project()->handlerForMime(m_dataSource->mime());
	tables->hookIntoView(m_view);
	tables->proxy(m_view)->executeItem(m_dataSource->identifier());
}

QString
KexiKWMMConfigDlg::mime() const
{
	return m_dataSource->mime();
}

QString
KexiKWMMConfigDlg::id() const
{
	return m_dataSource->identifier();
}

DbRecord
KexiKWMMConfigDlg::fields()
{
	DbRecord rec;

	KexiProjectHandler *h = m_connection->project()->handlerForMime(mime());
	KexiDataProvider *prov=KEXIDATAPROVIDER(h);

	if(!prov)
		return rec;

	QStringList fl = prov->fields(0, id());

	for(QStringList::Iterator it=fl.begin(); it != fl.end(); ++it)
	{
		rec.insert((*it), "");
	}

	return rec;
}

KexiKWMMConfigDlg::~KexiKWMMConfigDlg()
{
}

#include "kexikwmmconfigdlg.moc"
