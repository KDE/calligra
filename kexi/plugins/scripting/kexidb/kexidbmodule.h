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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KROSS_KEXIDB_KEXIDBMODULE_H
#define KROSS_KEXIDB_KEXIDBMODULE_H

#include <qstring.h>
#include <qvariant.h>

#include <api/module.h>

namespace Kross { namespace Api {
    class Manager;
}}

namespace Kross { 

/**
 * Wrapper around the Kexi::KexiDB classes.
 *
 * Only \a Kross::Api is used. So, this wrapper is independend
 * to the used scripting backend and any interpreter who wraps
 * the Kross-Api should be able to make such wrappers accessible
 * from within scripting-languages.
 */
namespace KexiDB {

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
            KexiDBModule(Kross::Api::Manager* manager);
            virtual ~KexiDBModule();
            virtual const QString getClassName() const;

            /**
             * Variable module-method use to call transparent some functionality
             * the module provides.
             * 
             * \param name A name passed to the method. This name is used internaly
             *        to determinate what the caller likes to do. Each implemented
             *        module have to implement what should be done.
             * \param p A variable pointer passed to the method. It depends on
             *        the module and the name what this pointer is.
             * \return a \a Kross::Api::Object or NULL.
             */
            virtual Kross::Api::Object::Ptr get(const QString& name, void* p = 0);

    };

}}

#endif

