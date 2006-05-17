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

#include "actiontests.h"
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
#include <ksharedptr.h>

using namespace KUnitTest;
using namespace KoMacroTest;

namespace KoMacroTest {

	/**
	* Register KoMacroTest::CommonTests as TestSuite.
	*/
	KUNITTEST_SUITE("KoMacroTestSuite")
	KUNITTEST_REGISTER_TESTER(ActionTests);

	class ActionTests::Private
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

		Private()
			: xmlguiclient(0)
			, testaction(0)
			, doomdocument(0)
		{
		}
	};
}

typedef QValueList< KSharedPtr<KoMacro::MacroItem> >::size_type sizetype;


ActionTests::ActionTests()
	: KUnitTest::SlotTester()
	, d( new Private() ) // create the private d-pointer instance.
{
}

ActionTests::~ActionTests()
{
	delete d->xmlguiclient;
	delete d;
}


void ActionTests::setUp()
{
	d->xmlguiclient = new KXMLGUIClient();
	
	//Singelton more or less ...
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
}

void ActionTests::tearDown()
{
	delete d->doomdocument;
	delete d->xmlguiclient;
}

void ActionTests::testAction()
{
	const QString testString = "teststring";
	const QString testInt = "testint";
	const QString testBool = "testbool";
	
	//TODO: CLEANUP!!!!!!	
	//TODO: test manipulation of action and macroitem and context.
	
	kdDebug()<<"===================== testAction() ======================" << endl;

	//Publish TestAction for the first time.
		
	QDomElement const domelement = d->doomdocument->documentElement();
	
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	
	//Is our XML parseable ?
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	
	//??
	macro->execute(this);

	//create list of KSharedPtr from the childs of the macro
	QValueList< KSharedPtr<KoMacro::MacroItem> >& items = macro->items();
	
	
	//check that there is one
	KOMACROTEST_ASSERT( items.count(), sizetype(1) );
	//get it
	KSharedPtr<KoMacro::Action> actionptr = items[0]->action();
	//-> check that it is not null
	KOMACROTEST_XASSERT(sizetype(actionptr.data()), sizetype(0));
	//check name
	KOMACROTEST_ASSERT(actionptr->name(),QString("testaction"));
	//change it
	actionptr->setName("ActionJackson");
	//retest it
	KOMACROTEST_ASSERT(actionptr->name(),QString("ActionJackson"));
	//check i18n text
	KOMACROTEST_ASSERT(actionptr->text(),QString("Test"));
	//change it
	actionptr->setText("']['3 $']['");
	//retest it
	KOMACROTEST_ASSERT(actionptr->text(),QString("']['3 $']['"));
	//check comment
	KOMACROTEST_XASSERT(actionptr->comment(),QString("No Comment!"));
	//set comment
	actionptr->setComment("Stringtest");
	//check comment again
	KOMACROTEST_ASSERT(actionptr->comment(),QString("Stringtest"));
	//fetch the "teststring"-variable
	KSharedPtr<KoMacro::Variable> variableptr = actionptr->variable(testString);	
	//So there is a variable, does hasVariable() work ?
	KOMACROTEST_ASSERT(actionptr->hasVariable(testString),true);
	//check count of variables
	KOMACROTEST_ASSERT(sizetype(actionptr->variableNames().count()),sizetype(4));
	//remove one
	actionptr->removeVariable(testString);
	//Decreased ??
	KOMACROTEST_ASSERT(sizetype(actionptr->variableNames().count()),sizetype(3));	
	//add one
	actionptr->setVariable(variableptr);
	//increased ??
	KOMACROTEST_ASSERT(sizetype(actionptr->variableNames().count()),sizetype(4));	
	
	//check that it is not null
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	//check that it is "testString"
	KOMACROTEST_ASSERT(variableptr->variant().toString(),QString("testString"));
	

	//fetch the "testint"-variable
	variableptr = actionptr->variable(testInt);
	//check that it is not null
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	//check that it is " "
	KOMACROTEST_ASSERT(variableptr->variant().toInt(),int(0));
	
	//fetch the "testbool"-variable
	variableptr = actionptr->variable("testbool");
	//check that it is not null
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	//check that it is " "
	KOMACROTEST_ASSERT(variableptr->variant().toBool(),true);

	actionptr->setVariable("teststring", "STRINGTEST", "TestString");
	variableptr = actionptr->variable("teststring");
	//check that it is not null
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	//check that it is " "
	KOMACROTEST_ASSERT(variableptr->variant().toString(),QString("TestString"));
	
	actionptr->setVariable(testInt,"INTTEST",INT_MAX);
	variableptr = actionptr->variable("testint");
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(INT_MAX));
	
	actionptr->setVariable("testbool","BOOLTEST", "false");
	variableptr = actionptr->variable("testbool");
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(variableptr->variant().toBool(),false);
	
	//create new macroitem for testing
	KoMacro::MacroItem* macroitem = new KoMacro::MacroItem();
	//set the action
	macroitem->setAction(d->testaction);
	//append the macroitem to testitems
	items.append(macroitem);
	//increased ??
	KOMACROTEST_ASSERT( items.count(), sizetype(2) );
		
	//Manipulate the macroitem
	macroitem->setVariable("teststring", "TeStString");
	variableptr = macroitem->variable("teststring");
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(variableptr->variant().toString(),QString("TeStString"));
		
	macroitem->setVariable(testInt,INT_MIN);
	variableptr = macroitem->variable(testInt);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(INT_MIN));
	
	macroitem->setVariable(testInt,-1);
	variableptr = macroitem->variable(testInt);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(-1));
	
	macroitem->setVariable(testInt,QVariant(0));
	variableptr = macroitem->variable(testInt);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(0));
	
	
	macroitem->setVariable(testInt,1);
	variableptr = macroitem->variable(testInt);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(1));
	
	macroitem->setVariable(testInt,INT_MAX);
	variableptr = macroitem->variable(testInt);
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(INT_MAX));
	
	macroitem->setVariable("testbool","false");
	variableptr = macroitem->variable("testbool");
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(variableptr->variant().toBool(),false);
	
	//secondway for appending an macroitem
	//add the manipulated macroitem
	macro->addItem(macroitem);


	//increased ??
	KOMACROTEST_ASSERT( items.count(), sizetype(3));
	
}
#include "actiontests.moc"
