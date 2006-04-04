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

#include "keximacrotextview.h"

//#include <qlayout.h>
//#include <qsplitter.h>
//#include <qtimer.h>
//#include <qdatetime.h>
#include <qdom.h>
//#include <qstylesheet.h>
#include <ktextedit.h>
#include <kdebug.h>

#include <kexidialogbase.h>
#include <kexidb/connection.h>

#include "lib/macro.h"

/**
* \internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroTextView::Private
{
	public:

		/**
		* The \a KoMacro::Manager instance used to access the
		* Macro Framework.
		*/
		::KoMacro::Macro::Ptr const macro;

		/**
		* The Editor used to display and edit the XML text.
		*/
		KTextEdit* editor;

		/**
		* Constructor.
		*
		* \param macro The \a KoMacro::Macro instance.
		*/
		explicit Private(::KoMacro::Macro* const m)
			: macro(m)
		{
		}

};

KexiMacroTextView::KexiMacroTextView(KexiMainWindow *mainwin, QWidget *parent, ::KoMacro::Macro* const macro)
	: KexiViewBase(mainwin, parent, "KexiMacroTextView")
	, d( new Private(macro) )
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	d->editor = new KTextEdit(this);
	layout->addWidget(d->editor);

	loadData();

	connect(d->editor, SIGNAL(textChanged()), this, SLOT(editorChanged()));
}

KexiMacroTextView::~KexiMacroTextView()
{
	delete d;
}

tristate KexiMacroTextView::beforeSwitchTo(int mode, bool& dontstore)
{
	kexipluginsdbg << "KexiMacroTextView::beforeSwitchTo mode=" << mode << endl;
	Q_UNUSED(dontstore);
	return true;
}

tristate KexiMacroTextView::afterSwitchFrom(int mode)
{
	kexipluginsdbg << "KexiMacroTextView::afterSwitchFrom mode=" << mode << endl;
	return true;
}

void KexiMacroTextView::editorChanged()
{
	setDirty(true);
}

bool KexiMacroTextView::loadData()
{
	QString data;
	if(! loadDataBlock(data)) {
		kexipluginsdbg << "KexiMacroTextView::loadData(): no DataBlock" << endl;
		return false;
	}

	QString errmsg;
	int errline;
	int errcol;

	QDomDocument domdoc;
	bool parsed = domdoc.setContent(data, false, &errmsg, &errline, &errcol);

	if(! parsed) {
		kexipluginsdbg << "KexiMacroTextView::loadData() XML parsing error line: " << errline << " col: " << errcol << " message: " << errmsg << endl;
		return false;
	}

	QDomElement macroelem = domdoc.namedItem("macro").toElement();
	if(macroelem.isNull()) {
		kexipluginsdbg << "KexiMacroTextView::loadData(): macro domelement is null" << endl;
		return false;
	}

	//QString s = macroelem.attribute("myattr");

	d->editor->setText( macroelem.text() );

	return true;
}

KexiDB::SchemaData* KexiMacroTextView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
	KexiDB::SchemaData *s = KexiViewBase::storeNewData(sdata, cancel);
	kexipluginsdbg << "KexiMacroTextView::storeNewData(): new id:" << s->id() << endl;

	if(!s || cancel) {
		delete s;
		return 0;
	}

	if(! storeData()) {
		kdWarning() << "KexiMacroTextView::storeNewData Failed to store the data." << endl;
		//failure: remove object's schema data to avoid garbage
		KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
		conn->removeObject( s->id() );
		delete s;
		return 0;
	}

	return s;
}

tristate KexiMacroTextView::storeData(bool /*dontAsk*/)
{
	kexipluginsdbg << "KexiMacroDesignView::storeData(): " << parentDialog()->partItem()->name() << " [" << parentDialog()->id() << "]" << endl;

	QDomDocument domdoc("macro");
	QDomElement macroelem = domdoc.createElement("macro");
	domdoc.appendChild(macroelem);

	//macroelem.setAttribute("myattr", s);

	QDomText elemtext = domdoc.createTextNode( d->editor->text() );
	macroelem.appendChild(elemtext);

	return storeDataBlock( domdoc.toString() );
}

#include "keximacrotextview.moc"

