/* This file is part of the KDE project
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoAbstractApplication.h"

KoAbstractApplication::KoAbstractApplication(QWidget *parent)
    : KoAbstractApplicationBase(parent)
{
}

KoAbstractApplication::~KoAbstractApplication()
{
}

void KoAbstractApplication::closeEvent(QCloseEvent *event)
{
    if (KoAbstractApplicationController::handleCloseEvent(event)) {
        KoAbstractApplicationBase::closeEvent(event);
    }
}

void KoAbstractApplication::goToPage(int page) { KoAbstractApplicationController::goToPage(page); }

void KoAbstractApplication::goToPreviousPage() { KoAbstractApplicationController::goToPreviousPage(); }

void KoAbstractApplication::goToNextPage() { KoAbstractApplicationController::goToNextPage(); }

void KoAbstractApplication::documentPageSetupChanged() { KoAbstractApplicationController::documentPageSetupChanged(); }

void KoAbstractApplication::closeDocument() { KoAbstractApplicationController::closeDocument(); }

bool KoAbstractApplication::doOpenDocument() { return KoAbstractApplicationController::doOpenDocument(); }

bool KoAbstractApplication::openDocument() { return KoAbstractApplicationController::openDocument(); }

bool KoAbstractApplication::openDocuments(const KoAbstractApplicationOpenDocumentArguments& args)
{
    return KoAbstractApplicationController::openDocuments(args);
}

bool KoAbstractApplication::openDocument(const QString &fileName) {
    return KoAbstractApplicationController::openDocument(fileName);
}

bool KoAbstractApplication::openDocumentAsTemplate(const QString &fileName)
{
    return KoAbstractApplicationController::openDocumentAsTemplate(fileName);
}

bool KoAbstractApplication::saveDocument() { return KoAbstractApplicationController::saveDocument(); }

bool KoAbstractApplication::saveDocumentAs() { return KoAbstractApplicationController::saveDocumentAs(); }

void KoAbstractApplication::goToNextSlide() { emit nextSlide(); }

void KoAbstractApplication::goToPreviousSlide() { emit previousSlide(); }

void KoAbstractApplication::resourceChanged(int key, const QVariant &value) {
    KoAbstractApplicationController::resourceChanged(key, value);
}

void KoAbstractApplication::addSheet() { KoAbstractApplicationController::addSheet(); }

void KoAbstractApplication::removeSheet() { KoAbstractApplicationController::removeSheet(); }

bool KoAbstractApplication::setEditingMode(bool set) { return KoAbstractApplicationController::setEditingMode(set); }

void KoAbstractApplication::toggleVirtualKeyboardVisibility() { KoAbstractApplicationController::toggleVirtualKeyboardVisibility(); }

void KoAbstractApplication::activeToolChanged(KoCanvasController* canvas, int uniqueToolId)
{
    KoAbstractApplicationController::activeToolChanged(canvas, uniqueToolId);
}
