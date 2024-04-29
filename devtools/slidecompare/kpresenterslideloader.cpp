/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "kpresenterslideloader.h"
#include <KPluginFactory>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <QDebug>

KPresenterSlideLoader::KPresenterSlideLoader(QObject *parent)
    : SlideLoader(parent)
{
    m_doc = 0;
    version = 0;
}

KPresenterSlideLoader::~KPresenterSlideLoader()
{
    close();
}
void KPresenterSlideLoader::close()
{
    delete m_doc;
    m_doc = 0;
    version++;
    Q_EMIT slidesChanged();
}

void KPresenterSlideLoader::open(const QString &path)
{
    close();

    KPluginFactory *factory = KPluginLoader("calligrastagepart", cd).factory();
    if (!factory) {
        qDebug() << "could not load calligrastagepart";
        close();
        return;
    }
    KoPADocument *doc = factory->create<KoPADocument>();
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
    Q_EMIT slidesChanged();
}
int KPresenterSlideLoader::numberOfSlides()
{
    return (m_doc) ? m_doc->pageCount() : 0;
}
QSize KPresenterSlideLoader::slideSize()
{
    if (!m_doc)
        return QSize();
    KoPAPageBase *page = m_doc->pages().value(0);
    if (!page)
        return QSize();
    return page->size().toSize();
}
QPixmap KPresenterSlideLoader::loadSlide(int number, const QSize &maxsize)
{
    if (!m_doc)
        return QPixmap();
    KoPAPageBase *page = m_doc->pages().value(number);
    if (!page)
        return QPixmap();
    return page->thumbnail(maxsize);
}
