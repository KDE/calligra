/* This file is part of the KDE project
 * Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 * Copyright (C) 2000-2005 David Faure <faure@kde.org>
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2010-2013 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2011 Inge Wallin <ingwa@kogmbh.com>
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

#include "KoDocumentBase.h"
#include <kundo2stack.h>

class KoDocumentBase::Private {
public:
    QString fileName;
    bool modified;
    KUndo2Stack *undoStack;
};


KoDocumentBase::KoDocumentBase(KUndo2Stack *undoStack, QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    d->fileName = tr("Untitled");
    d->modified = false;
    d->undoStack = undoStack;
}

KoDocumentBase::~KoDocumentBase()
{
    delete d;
}

void KoDocumentBase::setFileName(const QString &fileName)
{
    d->fileName = fileName;
}

QString &KoDocumentBase::fileName()
{
    return d->fileName;
}

void KoDocumentBase::setModified(bool modified)
{
    d->modified = modified;
}

bool KoDocumentBase::modified()
{
    return d->modified;
}
