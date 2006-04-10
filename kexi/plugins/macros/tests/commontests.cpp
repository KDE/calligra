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

#include "../lib/action.h"
#include "../lib/function.h"
#include "../lib/manager.h"
#include "../lib/macro.h"
#include "../lib/variable.h"
#include "../lib/metaobject.h"
#include "../lib/metamethod.h"
#include "../lib/metaparameter.h"
#include "../lib/exception.h"

#include <ostream>

#include <qstringlist.h>
#include <qdom.h>

#include <kdebug.h>
#include <kunittest/runner.h>
#include <kxmlguiclient.h>
#include <ksharedptr.h>

/**
* Macro to perform an equality check and exits the method if the check failed.
*
* @param actual The actual value.
* @param expected The expected value.
*/
#define KOMACROTEST_ASSERT(actual, expected) \
	std::cout << QString("Testing: %1 == %2").arg(#actual).arg(#expected).latin1() << std::endl; \
	check( __FILE__, __LINE__, #actual, actual, expected, false ); \
	if(actual != expected) \
	{ \
		kdWarning() << QString("==============> FAILED") << endl; \
		return; \
	}

/**
* Macro to perform a check that is expected to fail and that exits the method if the check failed.
* 
* @param actual The actual value.
* @param notexpected The not expected value.
*/
#define KOMACROTEST_XASSERT(actual, notexpected) \
	std::cout << QString("Testing: %1 != %2").arg(#actual).arg(#notexpected).latin1() << std::endl; \
	check( __FILE__, __LINE__, #actual, actual, notexpected, true ); \
	if(actual == notexpected) \
	{ \
		kdWarning() << QString("==============> FAILED") << endl; \
		return; \
	}

/**
* Macro to test that @p expression throws an exception that is catched with the
* @p exceptionCatch exception.
*
* @param exceptionCatch The exception that is expected to be thrown.
* @param expression The expression that is executed within a try-catch block to
* check for the @p exceptionCatch .
*/
#define KOMACROTEST_ASSERTEXCEPTION(exceptionCatch, expression) \
	try { \
		expression; \
	} \
	catch(exceptionCatch) { \
		setExceptionRaised(true); \
	} \
	if(exceptionRaised()) { \
		success(QString(__FILE__) + "[" + QString::number(__LINE__) + "]: passed " + #expression); \
		setExceptionRaised(false); \
	} \
	else { \
		failure(QString(__FILE__) + "[" + QString::number(__LINE__) + QString("]: failed to throw an exception on: ") + #expression); \
		return; \
	}

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

			/**
			* Constructor.
			*/
			Private()
				: xmlguiclient(0)
				, testobject(0)
			{
			}
	};

}

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
}

void CommonTests::tearDown()
{
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

void CommonTests::testAction()
{
	kdDebug()<<"===================== testAction() ======================" << endl;

	//create a QDomDocument
	QDomDocument domdocument = QDomDocument();
	//create some data
		QString const name = "file_new";
	QString	const comment = "Some comment";
		QString const text = "New file";
		//create a QString with the data
	QString const xml = QString("<action  name=\"%1\" comment=\"%2\" text=\"%3\" />").arg(name).arg(comment).arg(text);
	//set "Action"-content in QDocument	
	domdocument.setContent(xml);
	
	//create an KomacroAction  with our data, and put it into a KSharedPtr
	KSharedPtr<KoMacro::Action> actionptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//check that actionptr is not null
	KOMACROTEST_XASSERT((int) actionptr.data(), 0);

	//check that both domElements are the same
	KOMACROTEST_ASSERT( actionptr->domElement(), domdocument.documentElement() );
	//check that name is the same
	KOMACROTEST_ASSERT( QString(actionptr->name()), name );
		//check that comment is the same
	KOMACROTEST_ASSERT( actionptr->comment(), comment );
		//check that text is the same
	KOMACROTEST_ASSERT( actionptr->text(), text );
	//check that "action" is the same
	KOMACROTEST_ASSERT( actionptr->toString(), QString("Action:%1").arg(name) );
	
	//will be check later, so we need it here no longer....
	//actionptr->activate();
}

void CommonTests::testFunction()
{
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
	KSharedPtr<KoMacro::Action> functionptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
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
	for(KoMacro::Variable::List::Iterator it = funcvariables.begin(); it != funcvariables.end(); ++it) {
		kdDebug() << "VARIABLE => " << (*it)->toString() << endl;
		//hardcoded test:
		// firstrun we have a QString, secondrun we have an int
		if(i == 0) {
			//check first variable of func is the same as argument1
			//QString const argument1 = "Some string";
			KOMACROTEST_ASSERT((*it)->toString(), argument1);
		}
		else {
			//check second variable of func is the same as argument2
			//int const argument2 = 12345;
			KOMACROTEST_ASSERT((*it)->toInt(), argument2);
		}
		i++;
	}
	
	//check that we have two arguments in func
	KOMACROTEST_ASSERT( funcvariables.count(), uint(2) );
	
	//create a KoMacro-Variable-Ptr from first func argument
	KoMacro::Variable::Ptr stringvar = funcvariables[0];
	//check that it is not null
	KOMACROTEST_XASSERT((int) stringvar.data(),0);
	//check via QVariant type that stringvar is from Type Variant
	KOMACROTEST_ASSERT( stringvar->type(), KoMacro::MetaParameter::TypeVariant );
	//check via metaparameter that variant is from type string
	KOMACROTEST_ASSERT( stringvar->variantType(), QVariant::String );
	//chech that stringvar equals argument1
	KOMACROTEST_ASSERT( stringvar->toString(), argument1 );

	//create a KoMacro-Variable-Ptr from second func argument
	KoMacro::Variable::Ptr intvar = funcvariables[1];
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
	func->setReturnValue(new KoMacro::Variable("54321"));
	KOMACROTEST_ASSERT( func->returnValue()->toString(), QString("54321") );
}

void CommonTests::testIntFunction() 
{
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
	KSharedPtr<KoMacro::Action> functionptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//Cast data to function
	KoMacro::Function* func = dynamic_cast<KoMacro::Function*>( functionptr.data() );
		//check that it is not null
	KOMACROTEST_XASSERT((int) func, 0);
	//execute the function 
	func->activate();
	//Check returnvalue is same value we entered
	KOMACROTEST_ASSERT(func->returnValue()->toString(),QString("12345"));
}

void CommonTests::testDoubleFunction() 
{
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
	KSharedPtr<KoMacro::Action> functionptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//Cast data to function
	KoMacro::Function* func = dynamic_cast<KoMacro::Function*>( functionptr.data() );
		//check that it is not null
	KOMACROTEST_XASSERT((int) func, 0);
	//execute the function 	
	func->activate();
	//Check returnvalue is same value we entered
	KOMACROTEST_ASSERT(func->returnValue()->toString(),QString("12.56"));
}

void CommonTests::testQStringFunction() 
{
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
	KSharedPtr<KoMacro::Action> functionptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//Cast data to function
	KoMacro::Function* func = dynamic_cast<KoMacro::Function*>( functionptr.data() );
		//check that it is not null
	KOMACROTEST_XASSERT((int) func, 0);
	//execute the function		func->activate();
	//Check returnvalue is same value we entered
	//KOMACROTEST_ASSERT(func->returnValue()->toString(),QString("Some string"));
}

void CommonTests::testMacro()
{
	kdDebug()<<"===================== testMacro() ======================" << endl;

	//create a QDomDocument		
	QDomDocument domdocument = QDomDocument();

	//Fully fleged content this time with macro,function and action
	domdocument.setContent(QString(
		"<macro name=\"mymacro\" icon=\"myicon\" text=\"My Macro\" comment=\"Some comment to describe the Macro.\">"
			"<action name=\"myaction\" text=\"My Action\" comment=\"Just some comment\" />"
			"<function name=\"myfunc\" text=\"My Function\" comment=\"Describe what the function does\" receiver=\"TestObject\" slot=\"myslot(const QString &amp;)\">"
	"<argument>The myfunc argument string</argument>"
			"</function>"
		"</macro>"
	));

	//create Macro
	KSharedPtr<KoMacro::Action> macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//cast data to Macro
	KoMacro::Macro* macro = dynamic_cast<KoMacro::Macro*>( macroptr.data() );
		//check that it is not null
	KOMACROTEST_XASSERT((int) macro, 0);
		
	//check that domeElement given to manager is the sam as in the macro
	KOMACROTEST_ASSERT( macro->domElement(), domdocument.documentElement() );
	//check the name
	KOMACROTEST_ASSERT( QString(macro->name()), QString("mymacro") );
	//check the tect
	KOMACROTEST_ASSERT( macro->text(), QString("My Macro") );
	//check iconname
	KOMACROTEST_ASSERT( QString(macro->icon()), QString("myicon") );
	//check comment
	KOMACROTEST_ASSERT( macro->comment(), QString("Some comment to describe the Macro.") );

	//create list of KsharedPtr from the childs of the macro
	QValueList< KSharedPtr<KoMacro::Action> > children = macro->children();
	//check that there are two
	KOMACROTEST_ASSERT( children.count(), uint(2) );
	//fetch the first one
	KSharedPtr<KoMacro::Action> myactionptr = children[0];
	//check that it isn?t null
	KOMACROTEST_XASSERT((int) myactionptr.data(), 0);
	//check that it has the right name
	KOMACROTEST_ASSERT( QString(myactionptr->name()), QString("myaction") );
	//check that it has the right text
	KOMACROTEST_ASSERT( myactionptr->text(), QString("My Action") );
	//check that it has the right comment
	KOMACROTEST_ASSERT( myactionptr->comment(), QString("Just some comment") );

	//fetch the second one
	KSharedPtr<KoMacro::Action> myfuncptr = children[1];
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
	
	//create another macro
	KSharedPtr<KoMacro::Action> yanActionptr1 = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//cast data as macro
	KoMacro::Macro* yanMacro = dynamic_cast<KoMacro::Macro*>( yanActionptr1.data() );	
	//create two more macros
	KSharedPtr<KoMacro::Action> yanActionptr2 = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	KSharedPtr<KoMacro::Action> yanActionptr3 = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	
	//check that they aren?t null
	KOMACROTEST_XASSERT((int) yanActionptr1.data(), 0);
	KOMACROTEST_XASSERT((int) yanActionptr2.data(), 0);
	KOMACROTEST_XASSERT((int) yanActionptr3.data(), 0);
	
	//create a list of the children from yanMacro
	QValueList< KSharedPtr<KoMacro::Action> > yanChildren = yanMacro->children();
	//check that there are two
	KOMACROTEST_ASSERT(yanChildren.count(), uint(2));

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
}

void CommonTests::testDom() {

	kdDebug()<<"===================== testDom() ======================" << endl;

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
	KSharedPtr<KoMacro::Action> macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
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
}

void CommonTests::testVariables()
{
	kdDebug()<<"===================== testVariables() ======================" << endl;

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

	//cretae an macro
	KSharedPtr<KoMacro::Action> macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//cast data to macro
	KoMacro::Macro* macro = dynamic_cast<KoMacro::Macro*>( macroptr.data() );
		//check that it is not null
	KOMACROTEST_XASSERT((int) macro, 0);
		
	//create a list of its children
	QValueList< KSharedPtr<KoMacro::Action> > children = macro->children();
	//Check that there is one children
	KOMACROTEST_ASSERT( children.count(), uint(1) );
	//fetch the children
	KSharedPtr<KoMacro::Action> func1ptr = children[0];

	//create new context
	KoMacro::Context::Ptr context = new KoMacro::Context(macroptr);

	{
		//try to execute finction with non-functional variable
		KOMACROTEST_ASSERTEXCEPTION(KoMacro::Exception&, func1ptr->activate(context));
		// ??? ??? ???
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

}

#include "commontests.moc"
