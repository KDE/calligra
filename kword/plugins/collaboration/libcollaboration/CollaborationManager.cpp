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
#include "CollaborationManager.h"
#include <QtDebug>
#include <DNSSD/PublicService>
#include "network/ClientSession.h"
#include "network/ServerSession.h"
using namespace kcollaborate;

CollaborationManager::CollaborationManager( QObject *parent ):
        QObject( parent ), mainSession( NULL )
{}

CollaborationManager::~CollaborationManager()
{}

void CollaborationManager::connectUrl( const Url &url )
{
    qDebug() << "connect to " << url;
    delete mainSession;//TODO: something better
    mainSession =  new ClientSession( url, this );
}

void CollaborationManager::share( const Url &url )
{
    qDebug() << "share from " << url;
    delete mainSession;//TODO: something better
    mainSession =  new ServerSession( url, this );
}

#include "CollaborationManager.moc"
