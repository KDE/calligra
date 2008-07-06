/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIWEBFORMS_UPDATESERVICE_H
#define KEXIWEBFORMS_UPDATESERVICE_H

#include <QHash>
#include <QString>
#include <QList>

#include "WebFormsService.h"

struct RequestData;

namespace KexiWebForms {
    
    class UpdateService : public WebFormsService {
    public:
        UpdateService(const char* name) : WebFormsService(name) {}
        virtual ~UpdateService() {}

        virtual void operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& tcp_conn);
        QHash< QString, QList<uint> > cachedPkeys;
    };
    
}

#endif /* KEXIWEBFORMS_UPDATESERVICE_H */
