/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Bernd Steindorff (bernd@itii.de)
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

#include "xmlhandlertests2.h"
#include "testaction.h"
#include "komacrotestbase.h"

#include "../lib/action.h"
#include "../lib/manager.h"
#include "../lib/macro.h"
#include "../lib/variable.h"
#include "../lib/macroitem.h"

#include <ostream>
#include <cfloat>

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
	KUNITTEST_REGISTER_TESTER(XMLHandlerTests2);

	class XMLHandlerTests2::Private
	{
		public:		
		/**
		* An KXMLGUIClient instance created on @a setUp() and
		* passed to the @a KoMacro::Manager to bridge to the
		* app-functionality.
		*/
		KXMLGUIClient* xmlguiclient;

		/**
		* @a Macro instance as a container for the macroitems;
		*/
		KSharedPtr<KoMacro::Macro> macro;		// container for manually created items
		KSharedPtr<KoMacro::Macro> macro2;		// container for parsen items
		KSharedPtr<KoMacro::Macro> macro3;		// container for parsen items after back-converting by toXML() and again parseXML()

		/**
		* An @a TestObject instance used internaly to test
		* handling and communication with from QObject
		* inheritated instances.
		*/
		KSharedPtr<KoMacro::Action> testaction;
		KSharedPtr<KoMacro::Action> action2;	// action of the parsen macro2
		KSharedPtr<KoMacro::Action> action3;	// action of the parsen macro3
		KSharedPtr<KoMacro::Action> testaction_2;	// for test12
		KSharedPtr<KoMacro::Action> action2_2;	// action of the parsen macro2, for test12
		KSharedPtr<KoMacro::Action> action3_2;	// action of the parsen macro3, for test12

		/**
		* Represents a @a QValuList of @a MacroItem which are parsen in the 
		* correspondig @a Macro .
		*/
		QValueList<KSharedPtr<KoMacro::MacroItem > > macroitems2;	// items of macro2
		QValueList<KSharedPtr<KoMacro::MacroItem > > macroitems3;	// items of macro3

		/**
		* @a MacroItem instances which ist fillen manually from the given XML
		* and parsen by the @a XMLHandler over the XML.
		*/
		KSharedPtr<KoMacro::MacroItem> macroitem;	// created manually from XML
		KSharedPtr<KoMacro::MacroItem> macroitem2;	// parsen from XML in macro2
		KSharedPtr<KoMacro::MacroItem> macroitem3;	// parsen from XML in macro3
		KSharedPtr<KoMacro::MacroItem> macroitem_2;	// created manually from XML, for test12
		KSharedPtr<KoMacro::MacroItem> macroitem2_2;// parsen from XML in macro2, for test12
		KSharedPtr<KoMacro::MacroItem> macroitem3_2;// parsen from XML in macro3, for test12

		Private()
			: xmlguiclient(0)
			, testaction(0)
		{
		}
	};
}

// TODO: 	- assertMacroEqMacro

XMLHandlerTests2::XMLHandlerTests2()
	: KUnitTest::SlotTester()
	, d( new Private() ) // create the private d-pointer instance.
{
}

XMLHandlerTests2::~XMLHandlerTests2()
{
	delete d->xmlguiclient;
	delete d;
}


void XMLHandlerTests2::setUp()
{
	d->xmlguiclient = new KXMLGUIClient();
	
	//Singelton more or less ...
	if (::KoMacro::Manager::self() == 0) {	
		::KoMacro::Manager::init( d->xmlguiclient );
	}

	d->macro = KoMacro::Manager::self()->createMacro("testMacro");
	d->macro2 = KoMacro::Manager::self()->createMacro("testMacro");
	d->macro3 = KoMacro::Manager::self()->createMacro("testMacro");

	d->testaction = new TestAction();
	d->testaction_2 = new TestAction();
	::KoMacro::Manager::self()->publishAction(d->testaction);
	::KoMacro::Manager::self()->publishAction(d->testaction_2);
}

void XMLHandlerTests2::tearDown()
{
	delete d->xmlguiclient;
}

/**
* Test the @a KoMacro::XMLHandler parseXML() and toXML()-function.
*/
void XMLHandlerTests2::testParseAndToXML()
{
	kdDebug()<<"===================== testParseAndToXML2() ======================" << endl;

	// 1.Test - Correct DomElement.
	testCorrectDomElement();
// 	// 2.Test - XML-document with bad root element.
// 	testBadRoot();
	// 3.Test - XML-document with a missing Variable.
	testMissingVariable();
	// 4.Test - One more Variable in XML-Document.
	testMoreVariables();
// 	// 5.Test - XML-document with wrong macro-xmlversion.
// 	testWrongVersion();
// 	// 6.Test - XML-document if it has a wrong structure like wrong parathesis
// 	// 	or missing end tag.
// 	testWrongXMLStruct();
	// 7.Test-XML-document with maximum field-size.
	testMaxNum();
	// 8.Test-XML-document with maximum+1 field-size.
	testMaxNum2();
	// 9.Test-XML-document with minimum field-size.
	testMinNum();
	// 10.Test-XML-document with minimum-1 field-size.
	testMinNum2();
	// 11.Test - With a to big number.
	testBigNumber();
// 	// 12.Test - With two MacroItems.
	testTwoMacroItems();
}


/***************************************************************************
* Begin of Sub-methos of testParseXML().
***************************************************************************/
// 1.Test - Correct DomElement.
void XMLHandlerTests2::testCorrectDomElement()
{
	// Clear macroitems in the macros.
	d->macro->clearItems();
	d->macro2->clearItems();
	d->macro3->clearItems();

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >test_string</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	// Set the XML-document with the above string.
	QDomDocument doomdocument;
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();

	// Create a MacroItem with the TestAction for macro2 and add it to macro.
	d->macroitem = new KoMacro::MacroItem();
	d->macro->addItem(d->macroitem);

	d->macroitem->setAction(d->testaction);

	// Push the Variables into the macroitem.
	KSharedPtr<KoMacro::Variable> varstring = 	d->macroitem->addVariable("teststring",QVariant("test_string"));
	KSharedPtr<KoMacro::Variable> varint = 		d->macroitem->addVariable("testint",QVariant(0));
	KSharedPtr<KoMacro::Variable> varbool = 	d->macroitem->addVariable("testbool",QVariant(true));
	KSharedPtr<KoMacro::Variable> vardouble = 	d->macroitem->addVariable("testdouble",QVariant(0.6));

	// Is our XML parseable into a 2. Macro by calling parseXML()?
	KOMACROTEST_ASSERT(d->macro2->parseXML(elem),true);

	// Go down to the MacroItem of macro2.
	d->macroitems2 = d->macro2->items();
	// 1a.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems2.size(),(sizetypelist)1);

	{
		// 2a.comparison - Test if the Action is correct?
		d->macroitem2 = *d->macroitems2.constBegin();
		d->action2 = d->macroitem2->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action2),true);

		// 3a.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem2->variables().size(),(sizetypemap)4);
		{	
			// 4a.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem2->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem2->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem2->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem2->variable("testdouble")),true);
		}
	}

	// Now convert the parsen macro2 back to a QDomElement and again into macro3 for a better comparison.
	const QDomElement elem2 = d->macro2->toXML();
	KOMACROTEST_ASSERT(d->macro3->parseXML(elem2),true);

	// Go down to the MacroItem of macro2.
	d->macroitems3 = d->macro3->items();
	// 1b.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems3.size(),(sizetypelist)1);

	{
		// 2b.comparison - Test if the Action is correct?
		d->macroitem3 = *d->macroitems3.constBegin();
		d->action3 = d->macroitem3->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action3),true);

		// 3b.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem3->variables().size(),(sizetypemap)4);
		{	
			// 4b.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem3->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem3->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem3->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem3->variable("testdouble")),true);
		}
	}
}

/*
// 2.Test - XML-document with bad root element.
void XMLHandlerTests2::testBadRoot()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	const QString xml = QString("<!DOCTYPE macros>"
				    "<maro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</maro>");
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();
	KOMACROTEST_XASSERT(macro->parseXML(elem),true);

	//no assertMacroContentEqToXML(), because parsing failed.
	assertMacroContentEqToXML(macro,elem,true,false,QMap<QString,bool>());

	const QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,true,false,QMap<QString,bool>());
}
*/
// 3.Test - XML-document with a missing Variable.
void XMLHandlerTests2::testMissingVariable()
{
	// Clear macroitems in the macros.
	d->macro->clearItems();
	d->macro2->clearItems();
	d->macro3->clearItems();

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >test_string</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	// Set the XML-document with the above string.
	QDomDocument doomdocument;
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();

	// Create a MacroItem with the TestAction for macro2 and add it to macro.
	d->macroitem = new KoMacro::MacroItem();
	d->macro->addItem(d->macroitem);

	d->macroitem->setAction(d->testaction);

	// Push the Variables into the macroitem.
	KSharedPtr<KoMacro::Variable> varstring = 	d->macroitem->addVariable("teststring",QVariant("test_string"));
	KSharedPtr<KoMacro::Variable> varint = 		d->macroitem->addVariable("testint",QVariant(0));
	KSharedPtr<KoMacro::Variable> vardouble = 	d->macroitem->addVariable("testdouble",QVariant(0.6));
		
// Is our XML parseable into a 2. Macro by calling parseXML()?
	KOMACROTEST_ASSERT(d->macro2->parseXML(elem),true);

	// Go down to the MacroItem of macro2.
	d->macroitems2 = d->macro2->items();
	// 1a.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems2.size(),(sizetypelist)1);

	{
		// 2a.comparison - Test if the Action is correct?
		d->macroitem2 = *d->macroitems2.constBegin();
		d->action2 = d->macroitem2->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action2),true);

		// 3a.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem2->variables().size(),(sizetypemap)3);
		{	
			// 4a.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem2->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem2->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem2->variable("testdouble")),true);
		}
	}

	// Now convert the parsen macro2 back to a QDomElement and again into macro3 for a better comparison.
	const QDomElement elem2 = d->macro2->toXML();
	KOMACROTEST_ASSERT(d->macro3->parseXML(elem2),true);

	// Go down to the MacroItem of macro2.
	d->macroitems3 = d->macro3->items();
	// 1b.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems3.size(),(sizetypelist)1);

	{
		// 2b.comparison - Test if the Action is correct?
		d->macroitem3 = *d->macroitems3.constBegin();
		d->action3 = d->macroitem3->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action3),true);

		// 3b.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem3->variables().size(),(sizetypemap)3);
		{	
			// 4b.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem3->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem3->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem3->variable("testdouble")),true);
		}
	}
}

// 4.Test - One more Variable in XML-Document.
void XMLHandlerTests2::testMoreVariables()
{
	// Clear macroitems in the macros.
	d->macro->clearItems();
	d->macro2->clearItems();
	d->macro3->clearItems();

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >test_string</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
						"<variable name=\"testbla\" >somethingwrong</variable>"
				      "</item>"
				    "</macro>");
	// Set the XML-document with the above string.
	QDomDocument doomdocument;
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();

	// Create a MacroItem with the TestAction for macro2 and add it to macro.
	d->macroitem = new KoMacro::MacroItem();
	d->macro->addItem(d->macroitem);

	d->macroitem->setAction(d->testaction);

	// Push the Variables into the macroitem.
	KSharedPtr<KoMacro::Variable> varstring = 	d->macroitem->addVariable("teststring",QVariant("test_string"));
	KSharedPtr<KoMacro::Variable> varint = 		d->macroitem->addVariable("testint",QVariant(0));
	KSharedPtr<KoMacro::Variable> varbool = 	d->macroitem->addVariable("testbool",QVariant(true));
	KSharedPtr<KoMacro::Variable> vardouble = 	d->macroitem->addVariable("testdouble",QVariant(0.6));
	KSharedPtr<KoMacro::Variable> varbla = 		d->macroitem->addVariable("testbla","somethingwrong");

	// Is our XML parseable into a 2. Macro by calling parseXML()?
	KOMACROTEST_ASSERT(d->macro2->parseXML(elem),true);

	// Go down to the MacroItem of macro2.
	d->macroitems2 = d->macro2->items();
	// 1a.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems2.size(),(sizetypelist)1);

	{
		// 2a.comparison - Test if the Action is correct?
		d->macroitem2 = *d->macroitems2.constBegin();
		d->action2 = d->macroitem2->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action2),true);

		// 3a.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem2->variables().size(),(sizetypemap)5);
		{	
			// 4a.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem2->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem2->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem2->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem2->variable("testdouble")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbla,		d->macroitem2->variable("testbla")),true);
		}
	}

	// Now convert the parsen macro2 back to a QDomElement and again into macro3 for a better comparison.
	const QDomElement elem2 = d->macro2->toXML();
	KOMACROTEST_ASSERT(d->macro3->parseXML(elem2),true);

	// Go down to the MacroItem of macro2.
	d->macroitems3 = d->macro3->items();
	// 1b.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems3.size(),(sizetypelist)1);

	{
		// 2b.comparison - Test if the Action is correct?
		d->macroitem3 = *d->macroitems3.constBegin();
		d->action3 = d->macroitem3->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action3),true);

		// 3b.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem3->variables().size(),(sizetypemap)5);
		{	
			// 4b.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem3->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem3->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem3->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem3->variable("testdouble")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbla,		d->macroitem3->variable("testbla")),true);
		}
	}
}

/*
// 5.Test - XML-document with wrong macro-xmlversion.
void XMLHandlerTests2::testWrongVersion()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"2\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();
	KOMACROTEST_XASSERT(macro->parseXML(elem),true);

	//no assertMacroContentEqToXML(), because parsing failed.
	assertMacroContentEqToXML(macro,elem,true,false,QMap<QString,bool>());
	
	const QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,true,false,QMap<QString,bool>());
}

// 6.Test - XML-document if it has a wrong structure like wrong parathesis
// 	or missing end tag.
void XMLHandlerTests2::testWrongXMLStruct()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	const QString xml = QString("<!DOCTYPE macros>"
				    "macro xmlversion=\"1\">>"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
				      "</item>"
				    "</macro>");
	KOMACROTEST_XASSERT(doomdocument.setContent(xml),true);
	const QDomElement elem = doomdocument.documentElement();
	KOMACROTEST_XASSERT(macro->parseXML(elem),true);

	//no assertMacroContentEqToXML(), because parsing failed.
	assertMacroContentEqToXML(macro,elem,true,false,QMap<QString,bool>());
	
	const QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,true,false,QMap<QString,bool>());
}
*/
// 7.Test-XML-document with maximum field-size.
void XMLHandlerTests2::testMaxNum()
{
	// Clear macroitems in the macros.
	d->macro->clearItems();
	d->macro2->clearItems();
	d->macro3->clearItems();

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >test_string</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MAX).arg(DBL_MAX);
	// Set the XML-document with the above string.
	QDomDocument doomdocument;
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();

	// Create a MacroItem with the TestAction for macro2 and add it to macro.
	d->macroitem = new KoMacro::MacroItem();
	d->macro->addItem(d->macroitem);

	d->macroitem->setAction(d->testaction);

	// Push the Variables into the macroitem.
	KSharedPtr<KoMacro::Variable> varstring = 	d->macroitem->addVariable("teststring",QVariant("test_string"));
	KSharedPtr<KoMacro::Variable> varint = 		d->macroitem->addVariable("testint",QVariant(INT_MAX));
	KSharedPtr<KoMacro::Variable> varbool = 	d->macroitem->addVariable("testbool",QVariant(true));
	KSharedPtr<KoMacro::Variable> vardouble = 	d->macroitem->addVariable("testdouble",QVariant(DBL_MAX));

	// Is our XML parseable into a 2. Macro by calling parseXML()?
	KOMACROTEST_ASSERT(d->macro2->parseXML(elem),true);

	// Go down to the MacroItem of macro2.
	d->macroitems2 = d->macro2->items();
	// 1a.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems2.size(),(sizetypelist)1);

	{
		// 2a.comparison - Test if the Action is correct?
		d->macroitem2 = *d->macroitems2.constBegin();
		d->action2 = d->macroitem2->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action2),true);

		// 3a.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem2->variables().size(),(sizetypemap)4);
		{	
			// 4a.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem2->variable("teststring")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem2->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem2->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem2->variable("testdouble")),	true);
		}
	}

	// Now convert the parsen macro2 back to a QDomElement and again into macro3 for a better comparison.
	const QDomElement elem2 = d->macro2->toXML();
	KOMACROTEST_ASSERT(d->macro3->parseXML(elem2),true);

	// Go down to the MacroItem of macro2.
	d->macroitems3 = d->macro3->items();
	// 1b.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems3.size(),(sizetypelist)1);

	{
		// 2b.comparison - Test if the Action is correct?
		d->macroitem3 = *d->macroitems3.constBegin();
		d->action3 = d->macroitem3->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action3),true);

		// 3b.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem3->variables().size(),(sizetypemap)4);
		{	
			// 4b.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem3->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem3->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem3->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem3->variable("testdouble")),true);
		}
	}
}

// 8.Test-XML-document with maximum+1 field-size.
void XMLHandlerTests2::testMaxNum2()
{
	// Clear macroitems in the macros.
	d->macro->clearItems();
	d->macro2->clearItems();
	d->macro3->clearItems();

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >test_string</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MAX+1).arg(DBL_MAX+1);
	// Set the XML-document with the above string.
	QDomDocument doomdocument;
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();

	// Create a MacroItem with the TestAction for macro2 and add it to macro.
	d->macroitem = new KoMacro::MacroItem();
	d->macro->addItem(d->macroitem);

	d->macroitem->setAction(d->testaction);

	// Push the Variables into the macroitem.
	KSharedPtr<KoMacro::Variable> varstring = 	d->macroitem->addVariable("teststring",QVariant("test_string"));
	KSharedPtr<KoMacro::Variable> varint = 		d->macroitem->addVariable("testint",QVariant(INT_MAX+1));
	KSharedPtr<KoMacro::Variable> varbool = 	d->macroitem->addVariable("testbool",QVariant(true));
	KSharedPtr<KoMacro::Variable> vardouble = 	d->macroitem->addVariable("testdouble",QVariant(DBL_MAX+1));

	// Is our XML parseable into a 2. Macro by calling parseXML()?
	KOMACROTEST_ASSERT(d->macro2->parseXML(elem),true);

	// Go down to the MacroItem of macro2.
	d->macroitems2 = d->macro2->items();
	// 1a.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems2.size(),(sizetypelist)1);

	{
		// 2a.comparison - Test if the Action is correct?
		d->macroitem2 = *d->macroitems2.constBegin();
		d->action2 = d->macroitem2->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action2),true);

		// 3a.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem2->variables().size(),(sizetypemap)4);
		{	
			// 4a.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem2->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem2->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem2->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem2->variable("testdouble")),true);
		}
	}

	// Now convert the parsen macro2 back to a QDomElement and again into macro3 for a better comparison.
	const QDomElement elem2 = d->macro2->toXML();
	KOMACROTEST_ASSERT(d->macro3->parseXML(elem2),true);

	// Go down to the MacroItem of macro2.
	d->macroitems3 = d->macro3->items();
	// 1b.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems3.size(),(sizetypelist)1);

	{
		// 2b.comparison - Test if the Action is correct?
		d->macroitem3 = *d->macroitems3.constBegin();
		d->action3 = d->macroitem3->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action3),true);

		// 3b.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem3->variables().size(),(sizetypemap)4);
		{	
			// 4b.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem3->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem3->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem3->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem3->variable("testdouble")),true);
		}
	}
}

// 9.Test-XML-document with minimum field-size.
void XMLHandlerTests2::testMinNum()
{
	// Clear macroitems in the macros.
	d->macro->clearItems();
	d->macro2->clearItems();
	d->macro3->clearItems();

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >test_string</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MIN).arg(DBL_MIN);
	// Set the XML-document with the above string.
	QDomDocument doomdocument;
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();

	// Create a MacroItem with the TestAction for macro2 and add it to macro.
	d->macroitem = new KoMacro::MacroItem();
	d->macro->addItem(d->macroitem);

	d->macroitem->setAction(d->testaction);

	// Push the Variables into the macroitem.
	KSharedPtr<KoMacro::Variable> varstring = 	d->macroitem->addVariable("teststring",QVariant("test_string"));
	KSharedPtr<KoMacro::Variable> varint = 		d->macroitem->addVariable("testint",QVariant(INT_MIN));
	KSharedPtr<KoMacro::Variable> varbool = 	d->macroitem->addVariable("testbool",QVariant(true));
	KSharedPtr<KoMacro::Variable> vardouble = 	d->macroitem->addVariable("testdouble",QVariant(DBL_MIN));

	// Is our XML parseable into a 2. Macro by calling parseXML()?
	KOMACROTEST_ASSERT(d->macro2->parseXML(elem),true);

	// Go down to the MacroItem of macro2.
	d->macroitems2 = d->macro2->items();
	// 1a.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems2.size(),(sizetypelist)1);

	{
		// 2a.comparison - Test if the Action is correct?
		d->macroitem2 = *d->macroitems2.constBegin();
		d->action2 = d->macroitem2->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action2),true);

		// 3a.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem2->variables().size(),(sizetypemap)4);
		{	
			// 4a.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem2->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem2->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem2->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem2->variable("testdouble")),true);
		}
	}

	// Now convert the parsen macro2 back to a QDomElement and again into macro3 for a better comparison.
	const QDomElement elem2 = d->macro2->toXML();
	KOMACROTEST_ASSERT(d->macro3->parseXML(elem2),true);

	// Go down to the MacroItem of macro2.
	d->macroitems3 = d->macro3->items();
	// 1b.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems3.size(),(sizetypelist)1);

	{
		// 2b.comparison - Test if the Action is correct?
		d->macroitem3 = *d->macroitems3.constBegin();
		d->action3 = d->macroitem3->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action3),true);

		// 3b.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem3->variables().size(),(sizetypemap)4);
		{	
			// 4b.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem3->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem3->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem3->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem3->variable("testdouble")),true);
		}
	}
}

// 10.Test-XML-document with minimum+1 field-size.
void XMLHandlerTests2::testMinNum2()
{
	// Clear macroitems in the macros.
	d->macro->clearItems();
	d->macro2->clearItems();
	d->macro3->clearItems();

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >test_string</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MIN-1).arg(DBL_MIN-1);
	// Set the XML-document with the above string.
	QDomDocument doomdocument;
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();

	// Create a MacroItem with the TestAction for macro2 and add it to macro.
	d->macroitem = new KoMacro::MacroItem();
	d->macro->addItem(d->macroitem);

	d->macroitem->setAction(d->testaction);

	// Push the Variables into the macroitem.
	KSharedPtr<KoMacro::Variable> varstring = 	d->macroitem->addVariable("teststring",QVariant("test_string"));
	KSharedPtr<KoMacro::Variable> varint = 		d->macroitem->addVariable("testint",QVariant(INT_MIN-1));
	KSharedPtr<KoMacro::Variable> varbool = 	d->macroitem->addVariable("testbool",QVariant(true));
	KSharedPtr<KoMacro::Variable> vardouble = 	d->macroitem->addVariable("testdouble",QVariant(DBL_MIN-1));

	// Is our XML parseable into a 2. Macro by calling parseXML()?
	KOMACROTEST_ASSERT(d->macro2->parseXML(elem),true);

	// Go down to the MacroItem of macro2.
	d->macroitems2 = d->macro2->items();
	// 1a.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems2.size(),(sizetypelist)1);

	{
		// 2a.comparison - Test if the Action is correct?
		d->macroitem2 = *d->macroitems2.constBegin();
		d->action2 = d->macroitem2->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action2),true);

		// 3a.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem2->variables().size(),(sizetypemap)4);
		{	
			// 4a.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem2->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem2->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem2->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem2->variable("testdouble")),true);
		}
	}

	// Now convert the parsen macro2 back to a QDomElement and again into macro3 for a better comparison.
	const QDomElement elem2 = d->macro2->toXML();
	KOMACROTEST_ASSERT(d->macro3->parseXML(elem2),true);

	// Go down to the MacroItem of macro2.
	d->macroitems3 = d->macro3->items();
	// 1b.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems3.size(),(sizetypelist)1);

	{
		// 2b.comparison - Test if the Action is correct?
		d->macroitem3 = *d->macroitems3.constBegin();
		d->action3 = d->macroitem3->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action3),true);

		// 3b.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem3->variables().size(),(sizetypemap)4);
		{	
			// 4b.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem3->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem3->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem3->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem3->variable("testdouble")),true);
		}
	}
}

// 11.Test - With a to big number.
void XMLHandlerTests2::testBigNumber()
{
	// Clear macroitems in the macros.
	d->macro->clearItems();
	d->macro2->clearItems();
	d->macro3->clearItems();

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >test_string</variable>"
						"<variable name=\"testint\" >0123456789012345678901234567890123456789</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	// Set the XML-document with the above string.
	QDomDocument doomdocument;
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();

	// Create a MacroItem with the TestAction for macro2 and add it to macro.
	d->macroitem = new KoMacro::MacroItem();
	d->macro->addItem(d->macroitem);

	d->macroitem->setAction(d->testaction);

	// Push the Variables into the macroitem.
	KSharedPtr<KoMacro::Variable> varstring = 	d->macroitem->addVariable("teststring",QVariant("test_string"));
	//TODO //KSharedPtr<KoMacro::Variable> varint = 		d->macroitem->addVariable("testint",QVariant(0123456789012345678901234567890123456789));
	KSharedPtr<KoMacro::Variable> varbool = 	d->macroitem->addVariable("testbool",QVariant(true));
	KSharedPtr<KoMacro::Variable> vardouble = 	d->macroitem->addVariable("testdouble",QVariant(0.6));
	
	// Is our XML parseable into a 2. Macro by calling parseXML()?
	KOMACROTEST_ASSERT(d->macro2->parseXML(elem),true);

	// Go down to the MacroItem of macro2.
	d->macroitems2 = d->macro2->items();
	// 1a.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems2.size(),(sizetypelist)1);

	{
		// 2a.comparison - Test if the Action is correct?
		d->macroitem2 = *d->macroitems2.constBegin();
		d->action2 = d->macroitem2->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action2),true);

		// 3a.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem2->variables().size(),(sizetypemap)4);
		{	
			// 4a.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem2->variable("teststring")),true);
			//KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem2->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem2->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem2->variable("testdouble")),true);
		}
	}

	// Now convert the parsen macro2 back to a QDomElement and again into macro3 for a better comparison.
	const QDomElement elem2 = d->macro2->toXML();
	KOMACROTEST_ASSERT(d->macro3->parseXML(elem2),true);

	// Go down to the MacroItem of macro2.
	d->macroitems3 = d->macro3->items();
	// 1b.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems3.size(),(sizetypelist)1);

	{
		// 2b.comparison - Test if the Action is correct?
		d->macroitem3 = *d->macroitems3.constBegin();
		d->action3 = d->macroitem3->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action3),true);

		// 3b.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem3->variables().size(),(sizetypemap)4);
		{	
			// 4b.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem3->variable("teststring")),true);
			//KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem3->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem3->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem3->variable("testdouble")),true);
		}
	}
}

// 12.Test - With two MacroItems.
void XMLHandlerTests2::testTwoMacroItems()
{
	// Clear macroitems in the macros.
	d->macro->clearItems();
	d->macro2->clearItems();
	d->macro3->clearItems();

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >test_string</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
						"<variable name=\"testbla\" >somethingwrong</variable>"
				      "</item>"
					"<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString2</variable>"
						"<variable name=\"testint\" >4</variable>"
						"<variable name=\"testbool\" >false</variable>"
						"<variable name=\"testdouble\" >0.7</variable>"
						"<variable name=\"testbla\" >somethingwrong2</variable>"
				      "</item>"
				    "</macro>");
	// Set the XML-document with the above string.
	QDomDocument doomdocument;
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();

	// Create a MacroItem with the TestAction for macro2 and add it to macro.
	d->macroitem = new KoMacro::MacroItem();
	d->macroitem_2 = new KoMacro::MacroItem();
	d->macro->addItem(d->macroitem);
	d->macro->addItem(d->macroitem_2);

	d->macroitem->setAction(d->testaction);
	d->macroitem_2->setAction(d->testaction_2);

	// Push the Variables into the macroitem.
	KSharedPtr<KoMacro::Variable> varstring = 	d->macroitem->addVariable("teststring",QVariant("test_string"));
	KSharedPtr<KoMacro::Variable> varint = 		d->macroitem->addVariable("testint",QVariant(0));
	KSharedPtr<KoMacro::Variable> varbool = 	d->macroitem->addVariable("testbool",QVariant(true));
	KSharedPtr<KoMacro::Variable> vardouble = 	d->macroitem->addVariable("testdouble",QVariant(0.6));
	KSharedPtr<KoMacro::Variable> varbla = 		d->macroitem->addVariable("testbla","somethingwrong");

	// Push the Variables into the macroitem4.
	KSharedPtr<KoMacro::Variable> varstring_2 = d->macroitem_2->addVariable("teststring",QVariant("testString2"));
	KSharedPtr<KoMacro::Variable> varint_2 = 	d->macroitem_2->addVariable("testint",QVariant(4));
	KSharedPtr<KoMacro::Variable> varbool_2 = 	d->macroitem_2->addVariable("testbool",QVariant(false));
	KSharedPtr<KoMacro::Variable> vardouble_2 = d->macroitem_2->addVariable("testdouble",QVariant(0.7));
	KSharedPtr<KoMacro::Variable> varbla_2 = 	d->macroitem_2->addVariable("testbla","somethingwrong2");

	// Is our XML parseable into a 2. Macro by calling parseXML()?
	KOMACROTEST_ASSERT(d->macro2->parseXML(elem),true);

	// Go down to the MacroItem of macro2.
	d->macroitems2 = d->macro2->items();
	// 1a.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems2.size(),(sizetypelist)2);

	{
		QValueList<KSharedPtr<KoMacro::MacroItem > >::ConstIterator mit2(d->macroitems2.constBegin());
		// 2a.comparison - Test if the Action is correct?
		d->macroitem2 = *mit2;
		mit2++;
		d->macroitem2_2 = *mit2;
		d->action2 = d->macroitem2->action();
		d->action2_2 = d->macroitem2_2->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action2),true);
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction_2,d->action2_2),true);

		// 3a.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem2->variables().size(),(sizetypemap)5);
		KOMACROTEST_ASSERT(d->macroitem2_2->variables().size(),(sizetypemap)5);
		{	
			// 4a.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem2->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem2->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem2->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem2->variable("testdouble")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbla,		d->macroitem2->variable("testbla")),true);

			KOMACROTEST_ASSERT(assertVariablesEqual(varstring_2,d->macroitem2_2->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint_2,	d->macroitem2_2->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool_2,	d->macroitem2_2->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble_2,d->macroitem2_2->variable("testdouble")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbla_2,	d->macroitem2_2->variable("testbla")),true);
		}
	}

	// Now convert the parsen macro2 back to a QDomElement and again into macro3 for a better comparison.
	const QDomElement elem2 = d->macro2->toXML();
	KOMACROTEST_ASSERT(d->macro3->parseXML(elem2),true);

	// Go down to the MacroItem of macro2.
	d->macroitems3 = d->macro3->items();
	// 1b.comparison - Test if the MacroItems have the correct number?
	KOMACROTEST_ASSERT(d->macroitems3.size(),(sizetypelist)2);

	{
	QValueList<KSharedPtr<KoMacro::MacroItem > >::ConstIterator mit3(d->macroitems3.constBegin());
		// 2b.comparison - Test if the Action is correct?
		d->macroitem3 = *mit3;
		mit3++;
		d->macroitem3_2 = *mit3;
		d->action3 = d->macroitem3->action();
		d->action3_2 = d->macroitem3_2->action();
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action3),true);
		KOMACROTEST_ASSERT(assertActionsEqual(d->testaction,d->action3_2),true);

		// 3b.comparison - Test if the Variables have the correct number?
		KOMACROTEST_ASSERT(d->macroitem3->variables().size(),(sizetypemap)5);
		KOMACROTEST_ASSERT(d->macroitem3_2->variables().size(),(sizetypemap)5);
		{	
			// 4b.comparison - Test if the Variables are equal.
			KOMACROTEST_ASSERT(assertVariablesEqual(varstring,	d->macroitem3->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint,		d->macroitem3->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool,	d->macroitem3->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble,	d->macroitem3->variable("testdouble")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbla,		d->macroitem3->variable("testbla")),true);

			KOMACROTEST_ASSERT(assertVariablesEqual(varstring_2,d->macroitem3_2->variable("teststring")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varint_2,	d->macroitem3_2->variable("testint")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbool_2,	d->macroitem3_2->variable("testbool")),	true);
			KOMACROTEST_ASSERT(assertVariablesEqual(vardouble_2,d->macroitem3_2->variable("testdouble")),true);
			KOMACROTEST_ASSERT(assertVariablesEqual(varbla_2,	d->macroitem3_2->variable("testbla")),true);
		}
	}
}

/***************************************************************************
* End of Sub-methos of testParseAndToXML().
***************************************************************************/

/** 
* Compares a XML-Element with a Macro. Call sub-asserts.
* @p macro The parsen @a Macro.
* @p elem The given @a QDomElement which is parsen.
* @p isitemsempty Bool for expectation of an empty @a MacroItem -List.
* @p isactionset Bool for expectation that the @a Action -names are equal.
* @p isvariableok QMap of Bools for comparing each @a Variable .
*/
/*
void XMLHandlerTests2::assertMacroContentEqToXML(const KSharedPtr<KoMacro::Macro> macro,
	const QDomElement& elem,
	const bool isitemsempty,
	const bool isactionset,
	const QMap<QString, bool> isvariableok)
{
	// Make an Iterator over the MacroItems of the Macro.
	const QValueList<KSharedPtr<KoMacro::MacroItem > > macroitems = macro->items();
	QValueList<KSharedPtr<KoMacro::MacroItem > >::ConstIterator 
		mit(macroitems.constBegin()), end(macroitems.constEnd());

	//1.comparison - Is the MacroItem-list empty?
	{
		if( isitemsempty ) {
			KOMACROTEST_XASSERT(macroitems.empty(),false);
			kdDebug() << "There is no correct MacroItem parsen." << endl;
			return;
		}
		else {
			KOMACROTEST_ASSERT(macroitems.empty(),false);
		}
	}

	// Got to the first item-elements of the elem (there is only one in the tests).
	QDomNode itemnode = elem.firstChild();

	// Iterate over the MacroItems and item-elements.
	while(mit != end && ! itemnode.isNull()) {
		const KSharedPtr<KoMacro::MacroItem> macroitem = *mit;
		const QDomElement itemelem = itemnode.toElement();
		
		//2.comparison - Is the Action-name equal?
		{
			if( ! isactionset) {
				KOMACROTEST_XASSERT(macroitem->action()->name() == itemelem.attribute("action"),true);
				kdDebug() 	<< "Action-name not equal: " 
							<< macroitem->action()->name()
							<< " != " << itemelem.attribute("action") << endl;
				return;
			}
			else {
				KOMACROTEST_ASSERT(macroitem->action()->name() == itemelem.attribute("action"),true);
			}
		}

		// Go down to MacroItem->Variable and item->variable and compare them.
		QMap<QString, KSharedPtr<KoMacro::Variable > > mvariables = macroitem->variables();
		QDomNode varnode = itemelem.firstChild();

		while ( ! varnode.isNull()) {
			const QDomElement varelem = varnode.toElement();
			const KSharedPtr<KoMacro::Variable> varitem = mvariables.find(varelem.attribute("name")).data();
			
			//3.comparison - Is the content of the Variable
			// in the MacroItem and and item equal?
			{
				const bool var = *isvariableok.find(varelem.attribute("name"));
				if( ! var ) {
					KOMACROTEST_XASSERT(varitem->variant() == QVariant(varelem.text()), !var);
					kdDebug() 	<< "The content of the Variable: " << varitem->name() 
								<< " is not equal." << varitem->variant()
								<< "!=" << varelem.text() << endl;
				}
				else {
					KOMACROTEST_ASSERT(varitem->variant() == QVariant(varelem.text()), var);
				}

			}

			// Erase the MacroItem from the map, because it is parsen correctly.
			mvariables.erase(varitem->name());
			// Go to next Variable in node-tree.
			varnode = varnode.nextSibling();
		}

		//4.comparison - Is every MacroItem parsen?
		{
			KOMACROTEST_ASSERT(mvariables.empty(),true);
			kdDebug() << "There are non-filled variable in the MacroItem: " << mvariables.count() <<endl;
		}

		// Go to next MacroItem and next item-element.
		mit++;
		itemnode = itemnode.nextSibling();
	}
}

// Prints a QMap of Variables to kdDebug().
void XMLHandlerTests2::printMvariables(const QMap<QString, KSharedPtr<KoMacro::Variable > > mvariables, const QString s)
{
	//QValueList<QString>::ConstIterator kit (keys.constBegin()), end(keys.constEnd());
	QMap<QString, KSharedPtr<KoMacro::Variable > >::ConstIterator mvit (mvariables.constBegin()), end(mvariables.constEnd());
	while(mvit != end){
		const KoMacro::Variable * v = *mvit;
		kdDebug() << s << ": " << v->name() << endl;
		mvit++;
	}
}
*/

bool XMLHandlerTests2::assertActionsEqual(KSharedPtr<KoMacro::Action> action,
	KSharedPtr<KoMacro::Action> action2)
{
	return action->name() == action2->name();
}

bool XMLHandlerTests2::assertVariablesEqual(KSharedPtr<KoMacro::Variable> var,
	KSharedPtr<KoMacro::Variable> var2)
{
	if ( var->variant() != var2->variant() ) kdDebug() << "Variable1: " << var->variant() << " and Variable2: " << var2->variant() << endl;
	return var->variant() == var2->variant();
}

#include "xmlhandlertests2.moc"
