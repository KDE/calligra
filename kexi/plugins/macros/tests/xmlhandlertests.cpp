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

#include "xmlhandlertests.h"
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
	KUNITTEST_REGISTER_TESTER(XMLHandlerTests);

	class XMLHandlerTests::Private
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
		KSharedPtr<KoMacro::Action> testaction;

		Private()
			: xmlguiclient(0)
			, testaction(0)
		{
		}
	};
}

XMLHandlerTests::XMLHandlerTests()
	: KUnitTest::SlotTester()
	, d( new Private() ) // create the private d-pointer instance.
{
}

XMLHandlerTests::~XMLHandlerTests()
{
	delete d->xmlguiclient;
	delete d;
}


void XMLHandlerTests::setUp()
{
	d->xmlguiclient = new KXMLGUIClient();
	
	//Singelton more or less ...
	if (::KoMacro::Manager::self() == 0) {	
		::KoMacro::Manager::init( d->xmlguiclient );
	}
	
	d->testaction = new TestAction();
	::KoMacro::Manager::self()->publishAction(d->testaction);	
}

void XMLHandlerTests::tearDown()
{
	delete d->xmlguiclient;
}

/**
* Test the @a KoMacro::XMLHandler parseXML()-function.
* Sub-methods begin with: tpx (from testParseXml).
*/
void XMLHandlerTests::testParseXML()
{
	kdDebug()<<"===================== testParseXML() ======================" << endl;

	tpxTestCorrectDomElement();
	tpxTestBadRoot();
	tpxTestMissingVariable();
	tpxTestMoreVariables();
	tpxTestWrongVersion();
	//tpxTestWrongXMLStruct();
	//tpxTestWrongItemVarTags();
	tpxTestMaxNum();
	tpxTestMaxNum2();
	tpxTestMinNum();
	tpxTestMinNum2();
	tpxTestBigNumber();
}

/***************************************************************************
* Begin of Sub-methos of testParseXML().
***************************************************************************/

// 1.Test - Correct DomElement.
void XMLHandlerTests::tpxTestCorrectDomElement()
{
	// Local Init
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	// Set the XML-document with the above string.
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	// Is our XML parseable ?
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	// Is the parsen content in the Macro correct ?
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);
	// Test the Compare-method when a Variable will change, it must fail.
	macro->items().first()->variable("teststring")->setVariant("bla");
	//KOMACROTEST_XASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);

	delete doomdocument; // TODO delete domelement; ??
}

// 2.Test - XML-document with bad root element.
void XMLHandlerTests::tpxTestBadRoot()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<maro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</maro>");
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	KOMACROTEST_XASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_XASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);

	delete doomdocument;
}

// 3.Test - XML-document with a missing Variable.
void XMLHandlerTests::tpxTestMissingVariable()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);

	delete doomdocument;
}

// 4.Test - One more Variable in XML-Document.
void XMLHandlerTests::tpxTestMoreVariables()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
						"<variable name=\"testbla\" >somethingwrong</variable>"
				      "</item>"
				    "</macro>");
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);

	delete doomdocument;
}

// 5.Test - XML-document with wrong macro-xmlversion.
void XMLHandlerTests::tpxTestWrongVersion()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"2\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	KOMACROTEST_XASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_XASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);

	delete doomdocument;
}

// 6.Test - XML-document if it has a wrong structure like wrong parathesis
// 	or missing end tag. TODO is this critical??
void XMLHandlerTests::tpxTestWrongXMLStruct()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
				    "</macro>");
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);
	KOMACROTEST_ASSERT(doomdocument->isDocument(),true);
	KOMACROTEST_ASSERT(domelement.isElement(),true);

	delete doomdocument;
}

// 7.Test - XML-document with wrong item- and variable-tags.
// 	TODO Could this happen?? It could, but concerning to high performance
// 	we accept that the tags maybe wrong.
void XMLHandlerTests::tpxTestWrongItemVarTags()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<iem action=\"testaction\" >"
    					"<vle name=\"teststring\" >testString</variable>"
						"<v name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	KOMACROTEST_XASSERT(macro->parseXML(domelement),true); 

	delete doomdocument;
}

// 8.Test-XML-document with maximum field-size.
void XMLHandlerTests::tpxTestMaxNum()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MAX).arg(DBL_MAX);
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);

	delete doomdocument;
}

// 9.Test-XML-document with maximum+1 field-size.
void XMLHandlerTests::tpxTestMaxNum2()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MAX+1).arg(DBL_MAX+1);
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);

	delete doomdocument;
}

// 10.Test-XML-document with minimum field-size.
void XMLHandlerTests::tpxTestMinNum()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MIN).arg(DBL_MIN);
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);

	delete doomdocument;
}

// 11.Test-XML-document with minimum+1 field-size.
void XMLHandlerTests::tpxTestMinNum2()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MIN+1).arg(DBL_MIN+1);
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);

	delete doomdocument;
}

// 12.Test - With a to big number.
void XMLHandlerTests::tpxTestBigNumber()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument* doomdocument = new QDomDocument();
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > 0123456789012345678901234567890123456789 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %1 </variable>"
				      "</item>"
				    "</macro>").arg(DBL_MAX+1);
	doomdocument->setContent(xml);
	domelement = doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	assertMacroContentEqToXML(macro,domelement);

	delete doomdocument;
}
/***************************************************************************
* End of Sub-methos of testParseXML().
***************************************************************************/

// Compares a XML-Element with a Macro by value.
void XMLHandlerTests::assertMacroContentEqToXML(const KSharedPtr<KoMacro::Macro> macro, const QDomElement& domelement)
{	
	// To handle control flow.
	// TODO: We should also can do this by return-statement of the sub-asserts.
	bool proceed = true;

	// Make an Iterator over the MacroItems of the Macro.
	const QValueList<KSharedPtr<KoMacro::MacroItem > > macroitems = macro->items();
	QValueList<KSharedPtr<KoMacro::MacroItem > >::ConstIterator 
		mit(macroitems.constBegin()), end(macroitems.constEnd());
	//1. comparison - Is the MacroItem-list empty?
{
	assertItemsNotEmpty(macroitems, true, & proceed);
	if( ! proceed) return;
}
	// Got to the first item-elements of the domelement (there is only one in the tests).
	QDomNode itemnode = domelement.firstChild();

	// Iterate over the MacroItems and item-elements.
	while(mit != end && ! itemnode.isNull()){
		const KoMacro::MacroItem* macroitem = *mit;
		const QDomElement itemelem = itemnode.toElement();
		//Is the Action-name equal?
		if(macroitem->action()->name() != itemelem.attribute("action")) {
			kdDebug() 	<< "Action-name not equal: " 
						<< macroitem->action()->name()
						<< " != " << itemelem.attribute("action") << endl;
			return; //return false;
		}

		// Go down to MacroItem->Variable and item->variable and compare them.
		QMap<QString, KSharedPtr<KoMacro::Variable > > mvariables = macroitem->variables();
		//printMvariables(mvariables,"before");
		QDomNode varnode = itemelem.firstChild();

		while ( ! varnode.isNull()) {
			const QDomElement varelem = varnode.toElement();
			
			const KoMacro::Variable* varitem = mvariables.find(varelem.attribute("name")).data();
			
			if(varitem->name() == "testint") kdDebug() << "var-int: " << varitem->variant() << endl;
			// Compare the contents.
			if ( varitem->variant() != QVariant(varelem.text()) ) {
				kdDebug() 	<< "The content of the Variable: " << varitem->name() 
							<< " is not equal." << varitem->variant()
							<< "!=" << varelem.text() << endl;
				// TODO Leave test the type bool because of parsing problems.
				//if(varitem->name() == "testbool") {
				//	kdDebug() << "It's a bool, left checking of it. " 
				//			<< QVariant(varitem->variant()) << endl;
				//}
				return; // return false;
			}
			//printMvariables(mvariables,"doing");
			mvariables.erase(varitem->name());

			varnode = varnode.nextSibling();
		}
		// TODO Should I compare here with the Variables of the TestAction??
		if ( ! mvariables.empty()) {
			kdDebug() << "There are non-filled variable in the MacroItem: " << mvariables.count() <<endl;
			return; //return false;
		}
		//printMvariables(mvariables,"after");
		
		// Go to next MacroItem and next item-element.
		mit++;
		itemnode = itemnode.nextSibling();
	}

	//return true;
}

//1. comparison - Is the MacroItem-list empty?
void XMLHandlerTests::assertItemsNotEmpty(const QValueList<KSharedPtr<KoMacro::MacroItem > > macroitems, bool expect, bool proceed)
{
	KOMACROTEST_ASSERT(macroitems.empty(),expect);
	proceed != macroitems.empty();
}

// Prints a QMap of Variables to kdDebug().
void XMLHandlerTests::printMvariables(QMap<QString, KSharedPtr<KoMacro::Variable > > mvariables, QString s)
{
	//QValueList<QString>::ConstIterator kit (keys.constBegin()), end(keys.constEnd());
	QMap<QString, KSharedPtr<KoMacro::Variable > >::ConstIterator mvit (mvariables.constBegin()), end(mvariables.constEnd());
	while(mvit != end){
		KoMacro::Variable * v = *mvit;
		kdDebug() << s << ": " << v->name() << endl;
		mvit++;
	}
}

/**
* Test the @a KoMacro::XMLHandler toXML()-function.
*/
void XMLHandlerTests::testToXML()
{	
	kdDebug()<<"===================== testToXML() ======================" << endl;
	// TODO Part 2: From a Macro to XML.
	
	// Init requiered testobject.
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	KSharedPtr<KoMacro::MacroItem> macroitem = new KoMacro::MacroItem();
	KSharedPtr<KoMacro::Action> testaction = new TestAction();
	macroitem->setAction(testaction);
	macro->addItem(macroitem);

	// First Test.
	QDomElement domelement = macro->toXML();
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
}
#include "xmlhandlertests.moc"
