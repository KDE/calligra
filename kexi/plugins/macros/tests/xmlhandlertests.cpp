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

// TODO: 	- default in asser...()

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
* Test the @a KoMacro::XMLHandler parseXML() and toXML()-function.
*/
void XMLHandlerTests::testParseAndToXML()
{
	kdDebug()<<"===================== testParseAndToXML() ======================" << endl;

	// 1.Test - Correct DomElement.
	testCorrectDomElement();
	// 2.Test - XML-document with bad root element.
	testBadRoot();
	// 3.Test - XML-document with a missing Variable.
	testMissingVariable();
	// 4.Test - One more Variable in XML-Document.
	testMoreVariables();
	// 5.Test - XML-document with wrong macro-xmlversion.
	testWrongVersion();
	// 6.Test - XML-document if it has a wrong structure like wrong parathesis
	// 	or missing end tag.
	testWrongXMLStruct();
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
	// 12.Test - With two MacroItems.
	testTwoMacroItems();
}

/***************************************************************************
* Begin of Sub-methos of testParseXML().
***************************************************************************/
// 1.Test - Correct DomElement.
void XMLHandlerTests::testCorrectDomElement()
{
	// Local Init
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	const QString xml = QString("<!DOCTYPE macros>"
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
	const QDomElement elem = doomdocument.documentElement();
	// Is our XML parseable by calling parseXML()?
	KOMACROTEST_ASSERT(macro->parseXML(elem),true);
	
	// Is the parsen content in the Macro correct ?
	QMap<QString,bool> isvariableok;
	isvariableok["teststring"] = true;
	isvariableok["testint"] = true;
	isvariableok["testbool"] = true;
	isvariableok["testdouble"] = true;
	assertMacroContentEqToXML(macro,elem,false,true,isvariableok);
	
	// Transform back by calling toXML().
	const QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,false,true,isvariableok);

	// Test the Compare-method when a Variable will change, it must fail.
	macro->items().first()->variable("teststring")->setVariant("bla");
	isvariableok.replace("teststring",false);
	assertMacroContentEqToXML(macro,elem,false,true,isvariableok);

	// Example to do the 1.comparison in the test-methods.
	// KOMACROTEST_XASSERT(macro->items().empty(),false);
}

// 2.Test - XML-document with bad root element.
void XMLHandlerTests::testBadRoot()
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

// 3.Test - XML-document with a missing Variable.
void XMLHandlerTests::testMissingVariable()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();	
	KOMACROTEST_ASSERT(macro->parseXML(elem),true);

	QMap<QString,bool> isvariableok;
	isvariableok["teststring"] = true;
	isvariableok["testint"] = true;
	isvariableok["testdouble"] = true;
	assertMacroContentEqToXML(macro,elem,false,true,isvariableok);
	
	const QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,false,true,isvariableok);
}

// 4.Test - One more Variable in XML-Document.
void XMLHandlerTests::testMoreVariables()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	const QString xml = QString("<!DOCTYPE macros>"
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
	const QDomElement elem = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(elem),true);

	QMap<QString,bool> isvariableok;
	isvariableok["teststring"] = true;
	isvariableok["testint"] = true;
	isvariableok["testbool"] = true;
	isvariableok["testdouble"] = true;
	isvariableok["testbla"] = true;
	assertMacroContentEqToXML(macro,elem,false,true,isvariableok);
	
	const QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,false,true,isvariableok);
}

// 5.Test - XML-document with wrong macro-xmlversion.
void XMLHandlerTests::testWrongVersion()
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
void XMLHandlerTests::testWrongXMLStruct()
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

// 7.Test-XML-document with maximum field-size.
void XMLHandlerTests::testMaxNum()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MAX).arg(DBL_MAX);
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(elem),true);

	QMap<QString,bool> isvariableok;
	isvariableok["teststring"] = true;
	isvariableok["testint"] = true;
	isvariableok["testbool"] = true;
	isvariableok["testdouble"] = true;
	assertMacroContentEqToXML(macro,elem,false,true,isvariableok);
	
	QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,false,true,isvariableok);
}

// 8.Test-XML-document with maximum+1 field-size.
void XMLHandlerTests::testMaxNum2()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MAX+1).arg(DBL_MAX+1);
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(elem),true);

	QMap<QString,bool> isvariableok;
	isvariableok["teststring"] = true;
	isvariableok["testint"] = true;
	isvariableok["testbool"] = true;
	isvariableok["testdouble"] = true;
	assertMacroContentEqToXML(macro,elem,false,true,isvariableok);
	
	const QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,false,true,isvariableok);
}

// 9.Test-XML-document with minimum field-size.
void XMLHandlerTests::testMinNum()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MIN).arg(DBL_MIN);
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(elem),true);

	QMap<QString,bool> isvariableok;
	isvariableok["teststring"] = true;
	isvariableok["testint"] = true;
	isvariableok["testbool"] = true;
	isvariableok["testdouble"] = true;
	assertMacroContentEqToXML(macro,elem,false,true,isvariableok);
	
	const QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,false,true,isvariableok);
}

// 10.Test-XML-document with minimum+1 field-size.
void XMLHandlerTests::testMinNum2()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MIN-1).arg(DBL_MIN-1);
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(elem),true);

	QMap<QString,bool> isvariableok;
	isvariableok["teststring"] = true;
	isvariableok["testint"] = true;
	isvariableok["testbool"] = true;
	isvariableok["testdouble"] = true;
	assertMacroContentEqToXML(macro,elem,false,true,isvariableok);
	
	const QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,false,true,isvariableok);
}

// 11.Test - With a to big number.
void XMLHandlerTests::testBigNumber()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	const QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > 0123456789012345678901234567890123456789 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %1 </variable>"
				      "</item>"
				    "</macro>").arg(DBL_MAX+1);
	doomdocument.setContent(xml);
	const QDomElement elem = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(elem),true);

	QMap<QString,bool> isvariableok;
	isvariableok["teststring"] = true;
	isvariableok["testint"] = true;
	isvariableok["testbool"] = true;
	isvariableok["testdouble"] = true;
	assertMacroContentEqToXML(macro,elem,false,true,isvariableok);
	
	const QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,false,true,isvariableok);
}

// 12.Test - With two MacroItems.
void XMLHandlerTests::testTwoMacroItems()
{
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomDocument doomdocument;

	const QString xml = QString("<!DOCTYPE macros>"
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
	const QDomElement elem = doomdocument.documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(elem),true);

	QMap<QString,bool> isvariableok;
	isvariableok["teststring"] = true;
	isvariableok["testint"] = true;
	isvariableok["testbool"] = true;
	isvariableok["testdouble"] = true;
	assertMacroContentEqToXML(macro,elem,false,true,isvariableok);
	
	const QDomElement elem2 = macro->toXML();
	assertMacroContentEqToXML(macro,elem2,false,true,isvariableok);
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
void XMLHandlerTests::assertMacroContentEqToXML(const KSharedPtr<KoMacro::Macro> macro,
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
void XMLHandlerTests::printMvariables(const QMap<QString, KSharedPtr<KoMacro::Variable > > mvariables, const QString s)
{
	//QValueList<QString>::ConstIterator kit (keys.constBegin()), end(keys.constEnd());
	QMap<QString, KSharedPtr<KoMacro::Variable > >::ConstIterator mvit (mvariables.constBegin()), end(mvariables.constEnd());
	while(mvit != end){
		const KoMacro::Variable * v = *mvit;
		kdDebug() << s << ": " << v->name() << endl;
		mvit++;
	}
}

#include "xmlhandlertests.moc"
