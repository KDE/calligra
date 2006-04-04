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

#include "keximacrodesignview.h"

//#include <qlayout.h>
//#include <qsplitter.h>
//#include <qtimer.h>
//#include <qdatetime.h>
#include <qdom.h>
//#include <qstylesheet.h>
//#include <ktextbrowser.h>
#include <kdebug.h>

#include <kexidialogbase.h>
#include <kexidb/connection.h>

#include "lib/macro.h"

/**
* \internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroDesignView::Private
{
	public:

		/**
		* The \a KoMacro::Manager instance used to access the
		* Macro Framework.
		*/
		::KoMacro::Macro::Ptr const macro;

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

KexiMacroDesignView::KexiMacroDesignView(KexiMainWindow *mainwin, QWidget *parent, ::KoMacro::Macro* const macro)
	: KexiViewBase(mainwin, parent, "KexiMacroDesignView")
	, d( new Private(macro) )
{
	loadData();
}

KexiMacroDesignView::~KexiMacroDesignView()
{
	delete d;
}

tristate KexiMacroDesignView::beforeSwitchTo(int mode, bool& dontstore)
{
	kexipluginsdbg << "KexiMacroDesignView::beforeSwitchTo mode=" << mode << endl;
	Q_UNUSED(dontstore);
	return true;
}

tristate KexiMacroDesignView::afterSwitchFrom(int mode)
{
	kexipluginsdbg << "KexiMacroDesignView::afterSwitchFrom mode=" << mode << endl;
	return true;
}

bool KexiMacroDesignView::loadData()
{
	QString data;
	if(! loadDataBlock(data)) {
		kexipluginsdbg << "KexiMacroDesignView::loadData(): no DataBlock" << endl;
		return false;
	}

	QString errmsg;
	int errline;
	int errcol;

	QDomDocument domdoc;
	bool parsed = domdoc.setContent(data, false, &errmsg, &errline, &errcol);

	if(! parsed) {
		kexipluginsdbg << "KexiMacroDesignView::loadData() XML parsing error line: " << errline << " col: " << errcol << " message: " << errmsg << endl;
		return false;
	}

	/*
	QDomElement scriptelem = domdoc.namedItem("script").toElement();
	if(scriptelem.isNull()) {
		kexipluginsdbg << "KexiMacroDesignView::loadData(): script domelement is null" << endl;
		return false;
	}
	QString interpretername = scriptelem.attribute("language");
	*/

	return true;
}

KexiDB::SchemaData* KexiMacroDesignView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
	KexiDB::SchemaData *s = KexiViewBase::storeNewData(sdata, cancel);
	kexipluginsdbg << "KexiMacroDesignView::storeNewData(): new id:" << s->id() << endl;

	if(!s || cancel) {
		delete s;
		return 0;
	}

	if(! storeData()) {
		kdWarning() << "KexiMacroDesignView::storeNewData Failed to store the data." << endl;
		//failure: remove object's schema data to avoid garbage
		KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
		conn->removeObject( s->id() );
		delete s;
		return 0;
	}

	return s;
}

tristate KexiMacroDesignView::storeData(bool /*dontAsk*/)
{
	kexipluginsdbg << "KexiMacroDesignView::storeData(): " << parentDialog()->partItem()->name() << " [" << parentDialog()->id() << "]" << endl;

	QDomDocument domdoc("macro");
	/*
	QDomElement scriptelem = domdoc.createElement("script");
	domdoc.appendChild(scriptelem);
	scriptelem.setAttribute("language", language);
	scriptelem.setAttribute(it.key(), it.data().toString());
	QDomText scriptcode = domdoc.createTextNode(d->scriptaction->getCode());
	scriptelem.appendChild(scriptcode);
	*/

	return storeDataBlock( domdoc.toString() );
}

#include "keximacrodesignview.moc"

