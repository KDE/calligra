/*
 * This file is part of Calligra
 *
 * SPDX-FileCopyrightText: 2011 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Thorsten Zachmann thorsten.zachmann@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#include "CSThumbProviderStage.h"

#include <KoPADocument.h>
#include <KoPAPageBase.h>

#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QEventLoop>

CSThumbProviderStage::CSThumbProviderStage(KoPADocument *doc)
    : m_doc(doc)
{
}

CSThumbProviderStage::~CSThumbProviderStage() = default;

QVector<QImage> CSThumbProviderStage::createThumbnails(const QSize &thumbSize)
{
    //// make sure all is rendered before painting
    // int i = 100;
    // while (QCoreApplication::eventDispatcher()->hasPendingEvents() && i > 0) {
    //     --i;
    //     QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    // }

    QVector<QImage> thumbnails;
    foreach (KoPAPageBase *page, m_doc->pages(false)) {
        thumbnails.append(m_doc->pageThumbImage(page, thumbSize));
    }

    // make sure there are no events this fixes a crash on shutdown
    // i = 100;
    // while (QCoreApplication::eventDispatcher()->hasPendingEvents() && i > 0) {
    //    --i;
    //    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    //}
    return thumbnails;
}
