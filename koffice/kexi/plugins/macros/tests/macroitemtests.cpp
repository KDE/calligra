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

#include "macroitemtests.h"
#include "testaction.h"
#include "komacrotestbase.h"

#include "../lib/action.h"
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
	
	KUNITTEST_SUITE("KoMacroTestSuite");
	KUNITTEST_REGISTER_TESTER(MacroitemTests);
	

	class MacroitemTests::Private
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
		TestAction* testaction;

		QDomDocument* doomdocument;
		
		KSharedPtr<KoMacro::Macro> macro;

		Private()
			: xmlguiclient(0)
			, testaction(0)
			, doomdocument(0)
			, macro(0)
		{
		}
	};
}

typedef QValueList< KSharedPtr<KoMacro::MacroItem> >::size_type sizetype;

MacroitemTests::MacroitemTests()
	: KUnitTest::SlotTester()
	, d( new Private() ) // create the private d-pointer instance.
{
}

MacroitemTests::~MacroitemTests()
{
	delete d->xmlguiclient;
	delete d;
}

void MacroitemTests::setUp()
{
	d->xmlguiclient = new KXMLGUIClient();
	
	if (::KoMacro::Manager::self() == 0) {	
		::KoMacro::Manager::init( d->xmlguiclient );
	}
	
	d->testaction = new TestAction();
	::KoMacro::Manager::self()->publishAction(d->testaction);	

	d->doomdocument = new QDomDocument();

	QString const xml = QString("<!DOCTYPE macros>"
				   "<macro xmlversion=\"1\" >"
				      "<item action=\"testaction\" >"
				      "</item>"
				    "</macro>");
	
	d->doomdocument->setContent(xml);
	d->macro = KoMacro::Manager::self()->createMacro("testMacro");
	d->macro->parseXML(d->doomdocument->documentElement());
	d->macro->execute(this);
}

void MacroitemTests::tearDown()
{
	delete d->macro;
	delete d->doomdocument;
	delete d->xmlguiclient;
}


void MacroitemTests::testMacro()
{
	kdDebug()<<"===================== testMacroitem() ======================" << endl;	
	kdDebug()<<"===================== testMacro() ======================" << endl;
	
	//fetch Items and ..
	QValueList< KSharedPtr<KoMacro::MacroItem> >& items = d->macro->items();

	//... check that there is one
	KOMACROTEST_XASSERT( items.count(), sizetype(0) );
}

void MacroitemTests::testMacroItemString()
{
	

	kdDebug()<<"===================== testMacroItemString() ======================" << endl;	
	
	QValueList< KSharedPtr<KoMacro::MacroItem> >& items = d->macro->items();
	KSharedPtr<KoMacro::Action> actionptr = items[0]->action();
	KSharedPtr<KoMacro::Variable> variableptr = actionptr->variable(TESTSTRING);
	
	//create new macroitem for testing
	KoMacro::MacroItem* macroitem = new KoMacro::MacroItem();
	//set the action
	macroitem->setAction(d->testaction);
	
	//append the macroitem to testitems	
	items.append(macroitem);
	
	//increased ??
	KOMACROTEST_ASSERT( items.count(), sizetype(2) );
		
	//Manipulate the macroitem
	macroitem->setVariable(TESTSTRING, "TeStString");
	variableptr = macroitem->variable(TESTSTRING);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(variableptr->variant().toString(),QString("TeStString"));
		
	
	//secondway for appending an macroitem
	//add the manipulated macroitem
	d->macro->addItem(macroitem);

	//increased ??
	KOMACROTEST_ASSERT( items.count(), sizetype(3));
	
}

void MacroitemTests::testMacroItemInt()
{
	

	kdDebug()<<"===================== testMacroItemInt() ======================" << endl;	

	QValueList< KSharedPtr<KoMacro::MacroItem> >& items = d->macro->items();
	KSharedPtr<KoMacro::Action> actionptr = items[0]->action();	
	
	//create new macroitem for testing
	KoMacro::MacroItem* macroitem = new KoMacro::MacroItem();
	//set the action
	macroitem->setAction(d->testaction);
	items.append(macroitem);

	macroitem->setVariable(TESTINT,INT_MIN);
	KSharedPtr<KoMacro::Variable> variableptr = macroitem->variable(TESTINT);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(INT_MIN));
	
	macroitem->setVariable(TESTINT,-1);
	variableptr = macroitem->variable(TESTINT);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(-1));
	
	macroitem->setVariable(TESTINT,QVariant(0));
	variableptr = macroitem->variable(TESTINT);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(0));
	
	macroitem->setVariable(TESTINT,1);
	variableptr = macroitem->variable(TESTINT);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(1));
	
	macroitem->setVariable(TESTINT,INT_MAX);
	variableptr = macroitem->variable(TESTINT);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(INT_MAX));	
}

void MacroitemTests::testMacroItemBool()
{
	

	kdDebug()<<"===================== testMacroItemBool() ======================" << endl;	

	QValueList< KSharedPtr<KoMacro::MacroItem> >& items = d->macro->items();
	KSharedPtr<KoMacro::Action> actionptr = items[0]->action();	
	
	//create new macroitem for testing
	KoMacro::MacroItem* macroitem = new KoMacro::MacroItem();
	//set the action
	macroitem->setAction(d->testaction);	
	items.append(macroitem);

	macroitem->setVariable(TESTBOOL,"false");
	KSharedPtr<KoMacro::Variable> variableptr = macroitem->variable(TESTBOOL);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(variableptr->variant().toBool(),false);
}
#include "macroitemtests.moc"
