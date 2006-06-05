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

	d->doomdocument = new QDomDocument();
}

void XMLHandlerTests::tearDown()
{
	delete d->doomdocument;
	delete d->xmlguiclient;
}

/**
* Test the @a KoMacro::XMLHandler parseXML()-function.
*/
void XMLHandlerTests::testParseXML()
{
	kdDebug()<<"===================== testParseXML() ======================" << endl;

	// Local Init
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomElement domelement;

	// Part 1: From XML to a Macro.
	// Test-XML-document with normal allocated variables.
	QString xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testbla\" >somethingwrong</variable>" // TODO Is here a kdDebug-msg enough?
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	// Set the XML-document with the above string.
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	// Is our XML parseable ?
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	// Is the parsen content in the Macro correct ?
	KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);

	// Test-XML-document with bad root element.
	xml = QString("<!DOCTYPE macros>"
				    "<maro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</maro>");
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_XASSERT(macro->parseXML(domelement),true);
	macro->clearItems();
	KOMACROTEST_XASSERT(isMacroContentEqToXML(macro,domelement),true);

	// Test-XML-document with a missing Variable.
	xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//macro->clearItems();
	KOMACROTEST_ASSERT(isMacroContentEqToXML(macro,domelement),true);

	// Test-XML-document with wrong macro-xmlversion.
	xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"2\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),false);

	// Test-XML-document with wrong macro-xmlversion - 2 .
	xml = QString("<!DOCTYPE macros>"
				    "<macro sion=\"2\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),false);

	// TODO Test-XML-document if it has a wrong structure like wrong parathesis
	// or missing end tag (is this critical??).
	xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
				    "</macro>");
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);
	//KOMACROTEST_ASSERT(d->doomdocument->isDocument(),true);
	//KOMACROTEST_ASSERT(domelement.isElement(),true);

	/*// Test-XML-document with wrong item- and variable-tags.
	// Could this happen?? It could, but concerning to high performance
	// we accept that the tags maybe wrong.
	xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<iem action=\"testaction\" >"
    					"<vle name=\"teststring\" >testString</variable>"
						"<v name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>"
				      "</item>"
				    "</macro>");
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),false); 
	// is true, because there is no syntax-parsing for item and variable. */

	// Test-XML-document with maximum field-size.
	xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MAX).arg(DBL_MAX);
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);

	// Test-XML-document with minimum field-size.
	xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MIN).arg(DBL_MIN);
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);

	// TODO Test-XML-document with maximum +1 field-size.
	// Test doesn't works because INT_MAX+1 jumps to INT_MIN
	xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MAX+1).arg(DBL_MAX+1);
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);

	// TODO Test-XML-document with minimum-1 field-size.
	// Test doesn't works because INT_MIN-1 jumps to INT_MAX
	xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > %1 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %2 </variable>"
				      "</item>"
				    "</macro>").arg(INT_MIN-1).arg(DBL_MIN-1);
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);

	// Test with a to big number.
	// TODO Should this big number is parsen correct?
	xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > 5555555555555555555555555555555555555555555555555 </variable>"
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" > %1 </variable>"
				      "</item>"
				    "</macro>").arg(DBL_MAX+1);
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_XASSERT(macro->parseXML(domelement),true);
	//TODO kdDebug() << macro->items.first().variable("testint")->variant() << endl;
}

// Compares a XML-Element with a Macro by value.
// TODO Should I compare the types or is this done by QVariant?
bool XMLHandlerTests::isMacroContentEqToXML(const KSharedPtr<KoMacro::Macro> macro, const QDomElement& domelement)
{	
	// Make an Iterator over the MacroItems of the Macro.
	const QValueList<KSharedPtr<KoMacro::MacroItem > > macroitems = macro->items();
	QValueList<KSharedPtr<KoMacro::MacroItem > >::ConstIterator mit(macroitems.constBegin()), end(macroitems.constEnd());
	if(macroitems.empty()) return false;

	// Make an Iterator over the item-elements of the domelement.
	QDomNode itemnode = domelement.firstChild();

	// Iterate over the MacroItems and item-elements.
	while(mit != end && ! itemnode.isNull()){
		const KoMacro::MacroItem* macroitem = *mit;
		const QDomElement itemelem = itemnode.toElement();
		//Is the Action-name equal?
		if(macroitem->action()->name() != itemelem.attribute("action")) {
			kdDebug() << "Action-name not equal: " << macroitem->action()->name() << " != " << itemelem.attribute("action") << endl;
			return false;
		}

		/* Output for keys in Variable-map
		QValueList<QString> keys = mvariables.keys();
		QValueList<QString>::ConstIterator kit (keys.constBegin()), end(keys.constEnd());

		while ( kit != end) {
			//const QString* k = *kit;
			kdDebug() << *kit << endl;
			kit++;
		}*/

		// o down to MacroItem->Variable and item->variable and compare them.
		QMap<QString, KSharedPtr<KoMacro::Variable > > mvariables = macroitem->variables();
		QDomNode varnode = itemelem.firstChild();

		while ( ! varnode.isNull()) {
			const QDomElement varelem = varnode.toElement();
			
			const KoMacro::Variable* varitem = mvariables.find(varelem.attribute("name")).data();
			//if ( ! *varitem ) kdDebug() << "BBBBBBBBBBBThere are more variable-elements in the XML: " << mvariables.find(varelem.attribute("name")).key() << endl;

			// TODO Compare the contents.
			//if ( varitem->variant != QVariant(varelem.text())) kdDebug() << "The content of the Variable is not equal." << endl;
			mvariables.erase(varitem->name());
			
			// TODO Is it true that a Macroitem saves all parsen Variables also unknown???


			varnode = varnode.nextSibling();
		}
		// TODO Should I compare here with the Variables of the TestAction??
		// if ( ! mvariables.empty()) kdDebug() << "MMMMMMMThere are non-filled variable in the MacroItem: " << mvariables.count() <<endl;
		
		// Go to next MacroItem and next item-element.
		mit++;
		itemnode = itemnode.nextSibling();
	}

	return true;
}

/**
* Test the @a KoMacro::XMLHandler toXML()-function.
*/
void XMLHandlerTests::testToXML()
{	
	kdDebug()<<"===================== testToXML() ======================" << endl;
	// TODO Part 2: From a Macro to XML.

}
#include "xmlhandlertests.moc"
