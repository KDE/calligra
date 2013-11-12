/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>
   Copyright (C) 2006 Fredrik Edemar <f_edemar@linux.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoPartAdaptor.h"

#include <QList>

#include "KoDocument.h"
#include "KoPart.h"
#include "KoDocumentInfoDlg.h"
#include "KoDocumentInfo.h"
#include "KoView.h"
#include <kdebug.h>


KoPartAdaptor::KoPartAdaptor(KoPart *part)
    : QDBusAbstractAdaptor(part)
{
    setAutoRelaySignals(true);
    m_part = part;
}

KoPartAdaptor::~KoPartAdaptor()
{
}

int KoPartAdaptor::documentCount()
{
    return m_part->documentCount();
}

QString KoPartAdaptor::document(int idx)
{
    QList<QPointer<KoDocument> > documents = m_part->documents();
    QPointer<KoDocument> doc = documents.at(idx);
    if (!doc)
        return QString();

    return doc->objectName();

}

int KoPartAdaptor::viewCount()
{
    return m_part->viewCount();
}

QString KoPartAdaptor::view(int idx)
{
    QList<QPointer<KoView> > views = m_part->views();
    QPointer<KoView> v = views.at(idx);
    if (!v)
        return QString();

    return v->objectName();
}


#include <KoPartAdaptor.moc>
