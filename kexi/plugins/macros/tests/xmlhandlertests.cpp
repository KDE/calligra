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

// TODO: 	- test correct XML-tags once
//			- look at const
//			- compile error, begins with:
/*In file included from /usr/lib/qt3/include/qconnection.h:74,
                 from /usr/lib/qt3/include/qmetaobject.h:42,
                 from /home/bernd/swp/koffice/kexi/plugins/macros/tests/xmlhandlertests.moc:12,
                 from /home/bernd/swp/koffice/kexi/plugins/macros/tests/xmlhandlertests.cpp:593:
/usr/lib/qt3/include/qwinexport.h:221:1: error: unterminated argument list invoking macro "KOMACROTEST_ASSERT"
/usr/lib/qt3/include/qwinexport.h: In member function ‘void KoMacroTest::XMLHandlerTests::assertMacroContentEqToXML(KSharedPtr<KoMacro::Macro>, const QDomElement&, bool, bool, QMap<QString, bool>)’:
/usr/lib/qt3/include/qwinexport.h:1: error: ‘KOMACROTEST_ASSERT’ was not declared in this scope
/usr/lib/qt3/include/qmetaobject.h:50: error: expected `;' before ‘class’
/usr/lib/qt3/include/qmetaobject.h:129: error: a function-definition is not allowed here before ‘{’ token
/usr/lib/qt3/include/qmetaobject.h:258: error: a function-definition is not allowed here before �{’ token
/usr/lib/qt3/include/qmetaobject.h:261: error: a function-definition is not allowed here before ‘{’ token
/usr/lib/qt3/include/qmetaobject.h:265: error: a function-definition is not allowed here before ‘{’ token
*/

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

	// 1.Test - Correct DomElement 	- PASS.
	testCorrectDomElement();		
	testBadRoot();
	testMissingVariable();
	testMoreVariables();
	testWrongVersion();
	//testWrongXMLStruct();
	//testWrongItemVarTags();
	testMaxNum();
	testMaxNum2();
	testMinNum();
	testMinNum2();
	testBigNumber();
	testTwoMacroItems();
}

/***************************************************************************
* Begin of Sub-methos of testParseXML().
***************************************************************************/

// 1.Test - Correct DomElement	- PASS.
void XMLHandlerTests::testCorrectDomElement()
{
	// Local Init
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
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
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	// Is our XML parseable ?
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	// Is the parsen content in the Macro correct ?
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	QMap<QString,bool> isvariableok;
	isvariableok["teststring"] = true;	// TODO nicer?
	isvariableok["testint"] = true;
	isvariableok["testbool"] = true;
	isvariableok["testdouble"] = true;
	assertMacroContentEqToXML(macro,domelement,false,true,isvariableok);
	// Test the Compare-method when a Variable will change, it must fail.
	macro->items().first()->variable("teststring")->setVariant("bla");
	//KOMACROTEST_XASSERT(isMacroContentEqToXML(macro,domelement),true);
	//assertMacroContentEqToXML(macro,domelement,false,true,true);
}

// 2.Test - XML-document with bad root element.
void XMLHandlerTests::testBadRoot()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
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
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	KOMACROTEST_XASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_XASSERT(isMacroContentEqToXML(macro,domelement),true);
	//assertMacroContentEqToXML(macro,domelement);
}

// 3.Test - XML-document with a missing Variable.
void XMLHandlerTests::testMissingVariable()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	//assertMacroContentEqToXML(macro,domelement);
}

// 4.Test - One more Variable in XML-Document.
void XMLHandlerTests::testMoreVariables()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
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
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	//assertMacroContentEqToXML(macro,domelement);
}

// 5.Test - XML-document with wrong macro-xmlversion.
void XMLHandlerTests::testWrongVersion()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
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
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	KOMACROTEST_XASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_XASSERT(isMacroContentEqToXML(macro,domelement),true);
	//assertMacroContentEqToXML(macro,domelement);
}

// 6.Test - XML-document if it has a wrong structure like wrong parathesis
// 	or missing end tag. TODO is this critical??
void XMLHandlerTests::testWrongXMLStruct()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
				    "</macro>");
	// Test setting of the QDomDocument, but we get a QDomElement...
	KOMACROTEST_XASSERT(doomdocument.setContent(xml),true);
	domelement = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	//assertMacroContentEqToXML(macro,domelement);
	KOMACROTEST_ASSERT(doomdocument.isDocument(),true);
	KOMACROTEST_ASSERT(domelement.isElement(),true);
}

// 7.Test - XML-document with wrong item- and variable-tags.
// 	TODO Could this happen?? It could, but concerning to high performance
// 	we accept that the tags maybe wrong.
void XMLHandlerTests::testWrongItemVarTags()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
	QDomElement domelement;

	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<iem action=\"testaction\" >"
    					"<vle name=\"teststring\" >testString</variable>"
						"<v name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</iem>"
				    "</macro>");
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	KOMACROTEST_XASSERT(macro->parseXML(domelement),true);
}

// 8.Test-XML-document with maximum field-size.
void XMLHandlerTests::testMaxNum()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
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
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	//assertMacroContentEqToXML(macro,domelement);
}

// 9.Test-XML-document with maximum+1 field-size.
void XMLHandlerTests::testMaxNum2()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
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
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	//assertMacroContentEqToXML(macro,domelement);
}

// 10.Test-XML-document with minimum field-size.
void XMLHandlerTests::testMinNum()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
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
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	//assertMacroContentEqToXML(macro,domelement);
}

// 11.Test-XML-document with minimum+1 field-size.
void XMLHandlerTests::testMinNum2()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
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
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	//assertMacroContentEqToXML(macro,domelement);
}

// 12.Test - With a to big number.
void XMLHandlerTests::testBigNumber()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
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
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	////assertMacroContentEqToXML(macro,domelement);
}

// 13.Test - With two MacroItems.
void XMLHandlerTests::testTwoMacroItems()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;
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
					"<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString2</variable>"
						"<variable name=\"testint\" >4</variable>"
						"<variable name=\"testbool\" >false</variable>"
						"<variable name=\"testdouble\" >0.7</variable>"
						"<variable name=\"testbla\" >somethingwrong2</variable>"
				      "</item>"
				    "</macro>");
	doomdocument.setContent(xml);
	domelement = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);
	//assertMacroContentEqToXML(macro,domelement);
}
/***************************************************************************
* End of Sub-methos of testParseXML().
***************************************************************************/

/**
* Test the @a KoMacro::XMLHandler toXML()-function.
*/
void XMLHandlerTests::testToXML()
{	
	kdDebug()<<"===================== testToXML() ======================" << endl;
	// TODO Part 2: From a Macro to XML. Some little tests.
	
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

// Compares a XML-Element with a Macro by value.
void XMLHandlerTests::assertMacroContentEqToXML(const KSharedPtr<KoMacro::Macro> macro,
	const QDomElement& domelement,
	bool isitemsempty, bool isactionset, QMap<QString, bool> isvariableok)
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

	// Got to the first item-elements of the domelement (there is only one in the tests).
	QDomNode itemnode = domelement.firstChild();

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
				KOMACROTEST_ASSERT((macroitem->action()->name() == itemelem.attribute("action"),true);
			}
		}

		// Go down to MacroItem->Variable and item->variable and compare them.
		QMap<QString, KSharedPtr<KoMacro::Variable > > mvariables = macroitem->variables();
		QDomNode varnode = itemelem.firstChild();

		//printMvariables(mvariables,"before");

		while ( ! varnode.isNull()) {
			const QDomElement varelem = varnode.toElement();
			const KSharedPtr<KoMacro::Variable> varitem = mvariables.find(varelem.attribute("name")).data();
			
			//3.comparison - Is the content of the Variable
			// in the MacroItem and and item equal?
			{
				if( ! isvariableok ) {
					KOMACROTEST_XASSERT(varitem->variant() == QVariant(varelem.text()),
						isvariableok.find(varelem.attribute("name")));
					kdDebug() 	<< "The content of the Variable: " << varitem->name() 
								<< " is not equal." << varitem->variant()
								<< "!=" << varelem.text() << endl;
					return;
				}
				else {
					KOMACRO_ASSERT(varitem->variant() == QVariant(varelem.text()),
						isvariableok.find(varelem.attribute("name")));
				}
			}
			//printMvariables(mvariables,"doing");

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
		//printMvariables(mvariables,"after");
		
		// Go to next MacroItem and next item-element.
		mit++;
		itemnode = itemnode.nextSibling();
	}

	//return true;
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

#include "xmlhandlertests.moc"
