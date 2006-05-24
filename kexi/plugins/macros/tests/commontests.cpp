/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2005 by Tobi Krebs (tobi.krebs@gmail.com)
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

#include "commontests.h"
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
#include <climits>

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
	KUNITTEST_SUITE("CommonTestsSuite")
	KUNITTEST_REGISTER_TESTER(CommonTests);

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class CommonTests::Private
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

			/**
			* Constructor.
			*/
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

CommonTests::CommonTests()
	: KUnitTest::SlotTester()
	, d( new Private() ) // create the private d-pointer instance.
{
}

CommonTests::~CommonTests()
{
	delete d->xmlguiclient;
	delete d;
}

void CommonTests::setUp()
{
	d->xmlguiclient = new KXMLGUIClient();
	::KoMacro::Manager::init( d->xmlguiclient );

	d->testobject = new TestObject( this );
	::KoMacro::Manager::self()->publishObject("TestObject", d->testobject);
	
	d->testaction = new TestAction();
	::KoMacro::Manager::self()->publishAction(d->testaction);	

	d->doomdocument = new QDomDocument();

	QString const xml = QString("<!DOCTYPE macros>"

				    "<macro xmlversion=\"1\">"

				      "<item action=\"testaction\" >"
				      "</item>"
				    "</macro>");
	
	d->doomdocument->setContent(xml);
}

void CommonTests::tearDown()
{
	delete d->doomdocument;
	delete d->testobject;
	delete d->xmlguiclient;
}

void CommonTests::testManager()
{
	kdDebug()<<"===================== testManager() ======================" << endl;

	//check if manager-guiClient equals xmlguiclient
	KOMACROTEST_ASSERT( ::KoMacro::Manager::self()->guiClient(), d->xmlguiclient );
	//check if manger-object equals testobject
	KOMACROTEST_ASSERT( dynamic_cast<TestObject*>( (QObject*)::KoMacro::Manager::self()->object("TestObject") ), d->testobject );
}
/*
void CommonTests::testAction()
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
	//fetch the first one
	KSharedPtr<KoMacro::Action> actionptr = items[0]->action();
	//How do we know that an action exist ?	
	//-> check that it is not null
	KOMACROTEST_XASSERT(sizetype(actionptr.data()), sizetype(0));
	//fetch the "teststring"-variable
	KSharedPtr<KoMacro::Variable> variableptr = actionptr->variable("teststring");
	//check that it is not null
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	//check that it is " "
	KOMACROTEST_ASSERT(variableptr->variant().toString(),QString("testString"));

	//fetch the "testint"-variable
	variableptr = actionptr->variable("testint");
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
	
	actionptr->setVariable("testint","INTTEST",INT_MAX);
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
		
	macroitem->setVariable("testint",INT_MIN);
	variableptr = macroitem->variable("testint");
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(INT_MIN));
	
	macroitem->setVariable("testint",-1);
	variableptr = macroitem->variable("testint");
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(-1));
	
	
	//commontests.cpp: In member function 'void KoMacroTest::CommonTests::testAction()':
	//commontests.cpp:249: error: call of overloaded 'setVariable(const char [8], int)' is ambiguous
	//../lib/macroitem.h:131: note: candidates are: QStringList KoMacro::MacroItem::setVariable(const QString&, KoMacro::Variable::Ptr)
	//../lib/macroitem.h:137: note:                 QStringList KoMacro::MacroItem::setVariable(const QString&, const QVariant&)

	macroitem->setVariable("testint",(int) 0);
	variableptr = macroitem->variable("testint");
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(0));

	
	macroitem->setVariable("testint",1);
	variableptr = macroitem->variable("testint");
	KOMACROTEST_XASSERT(sizetype(variableptr.data()), sizetype(0));
	KOMACROTEST_ASSERT(sizetype(variableptr->variant().toInt()),sizetype(1));
	
	macroitem->setVariable("testint",INT_MAX);
	variableptr = macroitem->variable("testint");
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
} */

void CommonTests::testXmlhandler()
{
	kdDebug()<<"===================== testXmlhandler() ======================" << endl;

	// Local Init
	KoMacro::Macro::Ptr macro = KoMacro::Manager::self()->createMacro("testMacro");
	QDomElement domelement;

	// Save old doomdocument
	QString xmlOld = d->doomdocument->toString();

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
	//Is our XML parseable ?
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);

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
	KOMACROTEST_ASSERT(macro->parseXML(domelement),false);

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

	// TODO Test-XML-document if it has a wrong structure like wrong parathesis
	// or missing end tag (is this critical??).
	/*xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>"
						"<variable name=\"testbool\" >true</variable>"
				      "</item>"
				    "</macro>");
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),false);*/

	// Test-XML-document with wrong item- and variable-tags.
	// TODO Could this happen??
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
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);  //should be false?

	// TODO Test-XML-document with maximum field-size.
	xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" > 0 </variable>" // the value should be INT_MAX
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>" // DBL_MAX
				      "</item>"
				    "</macro>");
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);

	// TODO Test-XML-document with minimum field-size.
	xml = QString("<!DOCTYPE macros>"
				    "<macro xmlversion=\"1\">"
				      "<item action=\"testaction\" >"
    					"<variable name=\"teststring\" >testString</variable>"
						"<variable name=\"testint\" >0</variable>" // INT_MIN
						"<variable name=\"testbool\" >true</variable>"
						"<variable name=\"testdouble\" >0.6</variable>" // DBL_MIN
				      "</item>"
				    "</macro>");
	d->doomdocument->setContent(xml);
	domelement = d->doomdocument->documentElement();
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);

	// TODO Part 2: Read the parsen macro and make a comparison to the XML-document.

	// TODO Part 3: From a Macro to XML.

	// RODO Part 4: Compare the transformed XML with the given macro.

	// Set back xml-string for other tests.
	d->doomdocument->setContent(xmlOld);	
}

void CommonTests::testFunction()
{
//TODO: CLEANUP!!!!!!	
/*
	kdDebug()<<"===================== testFunction() ======================" << endl;
	
	//create a QDomDocument	
	QDomDocument domdocument = QDomDocument();
	//create some data
	QString	const comment = "Describe what the function does";
	QString const name = "myfunc";
	QString const text = "My Function";
	QString const receiver = "TestObject";
	QString const argument1 = "Some string";
	int const argument2 = 12345;

	//set "Function"-content in QDocument	
	domdocument.setContent(QString(
		"<function name=\"" + name + "\" text=\"" + text + "\" comment=\"" + comment + "\" receiver=\"" + receiver + "\" slot=\"myslot(const QString &amp; , int)\">"
			"<argument>" + argument1 + "</argument>"
			"<argument>" + QString("%1").arg(argument2) + "</argument>"
		"</function>"
	));

	//create an KomacroFunction  with our data, and put it into a KSharedPtr
	KoMacro::Action::Ptr functionptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//cast KSharedPtr to KoMacro-"Function"
	KoMacro::Function* func = dynamic_cast<KoMacro::Function*>( functionptr.data() );
		//check that function is not null
	KOMACROTEST_XASSERT((int) func, 0);

	//check domElement
	KOMACROTEST_ASSERT( func->domElement(), domdocument.documentElement() );
	//check name
	KOMACROTEST_ASSERT( QString(func->name()), name );
	//check text
	KOMACROTEST_ASSERT( func->text(), text );
		//check comment
	KOMACROTEST_ASSERT( func->comment(), comment );
	//check receiver
	KOMACROTEST_ASSERT( func->receiver(), receiver );
	//check slot (arguments)
	KOMACROTEST_ASSERT( QString(func->slot()), QString("myslot(const QString&,int)") );

	//create KoMacro-MetaObject from receiverObject
	KoMacro::MetaObject::Ptr receivermetaobject = func->receiverObject();
	//check that receivermetaobject.data is not null
	KOMACROTEST_XASSERT((int) receivermetaobject.data(), 0);
	
	//create KoMacro-MetaMethod from receiverObject
	KoMacro::MetaMethod::Ptr receivermetamethod = receivermetaobject->slot( func->slot().latin1() );
	//check that receivermetamethod.data is not null
	KOMACROTEST_XASSERT((int) receivermetamethod.data(), 0);

	//create list  of variables from func
	KoMacro::Variable::List funcvariables = func->variables();
	//counter for hardcoded tests see below ...
	uint i = 0;
	KoMacro::Variable::List::ConstIterator it, end( funcvariables.constEnd() );
	for( it = funcvariables.constBegin(); it != end; ++it) {
		kdDebug() << "VARIABLE => " << (*it ? (*it)->toString() : "<NULL>") << endl;
		//hardcoded test:
		// firstrun we have a QString, secondrun we have an int
		switch(i) {
			case 0: { // returnvalue
				KOMACROTEST_ASSERT(*it, KoMacro::Variable::Ptr(NULL));
			} break;
			case 1: { // first parameter
				//check first variable of func is the same as argument1
				//QString const argument1 = "Some string";
				KOMACROTEST_ASSERT((*it)->toString(), argument1);
			} break;
			case 2: { // second parameter
				//check second variable of func is the same as argument2
				//int const argument2 = 12345;
				KOMACROTEST_ASSERT((*it)->toInt(), argument2);
			} break;
			default: {
			} break;
		}
		i++;
	}
	
	//check that we have two arguments + one returnvalue in func
	KOMACROTEST_ASSERT( funcvariables.count(), uint(3) );

	// check that the first argument (the returnvalue) is empty
	KOMACROTEST_ASSERT( funcvariables[0], KoMacro::Variable::Ptr(NULL) );
	
	//create a KoMacro-Variable-Ptr from first func argument
	KoMacro::Variable::Ptr stringvar = funcvariables[1];
	//check that it is not null
	KOMACROTEST_XASSERT((int) stringvar.data(),0);
	//check via QVariant type that stringvar is from Type Variant
	KOMACROTEST_ASSERT( stringvar->type(), KoMacro::MetaParameter::TypeVariant );
	//check via metaparameter that variant is from type string
	KOMACROTEST_ASSERT( stringvar->variantType(), QVariant::String );
	//chech that stringvar equals argument1
	KOMACROTEST_ASSERT( stringvar->toString(), argument1 );

	//create a KoMacro-Variable-Ptr from second func argument
	KoMacro::Variable::Ptr intvar = funcvariables[2];
	//check that it is not null	
	KOMACROTEST_XASSERT((int) intvar.data(), 0);
	//check via QVariant type that stringvar is from Type Variant
	KOMACROTEST_ASSERT( intvar->type(), KoMacro::MetaParameter::TypeVariant );
	//check that intvar is An String -> we create an string from int because of xml
	KOMACROTEST_ASSERT( intvar->variantType(), QVariant::String );
	//check that intvar equals argument2	
	KOMACROTEST_ASSERT( intvar->toInt(), argument2 );

	//returnvalue see testobject ....
	KoMacro::Variable::Ptr funcreturnvalue = receivermetamethod->invoke( funcvariables );
	kdDebug() << "CommonTests::testFunction() RETURNVALUE =====> " << funcreturnvalue->toString() << endl;
	KOMACROTEST_ASSERT( funcreturnvalue->toInt(), argument2 );

	//check returnvalue
	//func->setReturnValue(new KoMacro::Variable("54321"));
	//KOMACROTEST_ASSERT( func->returnValue()->toString(), QString("54321") );
*/
}

void CommonTests::testIntFunction() 
{
//TODO: CLEANUP!!!!!!	
/*
	kdDebug()<<"===================== testIntFunction() ======================" << endl;

	//create a QDomDocument	
	QDomDocument domdocument = QDomDocument();
	
	//set "Function"-content in QDocument	
	domdocument.setContent(QString(
		"<function name=\"myfunc\" text=\"My Function\" comment=\"comment\" receiver=\"TestObject\" slot=\"myslot(const QString &amp; , int)\">"
			"<argument>Some string</argument>"
			"<argument>12345</argument>"
		"</function>"
	));

	//create an KomacroFunction  with our data, and put it into a KSharedPtr
	KoMacro::Action::Ptr functionptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//Cast data to function
	KoMacro::Function* func = dynamic_cast<KoMacro::Function*>( functionptr.data() );
		//check that it is not null
	KOMACROTEST_XASSERT((int) func, 0);
	//execute the function 
	func->activate();
	//Check returnvalue is same value we entered
	//KOMACROTEST_ASSERT(func->returnValue()->toString(),QString("12345"));
*/
}

void CommonTests::testDoubleFunction() 
{
//TODO: CLEANUP!!!!!!	
/*
	kdDebug()<<"===================== testDoubleFunction() ======================" << endl;

	//create a QDomDocument		
	QDomDocument domdocument = QDomDocument();

	//set "Function"-content in QDocument	
	domdocument.setContent(QString(
		"<function name=\"myfunc\" text=\"My Function\" comment=\"comment\" receiver=\"TestObject\" slot=\"myslot(const QString &amp; , double)\">"
			"<argument>Some string</argument>"
			"<argument>12.56</argument>"
		"</function>"
	));

	//create an KomacroFunction  with our data, and put it into a KSharedPtr
	KoMacro::Action::Ptr functionptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//Cast data to function
	KoMacro::Function* func = dynamic_cast<KoMacro::Function*>( functionptr.data() );
		//check that it is not null
	KOMACROTEST_XASSERT((int) func, 0);
	//execute the function 	
	func->activate();
	//Check returnvalue is same value we entered
	//KOMACROTEST_ASSERT(func->returnValue()->toString(),QString("12.56"));
*/
}

void CommonTests::testQStringFunction() 
{
//TODO: CLEANUP!!!!!!	
/*
	kdDebug()<<"===================== testQStringFunction() ======================" << endl;

	//create a QDomDocument		
	QDomDocument domdocument = QDomDocument();
	
	//set "Function"-content in QDocument	
	domdocument.setContent(QString(
		"<function name=\"myfunc\" text=\"My Function\" comment=\"comment\" receiver=\"TestObject\" slot=\"myslot(const QString &amp;)\">"
			"<argument>Some string</argument>"
		"</function>"
	));

	//create an KomacroFunction with our data, and put it into a KSharedPtr
	KoMacro::Action::Ptr functionptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//Cast data to function
	KoMacro::Function* func = dynamic_cast<KoMacro::Function*>( functionptr.data() );
		//check that it is not null
	KOMACROTEST_XASSERT((int) func, 0);
	//execute the function		func->activate();
	//Check returnvalue is same value we entered
	//KOMACROTEST_ASSERT(func->returnValue()->toString(),QString("Some string"));
*/
}

void CommonTests::testMacro()
{
//TODO: CLEANUP!!!!!!	
	kdDebug()<<"===================== testMacro() ======================" << endl;

	QDomElement const domelement = d->doomdocument->documentElement();
	
	KoMacro::Macro::Ptr macro = KoMacro::Manager::self()->createMacro("testMacro");
	//Is our XML parseable ?
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);

// 	//create a QDomDocument		
// 	QDomDocument domdocument = QDomDocument();
// 
// 	//Fully fleged content this time with macro,function and action
// 	domdocument.setContent(QString(
// 		"<macro name=\"mymacro\" icon=\"myicon\" text=\"My Macro\" comment=\"Some comment to describe the Macro.\">"
// 			"<action name=\"myaction\" text=\"My Action\" comment=\"Just some comment\" />"
// 			"<function name=\"myfunc\" text=\"My Function\" comment=\"Describe what the function does\" receiver=\"TestObject\" slot=\"myslot(const QString &amp;)\">"
// 				"<argument>The myfunc argument string</argument>"
// 			"</function>"
// 		"</macro>"
// 	));
// 
// 	//create Macro
// // 	KoMacro::Action::Ptr macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
// 	//cast data to Macro
// 	KoMacro::Macro* macro = dynamic_cast<KoMacro::Macro*>( macroptr.data() );
	//check that it is not null
	KOMACROTEST_XASSERT(sizetype(macro.data()), sizetype(0));
	//check that domeElement given to manager is the sam as in the macro
//	KOMACROTEST_ASSERT( macro->toXML(), d->doomdocument->documentElement() );
	//check the name
	KOMACROTEST_ASSERT( QString(macro->name()), QString("testMacro") );
	
	/**
	 @deprecated values no longer exist
	
	//check the text
	KOMACROTEST_ASSERT( macro->text(), QString("My Macro") );
	//check iconname
	KOMACROTEST_ASSERT( QString(macro->icon()), QString("myicon") );
	//check comment
	KOMACROTEST_ASSERT( macro->comment(), QString("Some comment to describe the Macro.") );
	 */
	
	//create list of KsharedPtr from the childs of the macro
	QValueList< KoMacro::MacroItem::Ptr >& items = macro->items();
	//check that there is one
	KOMACROTEST_ASSERT( items.count(), sizetype(1) );
	//fetch the first one
	KoMacro::Action::Ptr actionptr = items[0]->action();
	//How do we know that an action exist ?	
	//-> check that it is not null
	KOMACROTEST_XASSERT(sizetype(actionptr.data()), sizetype(0));
	//check that it has the right name
	KOMACROTEST_ASSERT( QString(actionptr->name()), QString("testaction") );
	//check that it has the right text
	KOMACROTEST_ASSERT( actionptr->text(), QString("Test") );
	//check that it has the right comment
//	KOMACROTEST_ASSERT( actionptr->comment(), QString("") );
/*
	//fetch the second one
	KoMacro::Action::Ptr myfuncptr = children[1];
	//cast it to function
	
                                                                                    KoMacro::Function* myfunc = dynamic_cast<KoMacro::Function*>( myfuncptr.data() );
	//check that it isn?t null
	KOMACROTEST_XASSERT((int) myfunc, 0);

	//check it?s name
	KOMACROTEST_ASSERT( QString(myfunc->name()), QString("myfunc"));
	
                                                                                    //check it?s text
	KOMACROTEST_ASSERT( myfunc->text(), QString("My Function") );
	//check it?s comment
	KOMACROTEST_ASSERT( myfunc->comment(), QString("Describe what the function does") );
	//check it?s receiver object
	KOMACROTEST_ASSERT( myfunc->receiver(), QString("TestObject") );
	//check it?s slot
	KOMACROTEST_ASSERT( myfunc->slot(), QString("myslot(const QString&)") );
	
	//exceute it
	myfunc->activate();
*/	
	//create another macro
	KoMacro::Macro::Ptr yanMacro = KoMacro::Manager::self()->createMacro("testMacro2");
	
	KOMACROTEST_ASSERT(yanMacro->parseXML(domelement),true);
	//create two more macros
	//KoMacro::Action::Ptr yanActionptr2 = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//KoMacro::Action::Ptr yanActionptr3 = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	
	//check that they aren?t null
	KOMACROTEST_XASSERT(sizetype(yanMacro.data()), sizetype(0));
	//KOMACROTEST_XASSERT((int) yanActionptr2.data(), 0);
	//KOMACROTEST_XASSERT((int) yanActionptr3.data(), 0);
	
	//create a list of the children from yanMacro
	//QValueList< KoMacro::Action::Ptr > yanChildren = yanMacro->children();
	//check that there are two
	//KOMACROTEST_ASSERT(yanChildren.count(), uint(2));
/*
	{
		//keep oldsize 
		const int oldsize = yanChildren.count();
		//add a new child to the macro
		yanMacro->addChild(yanActionptr2);
		//get the children
		yanChildren = yanMacro->children();
		//get count of children
		const int size = yanChildren.count();
		//check count has changed
		KOMACROTEST_XASSERT(size, oldsize);
	}

	{
		//keep oldsize 
		const int oldsize = yanChildren.count();
		//add a new child to the macro		
		yanMacro->addChild(yanActionptr3);
		//get the children
		yanChildren = yanMacro->children();
		//get count of children
		const int size = yanChildren.count();
		//check count has changed
		KOMACROTEST_XASSERT(size, oldsize);
		//check the hasChildren function
		KOMACROTEST_ASSERT(yanMacro->hasChildren(), true);
	}
*/	

}

void CommonTests::testDom() {
//TODO: CLEANUP!!!!!!	
	kdDebug()<<"===================== testDom() ======================" << endl;
/*
	//create a QDomDocument		
	QDomDocument domdocument = QDomDocument();
	//create data for various documents
	QString	const comment = "Describe what the function does";
	QString const name = "myfunc";
	QString const text = "My Function";
	QString const receiver1 = "TestObject";
	QString const receiver2 = "GibtsNich";

	//create wrong Argument tag
	domdocument.setContent(QString(
		"<function name=\"" + name + "\" text=\"" + text + "\" comment=\"" + comment + "\" receiver=\"" + receiver1 + "\" slot=\"myslot(const QString &amp; , int)\">"
			"<Arg0ment>Some string</Arg0ment>"
			"<Arg0ment>12345</Arg0ment>"
		"</function>"
	));
	//create functiom
	KoMacro::Action::Ptr macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//try to execute function and catch exception
	KOMACROTEST_ASSERTEXCEPTION(KoMacro::Exception&, macroptr->activate());

	//create wrong receiver
	domdocument.setContent(QString(
		"<function name=\"" + name + "\" text=\"" + text + "\" comment=\"" + comment + "\" receiver=\"" + receiver2 + "\" slot=\"myslot(const QString &amp; , int)\">"
			"<argument>Some string</argument>"
			"<argument>12345</argument>"
		"</function>"
	));
	//create function
	macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//try to execute function and catch exception
	KOMACROTEST_ASSERTEXCEPTION(KoMacro::Exception&, macroptr->activate());

	//create "wrong" number of parameters
	domdocument.setContent(QString(
		"<function name=\"" + name + "\" text=\"" + text + "\" comment=\"" + comment + "\" receiver=\"" + receiver1 + "\" slot=\"myslot(const QString &amp; , int, double)\">"
			"<argument>Some string</argument>"
			"<argument>12345</argument>"
			"<argument>12345.25</argument>"
		"</function>"
	));
	//create function
	macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//try to execute function and catch exception
	KOMACROTEST_ASSERTEXCEPTION(KoMacro::Exception&, macroptr->activate());

	//create wrong function tag
	domdocument.setContent(QString(
		"<NoFunction name=\"" + name + "\" text=\"" + text + "\" comment=\"" + comment + "\" receiver=\"" + receiver1 + "\" slot=\"myslot(const QString &amp; , int, double)\">"
			"<argument>Some string</argument>"
			"<argument>12345</argument>"
			"<argument>12345.25</argument>"
		"</NoFunction>"
	));
	//try to create function and catch exception
	KOMACROTEST_ASSERTEXCEPTION(KoMacro::Exception&, macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() ));
	
	//create empty function
	domdocument.setContent(QString(
		"<function name=\"\" text=\"\" comment=\"\" receiver=\"\" slot=\"\">"
			"<argument> </argument>"
			"<argument> </argument>"
			"<argument> </argument>"
		"</function>"
	));
	//create function
	macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//try to execute function and catch exception
	KOMACROTEST_ASSERTEXCEPTION(KoMacro::Exception&, macroptr->activate());

	
	//create empty function
	domdocument.setContent(QString(
		"<function>"
		"</function>"
	));
	//create function
	macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//try to execute function and catch exception
	KOMACROTEST_ASSERTEXCEPTION(KoMacro::Exception&, macroptr->activate());
*/
}

void CommonTests::testVariables()
{
//TODO: CLEANUP!!!!!!	
	kdDebug()<<"===================== testVariables() ======================" << endl;
/*
	//create a QDomDocument		
	QDomDocument domdocument = QDomDocument();
	//create data
	domdocument.setContent(QString(
		"<macro name=\"mymacro123\" text=\"My Macro 123\">"
			"<function name=\"func1\" text=\"Function1\" receiver=\"TestObject\" slot=\"myslot(const QString &amp;)\" >"
	"<argument>$MyArgumentVariable</argument>"
	"<return>$MyReturnVariable</return>"
			"</function>"
		"</macro>"
	));

	//create an macro
	KoMacro::Action::Ptr macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//cast data to macro
	KoMacro::Macro* macro = dynamic_cast<KoMacro::Macro*>( macroptr.data() );
		//check that it is not null
	KOMACROTEST_XASSERT((int) macro, 0);
		
	//create a list of its children
	QValueList< KoMacro::Action::Ptr > children = macro->children();
	//Check that there are two children. The first child is always the returnvalue.
	KOMACROTEST_ASSERT( children.count(), uint(2) );
	//fetch the children
	KoMacro::Action::Ptr func1ptr = children[1];

	//create new context
	KoMacro::Context::Ptr context = new KoMacro::Context(macroptr);

	{
		//try to execute function with non-functional variable
		KOMACROTEST_ASSERTEXCEPTION(KoMacro::Exception&, func1ptr->activate(context));
		
		KOMACROTEST_ASSERTEXCEPTION(KoMacro::Exception&, context->variable("$MyReturnVariable333"));
	}

	{
		//set variable to be a QString
		context->setVariable("$MyArgumentVariable", new KoMacro::Variable("Some string"));
		//execute function
		func1ptr->activate(context);
		//fetch return value
		KoMacro::Variable::Ptr returnvariable = context->variable("$MyReturnVariable");
		//check that it is not null
		KOMACROTEST_XASSERT( (int) returnvariable.data(), 0);
		//check that it is "Some String"
		KOMACROTEST_ASSERT(returnvariable->toString(),QString("Some string"));
	}

	{
		//set variable to be an Int 
		context->setVariable("$MyArgumentVariable", new KoMacro::Variable( 12345 ));
		//execute function
		func1ptr->activate(context);
		//fetch return value
		KoMacro::Variable::Ptr returnvariable = context->variable("$MyReturnVariable");
		//check that it is not null
		KOMACROTEST_XASSERT( (int) returnvariable.data(), 0);
		//check that it is 12345
		KOMACROTEST_ASSERT(returnvariable->toInt(),12345);
	}
*/
}

#include "commontests.moc"
