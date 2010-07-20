/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2010 Sven Langkamp <sven.langkamp@gmail.com>

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

#include <kaboutdata.h>

#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KDE/KPluginFactory>
#include <KDE/KStandardDirs>
#include <KDE/KMimeType>
#include <KDE/KParts/ComponentFactory>
#include <QtCore/QDebug>
#include <OkularOdpGenerator.h>
#include <okular/core/page.h>
#include <qimage.h>
#include <qpainter.h>

static KAboutData createAboutData()
{
    KAboutData aboutData(
         "okular_odp",
         "okular_odp",
         ki18n( "ODP Backend" ),
         "0.1",
         ki18n( "ODP file renderer" ),
         KAboutData::License_GPL,
         ki18n( "© 2010 Sven Langkamp" )
    );

    // fill the about data
    return aboutData;
}

OKULAR_EXPORT_PLUGIN(OkularOdpGenerator, createAboutData())

OkularOdpGenerator::OkularOdpGenerator( QObject *parent, const QVariantList &args )
    : Okular::Generator( parent, args )
{
    m_doc = 0;
}

OkularOdpGenerator::~OkularOdpGenerator()
{
}

bool OkularOdpGenerator::loadDocument( const QString &fileName, QVector<Okular::Page*> &pages )
{
    KComponentData cd("OkularOdpGenerator", QByteArray(),
                      KComponentData::SkipMainComponentRegistration);
    KPluginFactory *factory = KPluginLoader("libkpresenterpart", cd).factory();
    if (!factory) {
        qDebug() << "could not load libkpresenterpart";
        return false;
    }
    KoPADocument* doc = factory->create<KoPADocument>();
    m_doc = doc;
    KUrl url;
    url.setPath(fileName);
    doc->setCheckAutoSaveFile(false);
    doc->setAutoErrorHandlingEnabled(false); // show error dialogs
    if (!doc->openUrl(url)) {
        return false;
    }
    doc->setReadWrite(false);
    doc->setAutoSave(0);


    int pageCount = m_doc->pageCount();
    for(int i = 0; i < pageCount; i++) {
        KoPAPageBase* kprpage = m_doc->pages().value(i);
        if (!kprpage) {
            continue;
        }
        QSize size = kprpage->size().toSize();
    
        Okular::Page * page = new Okular::Page( i, size.width(), size.height(), Okular::Rotation0 );
        pages.append(page);
    }

    return true;
}

bool OkularOdpGenerator::doCloseDocument()
{
    delete m_doc;
    return true;
}

bool OkularOdpGenerator::canGeneratePixmap() const
{
    return true;
}

void OkularOdpGenerator::generatePixmap( Okular::PixmapRequest *request )
{
    QPixmap* pix;
    if (!m_doc) {
        pix = new QPixmap(request->width(), request->height());
        QPainter painter(pix);
        painter.fillRect(0 ,0 , request->width(), request->height(), Qt::white);
    } else {
        KoPAPageBase* page = m_doc->pages().value(request->pageNumber());
        pix = new QPixmap(page->thumbnail(QSize(request->width(), request->height())));
    }

   request->page()->setPixmap( request->id(), pix );

    signalPixmapRequestDone( request );
}
