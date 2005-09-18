/***************************************************************************
 * kexidbmodule.h
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

#ifndef KROSS_KEXIDB_KEXIDBMODULE_H
#define KROSS_KEXIDB_KEXIDBMODULE_H

#include <qstring.h>
#include <qvariant.h>

#include <api/module.h>

namespace Kross { namespace Api {
    class Manager;
}}

namespace Kross { namespace KexiDB {

    /**
     * The KexiDBModule wrapper around KexiDB.
     *
     * This class implementates a \a Kross::Api::Module to wrap
     * those parts of KexiDB that should be accessible for
     * the different scripting languages. All work will be done
     * and presented in the common Kross::Api and therefore
     * is independend of any used scripting-backend.
     */
    class KexiDBModule : public Kross::Api::Module
    {
        public:

            /**
             * Constructor.
             */
            KexiDBModule(Kross::Api::Manager* manager);

            /**
             * Destructor.
             */
            virtual ~KexiDBModule();

            /// \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

virtual Kross::Api::Object::Ptr get(const QString& name, void* = 0);

    };

}}

#endif

