/* This file is part of the KDE project
   Copyright (C) 2003   Joseph Wenninger <jowenn@kde.org>

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

#include <klocale.h>
#include <kgenericfactory.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <ktempfile.h>

#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbtable.h"
#include "kexiDB/kexidbfield.h"
#include "kexiDB/kexidbrecordset.h"

#include "kexikspreadimport.h"
#include "kexikspreadsource.h"

#include "core/filters/kexifiltermanager.h"
#include "core/filters/kexifilterwizardbase.h"
#include "core/kexiproject.h"


KexiKSpreadImport::KexiKSpreadImport(QObject *parent, const char *name, const QStringList &l)
 : KexiFilter(KEXIFILTERWIZARDBASE(parent), name,l)
{
	m_srcWidget=0;
	m_file="";
}

QString
KexiKSpreadImport::name()
{
	return i18n("kspread file");
}


bool KexiKSpreadImport::prepareImport(unsigned long type, const KURL& url)
{

	bool result=false;
	if(  KIO::NetAccess::download( url, m_file) ) {
		result=true;
	} else {
#warning display error
		m_file="";
	}

	return result;
}


KexiDBTable KexiKSpreadImport::tableStructure() {
	return m_srcWidget->tableStructure();
}

bool KexiKSpreadImport::firstTableRow() {
	return m_srcWidget->firstTableRow();
}


bool KexiKSpreadImport::nextTableRow() {
	return m_srcWidget->nextTableRow();
}

QVariant KexiKSpreadImport::tableValue(int field) {
	return m_srcWidget->tableValue(field);
}

QPtrList<QWidget> KexiKSpreadImport::tableSourceWidgets(QWidget *parent) {
	filterWizard()->setMode(KexiFilterManager::Data);
	m_srcWidget=new KexiKSpreadSource(parent);
	m_srcWidget->setFile(m_file);
	QPtrList<QWidget> tmp;
	tmp.append(m_srcWidget);
	return tmp;
}



KexiKSpreadImport::~KexiKSpreadImport()
{
	delete m_srcWidget;
	if (m_file.isEmpty()) {
		KIO::NetAccess::removeTempFile(m_file);
	}
}

K_EXPORT_COMPONENT_FACTORY(kexikspreadimport, KGenericFactory<KexiKSpreadImport>)

#include "kexikspreadimport.moc"
