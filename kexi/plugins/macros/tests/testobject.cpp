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

#include "testobject.h"

//#include "../lib/manager.h"
//#include "../lib/action.h"
//#include "../lib/function.h"
//#include "../lib/macro.h"
//#include "../lib/metaobject.h"

//#include <qstringlist.h>
//#include <qdom.h>

#include <kdebug.h>
//#include <kxmlguiclient.h>

using namespace KoMacroTest;

namespace KoMacroTest {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class TestObject::Private
	{
		public:
		
			/**
			* The @a KUnitTest::Tester instance that likes to test
			* our TestObject instance.
			*/
			KUnitTest::Tester* const tester;
				Private(KUnitTest::Tester* const tester)
	: tester(tester)
			{
			}
	};

}

TestObject::TestObject(KUnitTest::Tester* const tester)
	: QObject()
	, d( new Private(tester) ) // create the private d-pointer instance.
{
	setName("TestObject");
}

TestObject::~TestObject()
{
	delete d;
}

//testObject without arguments
void TestObject::myslot()
{
	QString s = "CALLED => TestObject::myslot()";
	//be loud
	kdDebug() << s << endl;
	//add some extra Debuginfos to TestResults see tester.h
	d->tester->results()->addDebugInfo(s);
}

//testobject with QString and int argument
//int is returnvalue
int TestObject::myslot(const QString&, int i)
{
	QString s = "CALLED => TestObject::myslot(const QString&, int)";
	//be loud
	kdDebug() << s << endl;
	//add some extra debuginfos to TestResults
	d->tester->results()->addDebugInfo(s);
	return i;
}

//testobject with QString argument
//QString is returnvalue
QString TestObject::myslot(const QString& s)
{
	QString t = QString("CALLED => TestObject::myslot(const QString& s) s=%1").arg(s);
	//be loud
	kdDebug() << t << endl;
	//add some extra Debuginfos to TestResults
	d->tester->results()->addDebugInfo(t);
	return s;
}

//testobject with QString and double argument
//double is returnvalue
double TestObject::myslot(const QString&, double d)
{
	QString s = "CALLED => TestObject::myslot(const QString&, double)";
	//be loud
	kdDebug() << s << endl;
	//add some extra Debuginfos to TestResults
	this->d->tester->results()->addDebugInfo(s);
	return d;
}

#include "testobject.moc"
