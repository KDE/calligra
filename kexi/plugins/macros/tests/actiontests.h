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

#ifndef KOMACROTEST_ACTIONTESTS_H
#define KOMACROTEST_ACTIONTESTS_H

#include <kunittest/tester.h>

namespace KoMacroTest {

	/**
	* The common testsuite used to test common @a KoMacro
	* functionality.
	*/
	class ActionTests : public KUnitTest::SlotTester
	{
			Q_OBJECT
		public:

			/**
			* Constructor.
			*/
			ActionTests();

			/**
			* Destructor.
			*/
			virtual ~ActionTests();

		public slots:

			/**
			* This slot got called by KUnitTest before testing
			* starts.
			*/
			void setUp();

			/**
			* This slot got called by KUnitTest after all tests
			* are done.
			*/
			void tearDown();

			/**
			* Test the @a KoMacro::Action functionality.
			*/
			void testAction();
			
			/**
			* Subtest for the @a KoMacro::Action functionality.
			*/
			void testMacro();
			/**
			* Subtest for the @a KoMacro::Action functionality.
			*/
			void testMacroItem();
			/**
			* Subtest for the @a KoMacro::Action functionality.
			*/			
			void testText();
			/**
			* Subtest for the @a KoMacro::Action functionality.
			*/
			void testName();
			/**
			* Subtest for the @a KoMacro::Action functionality.
			*/
			void testComment();
			/**
			* Subtest for the @a KoMacro::Action functionality.
			*/
			void testVariableString();
			/**
			* Subtest for the @a KoMacro::Action functionality.
			*/
			void testVariableInt();
			/**
			* Subtest for the @a KoMacro::Action functionality.
			*/
			void testVariableBool();

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
