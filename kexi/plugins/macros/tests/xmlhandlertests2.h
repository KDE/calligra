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

#ifndef KOMACROTEST_XMLHandlerTests2_H
#define KOMACROTEST_XMLHandlerTests2_H

#include <kunittest/tester.h>
#include "../lib/macro.h"

namespace KoMacroTest {

	/**
	* The common testsuite used to test common @a KoMacro
	* functionality.
	*/
	class XMLHandlerTests2 : public KUnitTest::SlotTester
	{
			Q_OBJECT
		public:

			/**
			* Constructor.
			*/
			XMLHandlerTests2();

			/**
			* Destructor.
			*/
			virtual ~XMLHandlerTests2();

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
			* Test the @a KoMacro::XMLHandler parseXML()-function.
			*/
			void testParseAndToXML();

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;

#if 0		
			/** 
			* Compares a XML-Element with a Macro. Call sub-asserts.
			* @p macro The parsen @a Macro.
			* @p domelement The given @a QDomElement which is parsen.
			* @p isitemsempty Bool for expectation of an empty @a MacroItem -List.
			* @p isactionset Bool for expectation that the @a Action -names are equal.
			* @p isvariableok QMap of Bools for comparing each @a Variable .
			*/
			void assertMacroContentEqToXML(const KSharedPtr<KoMacro::Macro> macro,
				const QDomElement& elem,
				const bool isitemsempty,
				const bool isactionset,
				const QMap<QString, bool> isvariableok);

			// Prints a QMap of Variables to kdDebug().
			void printMvariables(const QMap<QString, KSharedPtr<KoMacro::Variable > > mvariables, const QString s);
#endif
			/** 
			* Sub-methods of testParseXML() and testToXML().
			* Test the correct parsing of a @a QDomElement into a @a Macro
			* respectively expected failure of parsing. Then transform it
			* back and compare it.
			*/
			// 1.Test - Correct DomElement.
			void testCorrectDomElement();
#if 0			// 2.Test - XML-document with bad root element.
			void testBadRoot();
			// 3.Test - XML-document with a missing Variable.
			void testMissingVariable();
			// 4.Test - One more Variable in XML-Document.
			void testMoreVariables();
			// 5.Test - XML-document with wrong macro-xmlversion.
			void testWrongVersion();
			// 6.Test - XML-document if it has a wrong structure like
			// wrong parathesis	or missing end tag.
			void testWrongXMLStruct();
			// 7.Test-XML-document with maximum field-size.
			void testMaxNum();
			// 8.Test-XML-document with maximum+1 field-size.
			void testMaxNum2();
			// 9.Test-XML-document with minimum field-size.
			void testMinNum();
			// 10.Test-XML-document with minimum-1 field-size.
			void testMinNum2();
			// 11.Test - With a to big number.
			void testBigNumber();
			// 12.Test - With two MacroItems.
			void testTwoMacroItems();
#endif
			bool assertActionsEqual(KSharedPtr<KoMacro::Action> action,
				KSharedPtr<KoMacro::Action> action2);
	};
}

#endif
