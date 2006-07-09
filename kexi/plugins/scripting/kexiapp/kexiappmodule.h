/***************************************************************************
 * kexiappmodule.h
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

#ifndef KROSS_KEXIAPP_KEXIAPPMODULE_H
#define KROSS_KEXIAPP_KEXIAPPMODULE_H

#include <qstring.h>
#include <qvariant.h>

#include <api/module.h>

namespace Kross { namespace Api {
    class Manager;
}}

namespace Kross { 

/**
 * Wrapper around the Kexi-application to access runtime
 * informations a running Kexi-application likes to
 * provide.
 */
namespace KexiApp {

    class KexiAppModulePrivate;

    /**
     * The Kexi-application module which provides us the
     * main entrypoint to communicate with a running
     * Kexi-application.
     */
    class KexiAppModule : public Kross::Api::Module
    {
        public:

            /**
             * Constructor.
             *
             * \param manager The \a Kross::Api::Manager singleton
             *        instance used to access this module.
             */
            KexiAppModule(Kross::Api::Manager* manager);

            /**
             * Destructor.
             */
            virtual ~KexiAppModule();

            /// \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

        private:
            /// Private d-pointer class.
            KexiAppModulePrivate* d;
    };

}}

#endif

