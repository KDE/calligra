/***************************************************************************
 * pythonmanager.h
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
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

#ifndef KROSS_PYTHONMANAGER_H
#define KROSS_PYTHONMANAGER_H

#include <Python.h>

#include <qstring.h>
#include <qstringlist.h>
#include <kdebug.h>

namespace Kross
{

    class PythonManagerPrivate;

    /**
     * The PythonManager class is the main and most
     * abstracted interface to work with python.
     */
    class PythonManager
    {
        public:
            /**
             * Constructor.
             *
             * \param name The Programname passed
             *        to python.
             */
            explicit PythonManager(char* name);

            /**
             * Destructor.
             */
            ~PythonManager();

            /**
             * Execute a python string.
             *
             * \param execstring The string to pass
             *        to python for execution.
             * \param modules A list of build-in
             *        modules like "kexidb" to publish
             *        before execution.
             * \return true on success else false.
             */
            bool execute(const QString& execstring, const QStringList& modules);

        private:
            PythonManagerPrivate* d;
    };

}

#endif
