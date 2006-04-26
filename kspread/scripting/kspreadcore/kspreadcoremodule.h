/*
 *  Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 *  Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KSPREAD_KROSS_KSPREADCOREMODULE_H
#define KSPREAD_KROSS_KSPREADCOREMODULE_H

#include <qstring.h>
#include <qvariant.h>

#define KROSS_MAIN_EXPORT KDE_EXPORT

#include <api/module.h>
#include <api/event.h>

namespace Kross { namespace Api {
    class Manager;
}}

namespace Kross { namespace KSpreadCore {
    /**
     * This class contains functions use to create new Kross object in a script
     */
    class KSpreadCoreFactory : public Kross::Api::Event<KSpreadCoreFactory>
    {
        public:
            KSpreadCoreFactory();
        private:
            Kross::Api::Object::Ptr newRGBColor(Kross::Api::List::Ptr args);
    };
    /**
     *
     */
    class KSpreadCoreModule : public Kross::Api::Module
    {
        public:
            /**
             * Constructor.
             */
            KSpreadCoreModule(Kross::Api::Manager* manager);

            /**
             * Destructor.
             */
            virtual ~KSpreadCoreModule();

            /// \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;
            virtual Kross::Api::Object::Ptr call(const QString& name, Kross::Api::List::Ptr arguments);
        private:
            /**
             * This function return a new Color with the given RGB triplet
             * It takes three arguments :
             *  - red color (0 to 255)
             *  - blue color (0 to 255)
             *  - green color (0 to 255)
             *
             * For example (in ruby) :
             * @code
             * Krosskritacore::newRGBColor(255,0,0) # create a red color
             * Krosskritacore::newRGBColor(255,255,255) # create a white color
             * @endcode
             */
             Kross::Api::Object::Ptr newRGBColor(Kross::Api::List::Ptr);
        private:
            Kross::Api::Manager* m_manager;
            KSpreadCoreFactory* m_factory;
    };
    

}}

#endif

