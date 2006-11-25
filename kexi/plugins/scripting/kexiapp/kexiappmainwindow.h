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

#ifndef SCRIPTING_KEXIAPPMAINWINDOW_H
#define SCRIPTING_KEXIAPPMAINWINDOW_H

#include <qstring.h>
#include <qvariant.h>

// Forward declarations.
class KexiMainWindow;
class KexiProject;

namespace Scripting {

    // Forward declarations.
    class KexiAppMainWindowPrivate;

    /**
     * Class to handle Kexi's mainwindow instance.
     */
    class KexiAppMainWindow : public QObject
    {
            Q_OBJECT
        public:
            KexiAppMainWindow(KexiMainWindow* mainwindow);
            virtual ~KexiAppMainWindow();
            KexiProject* project();

        private:
            KexiMainWindow* m_mainwindow;

            bool isConnected();

#if 0
            /** \return the \a Kross::KexiDB::KexiDBConnection object that 
            belongs to the opened project or throw an exception if there 
            was no project opened (no connection established). */
            Kross::Api::Object::Ptr getConnection(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr getPartItems(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr openPartItem(Kross::Api::List::Ptr);
#endif
    };

}

#endif

