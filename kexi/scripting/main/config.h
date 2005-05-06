/***************************************************************************
 * config.h
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#ifndef KROSS_MAIN_CONFIG_H
#define KROSS_MAIN_CONFIG_H

/**
 * The Kross scripting bridge.
 *
 * Kross is the name of the scripting layer used as bridge
 * between scripting backends like \a Kross::Python and
 * so called functionality wrappers like \a Kross::KexiDB.
 *
 * Kross was mainly developed for usage within Kexi to
 * spend a flexible scripting system without restrictions
 * on the used scripting language, performance or
 * flexibility.
 *
 * \author Sebastian Sauer
 * \sa http://www.koffice.org/kexi
 * \sa http://www.dipe.org/kross
 */
namespace Kross {

    /**
     * The Kross-Api. Used to have a common api for all supported
     * scripting languages. It's designed as proxy for every kind
     * of interaction and spends us the base communication bridge
     * between wrappers like \a Kross::KexiDB and interpreters
     * like \a Kross::Python.
     *
     * \author Sebastian Sauer
     */
    namespace Api {

        // Enable kdDebug()-debugging for Kross::Api::Object
        //#define KROSS_API_OBJECT_DEBUG

    }

    /**
     * Wrapper around the Kexi::KexiDB classes.
     *
     * Only \a Kross::Api is used. So, this wrapper is independend
     * to the used scripting backend and any interpreter who wraps
     * the Kross-Api should be able to make such wrappers accessible
     * from within scripting-languages.
     *
     * \author Sebastian Sauer
     */
    namespace KexiDB {
    }

    /**
     * Code to manage the embedded python interpreter and
     * python-scripts.
     *
     * There is no dependency to e.g. the \a Kross::KexiDB
     * wrapper. Everything is handled through the common
     * \a Kross::Api bridge. Therefore this interpreter-
     * implementation should be able to make all defined
     * wrappers accessible by the python scripting
     * language.
     *
     * Internaly we use PyCXX - a set of classes to help
     * create extensions of python in the C++ language - to
     * access the python c api. Any python version since
     * 2.0 is supported.
     *
     * \author Sebastian Sauer
     * \sa http://www.python.org
     * \sa http://cxx.sourceforge.net
     */
    namespace Python {

        // Enable kdDebug()-debugging for Kross::Python::PythonScript
        //#define KROSS_PYTHON_SCRIPT_DEBUG

        // Enable kdDebug()-debugging for Kross::Python::PythonModule
        //#define KROSS_PYTHON_MODULE_DEBUG

        // Enable kdDebug()-debugging for Kross::Python::PythonExtension
        //#define KROSS_PYTHON_EXTENSION_DEBUG

    }

}

#endif

