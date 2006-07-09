/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
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
#ifndef KOMACROTEST_BASE_H
#define KOMACROTEST_BASE_H

//Our own extended Macros from KUnittest
/**
* Macro to perform an equality check and exits the method if the check failed.
*
* @param actual The actual value.
* @param expected The expected value.
*/
#define KOMACROTEST_ASSERT(actual, expected) \
	{ \
		std::cout << QString("Testing: %1 == %2").arg(#actual).arg(#expected).latin1() << std::endl; \
		check( __FILE__, __LINE__, #actual, actual, expected, false ); \
		if(actual != expected) \
		{ \
			kdWarning() << QString("==============> FAILED") << endl; \
			return; \
		} \
	}

/**
* Macro to perform a check that is expected to fail and that exits the method if the check failed.
* 
* @param actual The actual value.
* @param notexpected The not expected value.
*/
#define KOMACROTEST_XASSERT(actual, notexpected) \
	{ \
		std::cout << QString("Testing: %1 != %2").arg(#actual).arg(#notexpected).latin1() << std::endl; \
		check( __FILE__, __LINE__, #actual, actual, notexpected, true ); \
		if(actual == notexpected) \
		{ \
			kdWarning() << QString("==============> FAILED") << endl; \
			return; \
		} \
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
	{ \
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
		} \
	}

#endif

//Used more tha once at various places 
//names of variables from testaction
namespace KoMacroTest {
	static const QString TESTSTRING = "teststring";
	static const QString TESTINT = "testint";
	static const QString TESTBOOL = "testbool";	
}
