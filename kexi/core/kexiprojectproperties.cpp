/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2002 Daniel Molkentin <molkentin@kde.org>

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

#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>

#include <klocale.h>
#include <kiconloader.h>

#include "kexidbconnection.h"
#include "kexiprojectproperties.h"

KexiProjectProperties::KexiProjectProperties(QWidget *parent, KexiDBConnection *dbconn)
 : KDialogBase(IconList, i18n("Projects Properties"), KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, parent, 0, true)
{
	QPixmap pixdb = KGlobal::instance()->iconLoader()->loadIcon("db", KIcon::NoGroup, KIcon::SizeMedium );
	QFrame *dbFrame = addPage(i18n("Database"), i18n("Database Settings"), pixdb);

	QLabel *lEnc = new QLabel(i18n("Encoding:"), dbFrame);
	m_encoding = new QComboBox(dbFrame);
	m_encoding->insertItem( i18n("Latin 1") );
	m_encoding->insertItem( i18n("UTF-8 (8-Byte Unicode)") );
	m_encoding->insertItem( i18n("Locale-specific 8 Bit") );
	m_encoding->insertItem( i18n("ASCII") );

//TODO(js)	KexiFactory::global()->config()->setGroup("....");
//	KexiFactory::global()->config()->readEntry(  )
	
	QSpacerItem *vSpace = new QSpacerItem(20, 60);
	QGridLayout *g = new QGridLayout(dbFrame);
	g->addWidget(lEnc,	0, 0);
	g->addWidget(m_encoding,	0, 1);
	g->addMultiCell(vSpace,	1, 1, 0, 1);

	setupDBProperties(dbconn);
}

KexiDB::Encoding
KexiProjectProperties::encoding()
{
	switch(m_encoding->currentItem())
	{
		case 0:
			return KexiDB::Latin1;

		case 1:
			return KexiDB::Utf8;

		case 2:
			return KexiDB::Local8Bit;

		case 3:
			return KexiDB::Ascii;
	}
	qFatal("KexiProjectProperties::encoding()");
	return KexiDB::Latin1;
}

void
KexiProjectProperties::setupDBProperties(KexiDBConnection *db)
{
	switch(db->encoding())
	{
		case KexiDB::Utf8:
			m_encoding->setCurrentItem(1);
			break;

		case KexiDB::Local8Bit:
			m_encoding->setCurrentItem(2);
			break;

		case KexiDB::Ascii:
			m_encoding->setCurrentItem(3);
			break;

		default:
			m_encoding->setCurrentItem(0);
			break;
	}
}

KexiProjectProperties::~KexiProjectProperties()
{
}

#include "kexiprojectproperties.moc"
