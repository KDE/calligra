/*
   SPDX-FileCopyrightText: 2006 Boudewijn Rempt (boud@valdyas.org)
   SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>
   SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoDocumentResourceManager.h"

#include <FlakeDebug.h>
#include <QVariant>
#include <kundo2stack.h>

#include "KoResourceManager_p.h"
#include "KoShape.h"
#include "KoShapeController.h"

class Q_DECL_HIDDEN KoDocumentResourceManager::Private
{
public:
    KoResourceManager manager;
};

KoDocumentResourceManager::KoDocumentResourceManager(QObject *parent)
    : QObject(parent)
    , d(new Private())
{
}

KoDocumentResourceManager::~KoDocumentResourceManager()
{
    delete d;
}

void KoDocumentResourceManager::setResource(int key, const QVariant &value)
{
    d->manager.setResource(key, value);
    Q_EMIT resourceChanged(key, value);
}

QVariant KoDocumentResourceManager::resource(int key) const
{
    return d->manager.resource(key);
}

void KoDocumentResourceManager::setResource(int key, const KoColor &color)
{
    QVariant v;
    v.setValue(color);
    setResource(key, v);
}

void KoDocumentResourceManager::setResource(int key, KoShape *shape)
{
    QVariant v;
    v.setValue(shape);
    setResource(key, v);
}

void KoDocumentResourceManager::setResource(int key, const KoUnit &unit)
{
    QVariant v;
    v.setValue(unit);
    setResource(key, v);
}

KoColor KoDocumentResourceManager::koColorResource(int key) const
{
    return d->manager.koColorResource(key);
}

bool KoDocumentResourceManager::boolResource(int key) const
{
    return d->manager.boolResource(key);
}

int KoDocumentResourceManager::intResource(int key) const
{
    return d->manager.intResource(key);
}

QString KoDocumentResourceManager::stringResource(int key) const
{
    return d->manager.stringResource(key);
}

QSizeF KoDocumentResourceManager::sizeResource(int key) const
{
    return d->manager.sizeResource(key);
}

bool KoDocumentResourceManager::hasResource(int key) const
{
    return d->manager.hasResource(key);
}

void KoDocumentResourceManager::clearResource(int key)
{
    d->manager.clearResource(key);
    QVariant empty;
    Q_EMIT resourceChanged(key, empty);
}

KUndo2Stack *KoDocumentResourceManager::undoStack() const
{
    if (!hasResource(UndoStack))
        return nullptr;
    return static_cast<KUndo2Stack *>(resource(UndoStack).value<void *>());
}

void KoDocumentResourceManager::setHandleRadius(int handleRadius)
{
    // do not allow arbitrary small handles
    if (handleRadius < 3)
        handleRadius = 3;
    setResource(HandleRadius, QVariant(handleRadius));
}

int KoDocumentResourceManager::handleRadius() const
{
    if (hasResource(HandleRadius))
        return intResource(HandleRadius);
    return 3; // default value.
}
void KoDocumentResourceManager::setGrabSensitivity(int grabSensitivity)
{
    // do not allow arbitrary small grab sensitivity
    if (grabSensitivity < 3)
        grabSensitivity = 3;
    setResource(GrabSensitivity, QVariant(grabSensitivity));
}

int KoDocumentResourceManager::grabSensitivity() const
{
    if (hasResource(GrabSensitivity))
        return intResource(GrabSensitivity);
    return 3; // default value
}

void KoDocumentResourceManager::setPasteOffset(qreal pasteOffset)
{
    setResource(PasteOffset, QVariant(pasteOffset));
}

qreal KoDocumentResourceManager::pasteOffset() const
{
    return resource(PasteOffset).toDouble();
}

void KoDocumentResourceManager::enablePasteAtCursor(bool enable)
{
    setResource(PasteAtCursor, QVariant(enable));
}

bool KoDocumentResourceManager::pasteAtCursor() const
{
    return resource(PasteAtCursor).toBool();
}

void KoDocumentResourceManager::setUndoStack(KUndo2Stack *undoStack)
{
    QVariant variant;
    variant.setValue<void *>(undoStack);
    setResource(UndoStack, variant);
}

KoImageCollection *KoDocumentResourceManager::imageCollection() const
{
    if (!hasResource(ImageCollection))
        return nullptr;
    return static_cast<KoImageCollection *>(resource(ImageCollection).value<void *>());
}

void KoDocumentResourceManager::setImageCollection(KoImageCollection *ic)
{
    QVariant variant;
    variant.setValue<void *>(ic);
    setResource(ImageCollection, variant);
}

KoDocumentBase *KoDocumentResourceManager::odfDocument() const
{
    if (!hasResource(OdfDocument))
        return nullptr;
    return static_cast<KoDocumentBase *>(resource(OdfDocument).value<void *>());
}

void KoDocumentResourceManager::setOdfDocument(KoDocumentBase *currentDocument)
{
    QVariant variant;
    variant.setValue<void *>(currentDocument);
    setResource(OdfDocument, variant);
}

KoShapeController *KoDocumentResourceManager::shapeController() const
{
    if (!hasResource(ShapeController))
        return nullptr;
    return resource(ShapeController).value<KoShapeController *>();
}

void KoDocumentResourceManager::setShapeController(KoShapeController *shapeController)
{
    setResource(ShapeController, QVariant::fromValue(shapeController));
}
