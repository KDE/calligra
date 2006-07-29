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

using namespace KUnitTest;
using namespace KoMacroTest;

namespace KoMacroTest {

	/**
	* Register KoMacroTest::CommonTests as TestSuite.
	*/
	
	KUNITTEST_SUITE("KoMacroTestSuite");
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
		
		KSharedPtr<KoMacro::Macro> macro;

		QValueList< KSharedPtr<KoMacro::MacroItem> > items;

		KSharedPtr<KoMacro::Action> actionptr;

		Private()
			: xmlguiclient(0)
			, testaction(0)
			, doomdocument(0)
			, macro(0)
			, actionptr(0)
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
	d->items = d->macro->items();
	d->actionptr = d->items[0]->action();
}

void ActionTests::tearDown()
{
	delete d->actionptr;
	delete d->macro;
	delete d->doomdocument;
	delete d->xmlguiclient;
}


void ActionTests::testMacro()
{
	kdDebug()<<"===================== testMacro() ======================" << endl;
	
	//fetch Items and ..
	//QValueList< KSharedPtr<KoMacro::MacroItem> >& items = d->macro->items();

	//... check that there is one
	KOMACROTEST_XASSERT( d->items.count(), sizetype(0) );
}

void ActionTests::testAction()
{
	kdDebug()<<"===================== testAction() ======================" << endl;

	//get it
	//KSharedPtr<KoMacro::Action> actionptr = d->items[0]->action();
	//-> check that it is not null
	KOMACROTEST_XASSERT(sizetype(d->actionptr.data()), sizetype(0));
}

void ActionTests::testText()
{
	kdDebug()<<"===================== testText() ======================" << endl;		

	//KSharedPtr<KoMacro::Action> actionptr = items[0]->action();

	const QString leetSpeech = "']['3 $']['";
	
	//check i18n text
	KOMACROTEST_ASSERT(d->actionptr->text(),QString("Test"));
	//change it
	d->actionptr->setText(leetSpeech);
	//retest it
	KOMACROTEST_ASSERT(d->actionptr->text(),QString(leetSpeech));
}


void ActionTests::testName()
{
	kdDebug()<<"===================== testName() ======================" << endl;			

	//KSharedPtr<KoMacro::Action> actionptr = items[0]->action();

	//check name
	KOMACROTEST_ASSERT(d->actionptr->name(),QString("testaction"));
	//change it
	d->actionptr->setName("ActionJackson");
	//retest it
	KOMACROTEST_ASSERT(d->actionptr->name(),QString("ActionJackson"));
}

void ActionTests::testComment()
{
	kdDebug()<<"===================== testComment() ======================" << endl;			

	//KSharedPtr<KoMacro::Action> actionptr = items[0]->action();

	//check comment
	KOMACROTEST_XASSERT(d->actionptr->comment(),QString("No Comment!"));
	//set comment
	d->actionptr->setComment("Stringtest");
	//check comment again
	KOMACROTEST_ASSERT(d->actionptr->comment(),QString("Stringtest"));
}

#include "actiontests.moc"
