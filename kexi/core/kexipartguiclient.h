/* This file is part of the KDE project
   Copyright (C) 2003 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIPARTGUICL_H
#define KEXIPARTGUICL_H

#include "kexipart.h"

#include <QObject>

#include <kxmlguiclient.h>

namespace KexiPart
{

/** @internal A GUI Client used by KexiPart::Part objects within KexiMainWindow
*/
class GUIClient : public QObject, public KXMLGUIClient
{
    Q_OBJECT
public:
    virtual ~GUIClient() {}

    inline Part *part() {
        return static_cast<Part*>(QObject::parent());
    }

protected:
    /*! Creates a new GUI Client. If \a partInstanceClient is true, the part will be
     used as "instance" client, otherwise it will be defined per-view.
     \a nameSuffix is used in constructing client's name (only useful for debugging purposes). */
    GUIClient(Part* part, bool partInstanceClient, const char* nameSuffix);

    friend class Part;
};

}

#endif

