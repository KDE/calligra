/***************************************************************************
 * script.h
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

#ifndef KROSS_API_SCRIPT_H
#define KROSS_API_SCRIPT_H

#include <qstring.h>
#include <qvariant.h>
#include <qobject.h>
//#include <kdebug.h>
//#include "object.h"

namespace Kross { namespace Api {

    // Forward declarations.
    class Manager;
    class Object;
    class List;

    /**
     * The Script class represents a single scriptfile.
     */
    class Script : private QObject
    {
            Q_OBJECT

        public:

            /**
             * Constructor.
             */
            explicit Script(Manager* manager);

            /**
             * Destructor.
             */
            ~Script();

            /**
             * Return the scriptcode this Script holds.
             */
            const QString& getCode();

            /**
             * Set the scriptcode this Script holds.
             */
            void setCode(const QString&);

            /**
             * Return the name of the interpreter used
             * on \a execute.
             */
            const QString& getInterpreter();

            /**
             * Set the name of the interpreter used
             * on \a execute.
             */
            void setInterpreter(const QString&);

            bool execute();
            const QVariant& execute(const QString& name, const QVariant& args);
            Kross::Api::Object* execute(const QString& name, Kross::Api::List* args);

        signals:
            //void done();
            //void success();
            //void error();

        public slots:
            //void execute();

        private:
            Manager* m_manager;
            QString m_code;
            QString m_interpreter;
    };

}}

#endif

