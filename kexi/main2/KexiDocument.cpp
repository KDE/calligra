/* This file is part of the KDE project

   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005-2006 Sebastian Sauer <mail@dipe.org>

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

#include "KexiDocument.h"
#include "KexiView.h"
#include "KexiFactory.h"

#include <KoXmlWriter.h>

/*
#include <QApplication>
#include <QFileInfo>
#include <QFont>
#include <QPair>
#include <QTimer>
#include <kcompletion.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <ktemporaryfile.h>
#include <KoApplication.h>
#include <KoDocumentInfo.h>
#include <KoDom.h>
#include <KoMainWindow.h>
#include <KoOasisSettings.h>
#include <KoOasisStyles.h>
#include <KoStoreDevice.h>
#include <KoVariable.h>
#include <KoXmlNS.h>
*/

class KexiDocument::Private
{
    public:
        static QList<KexiDocument*> s_docs;
        static int s_docId;
};

QList<KexiDocument*> KexiDocument::Private::s_docs;
int KexiDocument::Private::s_docId = 0;

KexiDocument::KexiDocument(QWidget *parentWidget, QObject* parent, bool singleViewMode)
    : KoDocument(parentWidget, parent, singleViewMode)
    , d( new Private() )
{
    setObjectName( QString("Document%1").arg(d->s_docId++).toLocal8Bit() );
    documents().append( this );
    setInstance( KexiFactory::global(), false );
}

KexiDocument::~KexiDocument()
{
    //if( isReadWrite() ) saveConfig();
    //d->s_docs.removeAll( this );
    delete d;
}

QList<KexiDocument*> KexiDocument::documents()
{
  return Private::s_docs;
}

void KexiDocument::paintContent(QPainter& painter, const QRect& rect, bool transparent, double zoomX, double zoomY)
{
    //TODO
}

bool KexiDocument::loadXML(QIODevice*, const KoXmlDocument& doc)
{
    //TODO
    return true;
}

bool KexiDocument::loadOasis(const KoXmlDocument& doc, KoOasisStyles& oasisStyles, const KoXmlDocument& settings, KoStore*)
{
    //TODO
    return true;
}

bool KexiDocument::saveOasis(KoStore* store, KoXmlWriter* manifestWriter)
{
    //TODO
    return true;
}

KoView* KexiDocument::createViewInstance(QWidget* parent)
{
    return new KexiView(parent, this);
}

#include "KexiDocument.moc"

