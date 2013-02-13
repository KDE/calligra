/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2010-2011 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "terminaltool.h"

// Kasten core
#include <documentsyncmanager.h>
#include <abstractdocument.h>
#include <abstractmodelsynchronizer.h>
// KDE
#include <KLocale>
#include <KUrl>


namespace Kasten2
{

TerminalTool::TerminalTool( DocumentSyncManager* documentSyncManager )
  : AbstractTool(),
    mDocumentSyncManager( documentSyncManager ),
    mDocument( 0 )
{
    setObjectName( QLatin1String( "Terminal" ) );
}


QString TerminalTool::title() const { return i18nc("@title:window", "Terminal"); }


KUrl TerminalTool::currentUrl() const
{
    KUrl result;

    if( mDocument )
        result = mDocumentSyncManager->urlOf( mDocument ).upUrl();

    return result;
}

void TerminalTool::setTargetModel( AbstractModel* model )
{
    const KUrl oldCurrentUrl = currentUrl();

    mDocument = model ? model->findBaseModel<AbstractDocument*>() : 0;

    const KUrl newCurrentUrl = currentUrl();

    if( oldCurrentUrl != newCurrentUrl )
        emit currentUrlChanged( newCurrentUrl );
}

TerminalTool::~TerminalTool() {}

}
