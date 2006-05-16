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
#include <ksharedptr.h>

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
	//Singelton more or less ...
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
//TODO: CLEANUP!!!!!!	
	kdDebug()<<"===================== testMacro() ======================" << endl;

	QDomElement const domelement = d->doomdocument->documentElement();
	
	KSharedPtr<KoMacro::Macro> macro = KoMacro::Manager::self()->createMacro("testMacro");
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
// // 	KSharedPtr<KoMacro::Action> macroptr = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
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
	QValueList< KSharedPtr<KoMacro::MacroItem> >& items = macro->items();
	//check that there is one
	KOMACROTEST_ASSERT( items.count(), sizetype(1) );
	//fetch the first one
	KSharedPtr<KoMacro::Action> actionptr = items[0]->action();
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
*/	
	//create another macro
	KSharedPtr<KoMacro::Macro> yanMacro = KoMacro::Manager::self()->createMacro("testMacro2");
	
	KOMACROTEST_ASSERT(yanMacro->parseXML(domelement),true);
	//create two more macros
	//KSharedPtr<KoMacro::Action> yanActionptr2 = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	//KSharedPtr<KoMacro::Action> yanActionptr3 = ::KoMacro::Manager::self()->createAction( domdocument.documentElement() );
	
	//check that they aren?t null
	KOMACROTEST_XASSERT(sizetype(yanMacro.data()), sizetype(0));
	//KOMACROTEST_XASSERT((int) yanActionptr2.data(), 0);
	//KOMACROTEST_XASSERT((int) yanActionptr3.data(), 0);
	
	//create a list of the children from yanMacro
	//QValueList< KSharedPtr<KoMacro::Action> > yanChildren = yanMacro->children();
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
#include "macrotests.moc"
