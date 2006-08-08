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

#include <QString>
#include <QVariant>

#define KROSS_MAIN_EXPORT KDE_EXPORT

#include <api/module.h>

namespace Kross { namespace Api {
    class Manager;
}}

namespace Kross { namespace KSpreadCore {

    /**
     * The KSpreadCoreModule class implements a Kross::Api::Module to
     * provide access to the KSpread functionality.
     *
     * Example (in Ruby) :
     * @code
     * doc = krosskspreadcore::get("KSpreadDocument")
     * sheet = doc.currentSheet()
     * cell = sheet.cell(0, 0)
     * cell.setValue("Hello World")
     * @endcode
     */
    class KSpreadCoreModule : public Kross::Api::Module
    {
        public:
            KSpreadCoreModule(Kross::Api::Manager* manager);
            virtual ~KSpreadCoreModule();
            virtual const QString getClassName() const;
            //virtual Kross::Api::Object::Ptr call(const QString& name, Kross::Api::List::Ptr arguments);
        private:

#if 0
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
#endif

        private:
            Kross::Api::Manager* m_manager;
    };

}}

#endif

