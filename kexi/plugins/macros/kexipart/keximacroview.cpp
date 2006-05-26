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

#include "../lib/macro.h"
#include "../lib/xmlhandler.h"
#include "../lib/exception.h"

#include "keximacroerror.h"

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
	: KexiViewBase(mainwin, parent, (name ? name : "KexiMacroView"))
	, d( new Private(macro) )
{
	//kdDebug() << "KexiMacroView::KexiMacroView() Ctor" << endl;
	plugSharedAction( "data_execute", this, SLOT(execute()) );
}

KexiMacroView::~KexiMacroView()
{
	//kdDebug() << "KexiMacroView::~KexiMacroView() Dtor" << endl;
	delete d;
}

KoMacro::Macro::Ptr KexiMacroView::macro() const
{
	return d->macro;
}

tristate KexiMacroView::beforeSwitchTo(int mode, bool& dontstore)
{
	kexipluginsdbg << "KexiMacroView::beforeSwitchTo mode=" << mode << " dontstore=" << dontstore << endl;
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
	d->macro->clearItems();

	QString data;
	if(! loadDataBlock(data)) {
		kexipluginsdbg << "KexiMacroView::loadData(): no DataBlock" << endl;
		return false;
	}

	QString errmsg;
	int errline, errcol;

	QDomDocument domdoc;
	bool parsed = domdoc.setContent(data, false, &errmsg, &errline, &errcol);

	if(! parsed) {
		kexipluginsdbg << "KexiMacroView::loadData() XML parsing error line: " << errline << " col: " << errcol << " message: " << errmsg << endl;
		return false;
	}

	kexipluginsdbg << QString("KexiMacroView::loadData()\n%1").arg(domdoc.toString()) << endl;
	QDomElement macroelem = domdoc.namedItem("macro").toElement();
	if(macroelem.isNull()) {
		kexipluginsdbg << "KexiMacroView::loadData() Macro domelement is null" << endl;
		return false;
	}

	//kexipluginsdbg << "KexiMacroView::loadData()" << endl;
	return d->macro->parseXML(macroelem);
}

KexiDB::SchemaData* KexiMacroView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
	KexiDB::SchemaData *schema = KexiViewBase::storeNewData(sdata, cancel);
	kexipluginsdbg << "KexiMacroView::storeNewData() new id:" << schema->id() << endl;

	if(!schema || cancel) {
		delete schema;
		return 0;
	}

	if(! storeData()) {
		kexipluginsdbg << "KexiMacroView::storeNewData() Failed to store the data." << endl;
		//failure: remove object's schema data to avoid garbage
		KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
		conn->removeObject( schema->id() );
		delete schema;
		return 0;
	}

	return schema;
}

tristate KexiMacroView::storeData(bool /*dontAsk*/)
{
	QDomDocument domdoc("macros");
	QDomElement macroelem = d->macro->toXML();
	domdoc.appendChild(macroelem);
	const QString xml = domdoc.toString(2);
	const QString name = QString("%1 [%2]").arg(parentDialog()->partItem()->name()).arg(parentDialog()->id());
	kexipluginsdbg << QString("KexiMacroView::storeData %1\n%2").arg(name).arg(xml) << endl;
	return storeDataBlock(xml);
}

void KexiMacroView::execute(QObject* sender)
{
	KoMacro::Context::Ptr context = d->macro->execute(sender);
	if(context->hadException()) {
		
		KexiMacroError* error = new KexiMacroError(
				     mainWin(), //parent
 				     "keximacroerror",
				     0,
				     context 
				    );
		error->show();
	}
}

#include "keximacroview.moc"

