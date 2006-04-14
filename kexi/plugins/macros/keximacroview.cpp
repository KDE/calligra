/* This file is part of the KDE project
   Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "keximacroview.h"

#include <qdom.h>
#include <kdebug.h>

#include <kexidialogbase.h>
#include <kexidb/connection.h>
#include <kexidb/error.h>

#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipartmanager.h>
#include <core/kexipartinfo.h>

#include "lib/macro.h"
#include "lib/xmlhandler.h"

/**
* \internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroView::Private
{
	public:

		/**
		* The \a KoMacro::Manager instance used to access the
		* Macro Framework.
		*/
		::KoMacro::Macro::Ptr macro;

		/**
		* Constructor.
		*
		* \param m The passed \a KoMacro::Manager instance our
		*        \a manager points to.
		*/
		Private(::KoMacro::Macro* const m)
			: macro(m)
		{
		}

};

KexiMacroView::KexiMacroView(KexiMainWindow *mainwin, QWidget *parent, ::KoMacro::Macro* const macro, const char* name)
	: KexiViewBase(mainwin, parent, name)
	, d( new Private(macro) )
{
	plugSharedAction( "data_execute", this, SLOT(execute()) );
}

KexiMacroView::~KexiMacroView()
{
	delete d;
}

::KoMacro::Macro* KexiMacroView::macro() const
{
	return d->macro.data();
}

tristate KexiMacroView::beforeSwitchTo(int mode, bool& dontstore)
{
	kexipluginsdbg << "KexiMacroView::beforeSwitchTo mode=" << mode << endl;
	Q_UNUSED(dontstore);
	return true;
}

tristate KexiMacroView::afterSwitchFrom(int mode)
{
	kexipluginsdbg << "KexiMacroView::afterSwitchFrom mode=" << mode << endl;
	loadData(); // reload the data
	return true;
}

bool KexiMacroView::loadData()
{
	QString data;
	if(! loadDataBlock(data)) {
		kexipluginsdbg << "KexiMacroView::loadData(): no DataBlock" << endl;
		return false;
	}

	QString errmsg;
	int errline;
	int errcol;

	QDomDocument domdoc;
	bool parsed = domdoc.setContent(data, false, &errmsg, &errline, &errcol);

	if(! parsed) {
		kexipluginsdbg << "KexiMacroView::loadData() XML parsing error line: " << errline << " col: " << errcol << " message: " << errmsg << endl;
		return false;
	}

	kdDebug() << QString("KexiMacroView::loadData()\n%1").arg(domdoc.toString()) << endl;
	QDomElement macroelem = domdoc.namedItem("macro").toElement();
	if(macroelem.isNull()) {
		kexipluginsdbg << "KexiMacroView::loadData(): macro domelement is null" << endl;
		return false;
	}
		
	return d->macro->xmlHandler()->fromXML(macroelem);
}

KexiDB::SchemaData* KexiMacroView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
	KexiDB::SchemaData *s = KexiViewBase::storeNewData(sdata, cancel);
	kexipluginsdbg << "KexiMacroView::storeNewData(): new id:" << s->id() << endl;

	if(!s || cancel) {
		delete s;
		return 0;
	}

	if(! storeData()) {
		kdWarning() << "KexiMacroView::storeNewData Failed to store the data." << endl;
		//failure: remove object's schema data to avoid garbage
		KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
		conn->removeObject( s->id() );
		delete s;
		return 0;
	}

	return s;
}

tristate KexiMacroView::storeData(bool /*dontAsk*/)
{
	kexipluginsdbg << "KexiMacroView::storeData(): " << parentDialog()->partItem()->name() << " [" << parentDialog()->id() << "]" << endl;

	/*
	QDomElement macroelem = domdoc.createElement("macro");
	domdoc.appendChild(macroelem);
	for(KexiTableViewData::Iterator it = d->tabledata->iterator(); it.current(); ++it) {
		KexiTableItem* item = it.current();
		if(! item->at(0).isNull()) {
			bool ok;
			int actionindex = item->at(0).toInt(&ok);
			if(ok) {
				QString s = QString("action%1").arg(actionindex); //TODO
				QDomElement elem = domdoc.createElement(s);
				macroelem.appendChild(elem);
			}
		}
	}
	QString xml = domdoc.toString();
	*/

	QDomDocument domdoc("macros");
	QDomElement macroelem = d->macro->xmlHandler()->toXML();
	domdoc.appendChild(macroelem);
	QString xml = domdoc.toString();
	kdDebug() << QString("KexiMacroView::storeData\n%1").arg(xml) << endl;
	return storeDataBlock(xml);
}

void KexiMacroView::execute()
{
	kdDebug() << "KexiMacroView::execute" << endl;
	d->macro->activate();
}

#include "keximacroview.moc"

