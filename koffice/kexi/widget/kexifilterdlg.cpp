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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qheader.h>
#include <qstringlist.h>

#include "kexiproject.h"
#include "kexiprojecthandler.h"
#include "kexiprojecthandleritem.h"
#include "kexidataprovider.h"
#include "kexifilterdlg.h"
#include "kexiquerydesignersqleditor.h"

KexiFilterDlg::KexiFilterDlg(KexiProject *project, QWidget *parent, const char *name)
 : QDialog(parent, name)
{
	m_project = project;

	QHBoxLayout *lbraces = new QHBoxLayout(0, 0, 4);

	QPushButton *bsBO = createMiniButton("[");
	QPushButton *bBO = createMiniButton("(");
	QPushButton *bBC = createMiniButton(")");
	QPushButton *bsBC = createMiniButton("]");
	lbraces->addWidget(bsBO);
	lbraces->addWidget(bBO);
	lbraces->addWidget(bBC);
	lbraces->addWidget(bsBC);

	QHBoxLayout *lcond = new QHBoxLayout(0, 0, 4);
	QPushButton *blt = createMiniButton("<");
	QPushButton *beq = createMiniButton("=");
	QPushButton *bgt = createMiniButton(">");
	QPushButton *bp = createMiniButton("%");
	lcond->addWidget(blt);
	lcond->addWidget(beq);
	lcond->addWidget(bgt);
	lcond->addWidget(bp);

	QHBoxLayout *lbool = new QHBoxLayout(0, 0, 4);
	QPushButton *bAnd = new QPushButton("AND", this);
	bAnd->setFlat(true);
	QPushButton *bOr = new QPushButton("OR", this);
	bOr->setFlat(true);
	QPushButton *bLike = new QPushButton("LIKE", this);
	bLike->setFlat(true);
	lbool->addWidget(bLike);
	lbool->addWidget(bAnd);
	lbool->addWidget(bOr);

	m_catalog = new QListView(this);
	m_catalog->addColumn("a");
	m_catalog->header()->hide();

	KexiQueryDesignerSQLEditor *e = new KexiQueryDesignerSQLEditor(this);

	setupCatalog(QString("kexi/table"));

	QGridLayout *g = new QGridLayout(this);
	g->setSpacing(6);
	g->addMultiCellWidget(e,		0, 0, 0, 2);
	g->addItem(lbraces,			1, 0);
	g->addItem(lcond,			1, 1);
	g->addItem(lbool,			1, 2);
	g->addMultiCellWidget(m_catalog,	2, 2, 0, 2);
}

QPushButton*
KexiFilterDlg::createMiniButton(const QString &text)
{
	QPushButton *p = new QPushButton(text, this);
	p->setFlat(true);
	p->setMaximumSize(QSize(20, 300));

	return p;
}

void
KexiFilterDlg::setupCatalog(const QStringList &mimes)
{
	m_catalog->clear();
	m_catalog->setRootIsDecorated(true);
	QStringList::ConstIterator it, end( mimes.constEnd() );
	for( it = mimes.constBegin(); it != end; ++it)
	{
		KexiProjectHandler *h = m_project->handlerForMime(*it);
		if(h)
		{
			QListViewItem *base = new QListViewItem(m_catalog, h->name());
			base->setPixmap(0, h->groupPixmap());

			QDictIterator<KexiProjectHandlerItem> iit(*h->items()); // See QDictIterator
			for(; iit.current(); ++iit )
			{
				QListViewItem *bi = new QListViewItem(base, iit.current()->name());
				bi->setPixmap(0, h->itemPixmap());

				KexiDataProvider *prov=KEXIDATAPROVIDER(h);
				if(prov)
				{
					QStringList fields = prov->fields(0, iit.current()->identifier());
					QStringList::ConstIterator fit, end( fields.constEnd() );
					for( fit = fields.constBegin(); fit != end; ++fit)
					{
						QListViewItem *bif = new QListViewItem(bi, (*fit));
					}
				}
			}
		}
	}
}

void
KexiFilterDlg::setupCatalog(const QString &mime)
{
	QStringList l;
	l.append(mime);
	setupCatalog(l);
}

void
KexiFilterDlg::insert(QListViewItem *)
{
}

KexiFilterDlg::~KexiFilterDlg()
{
}

#include "kexifilterdlg.moc"
