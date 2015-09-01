/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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

#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexipartitem.h"
//! @todo KEXI3 #include "kexistaticpart.h"
#include "KexiWindow.h"
#include "KexiWindowData.h"
#include "KexiView.h"
#include "kexipartguiclient.h"
#include "KexiMainWindowIface.h"
#include "kexi.h"
#include <kexiutils/utils.h>

#include <KDb>
#include <KDbConnection>

#include <KActionCollection>
#include <KMessageBox>

#include <QDebug>

namespace KexiPart
{

class GUIClient::Private
{
public:
    Private() : actionCollection(static_cast<QObject*>(0)) {}
    KActionCollection actionCollection;
};
}

//-------------------------------------------------------------------------

using namespace KexiPart;

GUIClient::GUIClient(Part* part, bool partInstanceClient, const char* nameSuffix)
        : QObject(part)
        , d(new Private)
{
    Q_UNUSED(partInstanceClient);
    setObjectName(
        part->info()->id() + (nameSuffix ? QString(":%1").arg(nameSuffix) : QString()));
}

GUIClient::~GUIClient()
{
    delete d;
}

KActionCollection* GUIClient::actionCollection() const
{
    return &d->actionCollection;
}
