/*
 * This file is part of Calligra
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Thorsten Zachmann thorsten.zachmann@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "CSThumbProviderStage.h"

#include <KoPADocument.h>
#include <KoPAPageBase.h>

#include <QApplication>
#include <QEventLoop>

CSThumbProviderStage::CSThumbProviderStage(KoPADocument *doc)
: m_doc(doc)
{
}

CSThumbProviderStage::~CSThumbProviderStage()
{
}

QVector<QImage> CSThumbProviderStage::createThumbnails(const QSize &thumbSize)
{
    // make sure all is rendered before painting
    int i = 100;
    while (QCoreApplication::hasPendingEvents() && i > 0) {
        --i;
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    QVector<QImage> thumbnails;
    foreach(KoPAPageBase *page, m_doc->pages(false)) {
        thumbnails.append(m_doc->pageThumbImage(page, thumbSize));
    }

    // make sure there are no events this fixes a crash on shutdown
    i = 100;
    while (QCoreApplication::hasPendingEvents() && i > 0) {
        --i;
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    return thumbnails;
}
