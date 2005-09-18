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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#ifndef KROSS_KEXIAPP_KEXIAPPMODULE_H
#define KROSS_KEXIAPP_KEXIAPPMODULE_H

#include <qstring.h>
#include <qvariant.h>

#include <api/module.h>

namespace Kross { namespace Api {
    class Manager;
}}

namespace Kross { namespace KexiApp {

    class KexiAppModulePrivate;

    /**
     *
     */
    class KexiAppModule : public Kross::Api::Module
    {
        public:

            /**
             * Constructor.
             */
            KexiAppModule(Kross::Api::Manager* manager);

            /**
             * Destructor.
             */
            virtual ~KexiAppModule();

            /// \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

        private:
            /// Private d-pointer class.
            KexiAppModulePrivate* d;
    };

}}

#endif

