/*
 * This file is part of KPlato
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 * Copyright (c) 2008 Dag Andersen <kplato@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTING_MODULE_H
#define SCRIPTING_MODULE_H

#include <QString>
#include <QStringList>
#include <QObject>

#include <KoScriptingModule.h>

#include <kptpart.h>

class QWidget;

namespace KPlato {
    class Part;
}

namespace Scripting {

    class Project;

    /**
    * The Module class enables access to the KPlato functionality
    * from within the scripting backends.
    *
    * Python example that prints the documents Url and some other information;
    * \code
    * import KPlato
    * print KPlato.document().url()
    * print KPlato.document().documentInfoTitle()
    * print KPlato.document().documentInfoAuthorName()
    * print KPlato.project().name()
    * \endcode
    */
    class Module : public KoScriptingModule
    {
            Q_OBJECT
        public:
            explicit Module(QObject* parent = 0);
            virtual ~Module();

            KPlato::Part* part();
            virtual KoDocument* doc();
            
        public Q_SLOTS:

            /// Open another KPlato document
            QObject *openDocument( const QString tag, const QString &url );
            
            /// Return the project
            QObject *project();
            /// Return a schedule list view
            QWidget *createScheduleListView( QWidget *parent );
            /// Return a node property list view
            QWidget *createDataQueryView( QWidget *parent );
            
            /// Return data, default role and schedule
            QVariant data( QObject *object, const QString &property );
            /// Return data
            QVariant data( QObject *object, const QString &property, const QString &role, qlonglong scheduleId );
            /// Return header text
            QVariant headerData( int objectType, const QString &property );
        private:
            /// \internal d-pointer class.
            class Private;
            /// \internal d-pointer instance.
            Private* const d;
    };

}

#endif
