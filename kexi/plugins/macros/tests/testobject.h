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

#ifndef KOMACROTEST_TESTOBJECT_H
#define KOMACROTEST_TESTOBJECT_H

#include <qobject.h>
#include <kunittest/tester.h>

namespace KoMacroTest {

	/**
	* The TestObject class is used to test handling and communication
	* of external from QObject inheritated classes.
	*/
	class TestObject : public QObject
	{
			Q_OBJECT
		public:

			/**
			* Constructor.
			* 
			* @param tester The @a KUnitTest::Tester instance
			* that likes to test our TestObject instance.
			*/
			TestObject(KUnitTest::Tester* const tester);

			/**
			* Destructor.
			*/
			virtual ~TestObject();

		public slots:

			/**
			* This slot got published to the KoMacro-framework
			* and will be called to test the functionality.
			*/
			void myslot();

			/**
			* This slot got published to the KoMacro-framework
			* and will be called to test the functionality.
			*/
			int myslot(const QString&, int);

			/**
			* This slot got published to the KoMacro-framework
			* and will be called to test the functionality.
			*/
			QString myslot(const QString&);

			/**
			* This slot got published to the KoMacro-framework
			* and will be called to test the functionality.
			* @return is @param d
			*/
			double myslot(const QString&, double d);
		
		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};
}

#endif
