/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
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
#ifndef KCOLLABORATE_COLLABORATIONMANAGER_H
#define KCOLLABORATE_COLLABORATIONMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>

#include <libcollaboration/kcollaborate_export.h>
#include <libcollaboration/network/Url.h>

namespace kcollaborate
{

class Session;

//TODO: remove this class?
class KCOLLABORATE_EXPORT CollaborationManager : public QObject
{
        Q_OBJECT

    public:
        CollaborationManager( QObject *parent = 0 );
        virtual ~CollaborationManager();

    private slots:
        void connect( const Url &url );
        void share( const Url &url );

    private:
        Session *mainSession;
};

};

#endif
