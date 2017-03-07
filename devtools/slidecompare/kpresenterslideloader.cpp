/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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
#include "kpresenterslideloader.h"
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <kpluginfactory.h>
#include <QDebug>

KPresenterSlideLoader::KPresenterSlideLoader(QObject* parent) :SlideLoader(parent) {
    m_doc = 0;
    version = 0;
}

KPresenterSlideLoader::~KPresenterSlideLoader() {
    close();
}
void KPresenterSlideLoader::close() {
    delete m_doc;
    m_doc = 0;
    version++;
    emit slidesChanged();
}

void
KPresenterSlideLoader::open(const QString& path)
{
    close();

    KPluginFactory *factory = KPluginLoader("calligrastagepart", cd).factory();
    if (!factory) {
        qDebug() << "could not load calligrastagepart";
        close();
        return;
    }
    KoPADocument* doc = factory->create<KoPADocument>();
    m_doc = doc;
    KUrl url;
    url.setPath(path);
    doc->setCheckAutoSaveFile(false);
    doc->setAutoErrorHandlingEnabled(true); // show error dialogs
    if (!doc->openUrl(url)) {
        qDebug() << "could not open " << path;
        close();
        return;
    }
    doc->setReadWrite(false);
    doc->setAutoSave(0);
    emit slidesChanged();
}
int
KPresenterSlideLoader::numberOfSlides() {
    return (m_doc) ?m_doc->pageCount() :0;
}
QSize KPresenterSlideLoader::slideSize() {
    if (!m_doc) return QSize();
    KoPAPageBase* page = m_doc->pages().value(0);
    if (!page) return QSize();
    return page->size().toSize();
}
QPixmap KPresenterSlideLoader::loadSlide(int number, const QSize& maxsize) {
    if (!m_doc) return QPixmap();
    KoPAPageBase* page = m_doc->pages().value(number);
    if (!page) return QPixmap();
    return page->thumbnail(maxsize);
}
