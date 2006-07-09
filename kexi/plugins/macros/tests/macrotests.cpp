/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "macrotests.h"
#include "testobject.h"
#include "testaction.h"
#include "komacrotestbase.h"

#include "../lib/action.h"
#include "../lib/function.h"
#include "../lib/manager.h"
#include "../lib/macro.h"
#include "../lib/variable.h"
#include "../lib/metaobject.h"
#include "../lib/metamethod.h"
#include "../lib/metaparameter.h"
#include "../lib/exception.h"
#include "../lib/macroitem.h"

#include <ostream>

#include <qstringlist.h>
#include <qdom.h>

#include <kdebug.h>
#include <kunittest/runner.h>
#include <kxmlguiclient.h>

using namespace KUnitTest;
using namespace KoMacroTest;

namespace KoMacroTest {

	/**
	* Register KoMacroTest::CommonTests as TestSuite.
	*/
	KUNITTEST_SUITE("KoMacroTestSuite")
	KUNITTEST_REGISTER_TESTER(MacroTests);

	class MacroTests::Private
	{
		public:		
		/**
		* An KXMLGUIClient instance created on @a setUp() and
		* passed to the @a KoMacro::Manager to bridge to the
		* app-functionality.
		*/
		KXMLGUIClient* xmlguiclient;

		/**
		* An @a TestObject instance used internaly to test
		* handling and communication with from QObject
		* inheritated instances.
		*/
		TestObject* testobject;

		TestAction* testaction;

		QDomDocument* doomdocument;

		Private()
			: xmlguiclient(0)
			, testobject(0)
			, testaction(0)
			, doomdocument(0)
		{
		}
	};
}

typedef QValueList< KSharedPtr<KoMacro::MacroItem> >::size_type sizetype;


MacroTests::MacroTests()
	: KUnitTest::SlotTester()
	, d( new Private() ) // create the private d-pointer instance.
{
}

MacroTests::~MacroTests()
{
	delete d->xmlguiclient;
	delete d;
}


void MacroTests::setUp()
{
	d->xmlguiclient = new KXMLGUIClient();
	//::KoMacro::Manager::init( d->xmlguiclient );
	if (::KoMacro::Manager::self() == 0) {	
		::KoMacro::Manager::init( d->xmlguiclient );
	}
	d->testobject = new TestObject( this );
	::KoMacro::Manager::self()->publishObject("TestObject", d->testobject);
	
	d->testaction = new TestAction();
	::KoMacro::Manager::self()->publishAction(d->testaction);	

	d->doomdocument = new QDomDocument();

	QString const xml = QString("<!DOCTYPE macros>"
				   "<macro xmlversion=\"1\" >"
				      "<item action=\"testaction\" >"
				      "</item>"
				    "</macro>");
	
	d->doomdocument->setContent(xml);
}

void MacroTests::tearDown()
{
	delete d->doomdocument;
	delete d->testobject;
	delete d->xmlguiclient;
}

void MacroTests::testMacro()
{
	kdDebug()<<"===================== testMacro() ======================" << endl;

	QDomElement const domelement = d->doomdocument->documentElement();
	
	KSharedPtr<KoMacro::Macro> macro1 = KoMacro::Manager::self()->createMacro("testMacro");
	KSharedPtr<KoMacro::Macro> macro2 = KoMacro::Manager::self()->createMacro("testMacro");
	//Is our XML parseable ?
	KOMACROTEST_ASSERT(macro1->parseXML(domelement),true);
	KOMACROTEST_ASSERT(macro2->parseXML(domelement),true);

	//check that it is not null
	KOMACROTEST_XASSERT(sizetype(macro1.data()), sizetype(0));
	KOMACROTEST_XASSERT(sizetype(macro2.data()), sizetype(0));

	//check macro1 == macro2
	KOMACROTEST_ASSERT(macro1->name(), macro2->name() );

	//create list of KsharedPtr from the childs of the macro	
	QValueList< KSharedPtr<KoMacro::MacroItem> >& items1 = macro1->items();
	QValueList< KSharedPtr<KoMacro::MacroItem> >& items2 = macro2->items();
	
	//check that there is one
	KOMACROTEST_XASSERT( items1.count(), sizetype(0) );	
	KOMACROTEST_XASSERT( items2.count(), sizetype(0) );	

	//check macro1 == macro2	
	KOMACROTEST_ASSERT( items1.count(), items2.count() );
	
	//check the name
	KOMACROTEST_ASSERT( QString(macro1->name()), QString("testMacro") );
	
	{
		const QString tmp1 = QString("test");
		macro1->setName(tmp1);

		//check the name changed
		KOMACROTEST_XASSERT( QString(macro1->name()), QString("testMacro") );	
		//check the name
		KOMACROTEST_ASSERT( QString(macro1->name()), QString("test") );
	}
	
	//fetch the first one
	KSharedPtr<KoMacro::Action> actionptr = items1[0]->action();
	//check that it is not null
	KOMACROTEST_XASSERT(sizetype(actionptr.data()), sizetype(0));
	//check that it has the right name
	KOMACROTEST_ASSERT( QString(actionptr->name()), QString("testaction") );
	//check that it has the right text
	KOMACROTEST_ASSERT( actionptr->text(), QString("Test") );

	//try to clear items
	macro1->clearItems();
	//get items
	items1 = macro1->items();
	//check taht they are deleted
	KOMACROTEST_ASSERT( items1.count(), sizetype(0) );
}
#include "macrotests.moc"
