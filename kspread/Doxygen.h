/* This file is part of the KDE project
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

/* This file collects global doxygen documentation.
   It should not be included in any other source file.
   To keep these documentation parts in a separate file prevents
   unnecessary recompiling, if changes on it are done.
*/

/* NOTE Stefan:
        Use doxygen's 'ingroup' command for each method/class/namespace
        explicitly. This way one sees immediately to which group the method
        belongs, if one looks directly into the header file.
        Don't group member function together and use 'addgroup'.
*/

/**
 * \defgroup OpenDocument
 * Classes and methods related to the OpenDocument loading/saving.
 */

/**
 * \defgroup Operations
 * Classes and methods related to KSpread Operations.
 * These are all actions that can be un- and redone.
 */

/**
 * \defgroup NativeFormat
 * Classes and methods related to the KSpread's native file format loading/saving.
 */

/**
 * \defgroup Painting
 * Classes and methods related to the painting of cells and objects.
 */

/**
 * \defgroup Plugins
 * Classes and methods related to the plugins.
 */

