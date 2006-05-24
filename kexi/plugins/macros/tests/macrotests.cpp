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

typedef QValueList< KoMacro::MacroItem::Ptr >::size_type sizetype;


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
	
	KoMacro::Macro::Ptr macro = KoMacro::Manager::self()->createMacro("testMacro");
	//Is our XML parseable ?
	KOMACROTEST_ASSERT(macro->parseXML(domelement),true);

	//check that it is not null
	KOMACROTEST_XASSERT(sizetype(macro.data()), sizetype(0));

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

	//create another macro
	KoMacro::Macro::Ptr yanMacro = KoMacro::Manager::self()->createMacro("testMacro2");
	
	KOMACROTEST_ASSERT(yanMacro->parseXML(domelement),true);
	
	//check that they aren?t null
	KOMACROTEST_XASSERT(sizetype(yanMacro.data()), sizetype(0));
}
#include "macrotests.moc"
