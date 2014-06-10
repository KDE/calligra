/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2012 Sven Langkamp <sven.langkamp@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#include "OkularOdtGenerator.h"

#include <QDebug>
#include <QImage>
#include <QPainter>

#include <KoDocumentEntry.h>
#include <KoPart.h>
#include <KWDocument.h>
#include <KWPage.h>
#include <KWCanvasItem.h>
#include <KoShapeManager.h>

#include <kaboutdata.h>
#include <kmimetype.h>

#include <okular/core/page.h>
#include <okular/core/version.h>

static KAboutData createAboutData()
{
    KAboutData aboutData(
         "okular_odt",
         "okular_odt",
         ki18n( "ODT/OTT Backend" ),
         "0.1",
         ki18n( "ODT/OTT file renderer" ),
         KAboutData::License_GPL,
         ki18n( "© 2012 Sven Langkamp" )
    );

    // fill the about data
    return aboutData;
}

OKULAR_EXPORT_PLUGIN(OkularOdtGenerator, createAboutData())

OkularOdtGenerator::OkularOdtGenerator( QObject *parent, const QVariantList &args )
    : Okular::Generator( parent, args )
{
    m_doc = 0;
}

OkularOdtGenerator::~OkularOdtGenerator()
{
}

bool OkularOdtGenerator::loadDocument( const QString &fileName, QVector<Okular::Page*> &pages )
{
    KComponentData cd("OkularOdtGenerator", QByteArray(),
                      KComponentData::SkipMainComponentRegistration);

    const QString mimetype = KMimeType::findByPath(fileName)->name();

    QString error;
    KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(mimetype);
    KoPart *part = documentEntry.createKoPart(&error);

    if (!error.isEmpty()) {
        qWarning() << "Error creating document" << mimetype << error;
        return 0;
    }

    KWDocument* doc = qobject_cast<KWDocument*>(part->document());
    m_doc = doc;
    KUrl url;
    url.setPath(fileName);
    doc->setCheckAutoSaveFile(false);
    doc->setAutoErrorHandlingEnabled(false); // show error dialogs
    if (!doc->openUrl(url)) {
        return false;
    }

    while (!doc->layoutFinishedAtleastOnce()) {
        QCoreApplication::processEvents();

        if (!QCoreApplication::hasPendingEvents())
            break;
    }

    KWPageManager *pageManager = doc->pageManager();
    int pageCount = pageManager->pages().count();
    for(int i = 0; i < pageCount; ++i) {

        KWPage kwpage = pageManager->pages().at(i);

        Okular::Page * page = new Okular::Page( i, kwpage.width(), kwpage.height(), Okular::Rotation0 );
        pages.append(page);
    }

    return true;
}

bool OkularOdtGenerator::doCloseDocument()
{
    delete m_doc;
    return true;
}

bool OkularOdtGenerator::canGeneratePixmap() const
{
    return true;
}

void OkularOdtGenerator::generatePixmap( Okular::PixmapRequest *request )
{
    QPixmap* pix;
    if (!m_doc) {
        pix = new QPixmap(request->width(), request->height());
        QPainter painter(pix);
        painter.fillRect(0 ,0 , request->width(), request->height(), Qt::white);
    } else {

        // use shape manager from canvasItem even for QWidget environments
        // if using the shape manager from one of the views there is no guarantee
        // that the view, its canvas and the shapemanager is not destroyed in between
        KoShapeManager* shapeManager = static_cast<KWCanvasItem*>(m_doc->documentPart()->canvasItem(m_doc))->shapeManager();

        KWPageManager *pageManager = m_doc->pageManager();

        KWPage page = pageManager->pages().at(request->pageNumber());
        pix = new QPixmap(request->width(), request->height());
        QPainter painter(pix);

        QSize rSize(request->width(), request->height());

        pix = new QPixmap();
        pix->convertFromImage(page.thumbnail(rSize, shapeManager));
    }

// API change
#if OKULAR_IS_VERSION(0, 16, 60)
    request->page()->setPixmap( request->observer(), pix );
#else
    request->page()->setPixmap( request->id(), pix );
#endif

    signalPixmapRequestDone( request );
}
