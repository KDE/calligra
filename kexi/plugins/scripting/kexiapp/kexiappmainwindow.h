/***************************************************************************
 * kexiappmainwindow.h
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

#ifndef KROSS_KEXIAPP_KEXIAPPMAINWINDOW_H
#define KROSS_KEXIAPP_KEXIAPPMAINWINDOW_H

#include <qstring.h>
#include <qvariant.h>

#include <api/object.h>
#include <api/variant.h>
#include <api/list.h>
#include <api/class.h>

// Forward declarations.
class KexiMainWindow;

namespace Kross { namespace KexiApp {

    // Forward declarations.
    class KexiAppMainWindowPrivate;

    /**
     * Class to handle Kexi's mainwindow instance.
     */
    class KexiAppMainWindow : public Kross::Api::Class<KexiAppMainWindow>
    {
        public:

            /**
             * Constructor.
             *
             * \param mainwindow The \a KexiMainWindow instance
             *       this class provides access to.
             */
            KexiAppMainWindow(KexiMainWindow* mainwindow);

            /**
             * Destructor.
             */
            virtual ~KexiAppMainWindow();

            /// \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

        private:
            /// Private d-pointer class.
            KexiAppMainWindowPrivate* d;

            Kross::Api::Object::Ptr isConnected(Kross::Api::List::Ptr);

            /** \return the \a Kross::KexiDB::KexiDBConnection object that 
            belongs to the opened project or throw an exception if there 
            was no project opened (no connection established). */
            Kross::Api::Object::Ptr getConnection(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr getPartItems(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr openPartItem(Kross::Api::List::Ptr);
    };

}}

#endif

