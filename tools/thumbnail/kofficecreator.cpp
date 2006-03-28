/*  This file is part of the KDE libraries
    Copyright (C) 2002 Simon MacMullen

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

// $Id$

#include <time.h>

#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QTimerEvent>

#include <kapplication.h>
#include <kfileitem.h>
#include <ktrader.h>
#include <klibloader.h>
#include <kparts/part.h>
#include <kparts/componentfactory.h>

#include "kofficecreator.h"
#include <KoStore.h>
#include <KoDocument.h>
#include <QAbstractEventDispatcher>
#include "koffice_export.h"

extern "C"
{
    KOFFICETOOLS_EXPORT ThumbCreator *new_creator()
    {
        return new KOfficeCreator;
    }
}

KOfficeCreator::KOfficeCreator()
    : m_doc(0)
{
}

KOfficeCreator::~KOfficeCreator()
{
    delete m_doc;
}

bool KOfficeCreator::create(const QString &path, int width, int height, QImage &img)
{
    KoStore* store = KoStore::createStore(path, KoStore::Read);

    if ( store && ( store->open( QString("Thumbnails/thumbnail.png") ) || store->open( QString("preview.png") ) ) )
    {
        // Hooray! No long delay for the user...
        QByteArray bytes = store->read(store->size());
        store->close();
        delete store;
        return img.loadFromData(bytes);
    }
    delete store;

    QString mimetype = KMimeType::findByPath( path )->name();

    m_doc = KParts::ComponentFactory::createPartInstanceFromQuery<KoDocument>( mimetype, QString::null);

    if (!m_doc) return false;

    connect(m_doc, SIGNAL(completed()), SLOT(slotCompleted()));

    KUrl url;
    url.setPath( path );
    m_doc->setCheckAutoSaveFile( false );
    m_doc->setAutoErrorHandlingEnabled( false ); // don't show message boxes
    if ( !m_doc->openURL( url ) )
        return false;
    m_completed = false;
    startTimer(5000);
    while (!m_completed)
        kapp->processOneEvent();
    QAbstractEventDispatcher::instance()->unregisterTimers(this);

    // render the page on a bigger pixmap and use smoothScale,
    // looks better than directly scaling with the QPainter (malte)
    QPixmap pix;
    if (width > 400)
    {
        pix = m_doc->generatePreview(QSize(width, height));
    }
    else
    {
        pix = m_doc->generatePreview(QSize(400, 400));
    }

    img = pix.convertToImage();
    return true;
}

void KOfficeCreator::timerEvent(QTimerEvent *)
{
    m_doc->closeURL();
    m_completed = true;
}

void KOfficeCreator::slotCompleted()
{
    m_completed = true;
}

ThumbCreator::Flags KOfficeCreator::flags() const
{
    return (Flags)(DrawFrame | BlendIcon);
}

#include "kofficecreator.moc"

