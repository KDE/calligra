/*
   SPDX-FileCopyrightText: 2006 Boudewijn Rempt (boud@valdyas.org)
   SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>
   SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoCanvasResourceManager.h"

#include <FlakeDebug.h>
#include <QVariant>

#include "KoResourceManager_p.h"
#include "KoShape.h"
#include "KoShapeStroke.h"
#include <KoColorSpaceRegistry.h>

class Q_DECL_HIDDEN KoCanvasResourceManager::Private
{
public:
    KoResourceManager manager;
};

KoCanvasResourceManager::KoCanvasResourceManager(QObject *parent)
    : QObject(parent)
    , d(new Private())
{
    const KoColorSpace *cs = KoColorSpaceRegistry::instance()->rgb8();
    setForegroundColor(KoColor(Qt::black, cs));
    setBackgroundColor(KoColor(Qt::white, cs));
    setResource(ApplicationSpeciality, NoSpecial);
}

KoCanvasResourceManager::~KoCanvasResourceManager()
{
    delete d;
}

void KoCanvasResourceManager::setResource(int key, const QVariant &value)
{
    d->manager.setResource(key, value);
    Q_EMIT canvasResourceChanged(key, value);
}

QVariant KoCanvasResourceManager::resource(int key) const
{
    return d->manager.resource(key);
}

void KoCanvasResourceManager::setResource(int key, const KoColor &color)
{
    QVariant v;
    v.setValue(color);
    setResource(key, v);
}

void KoCanvasResourceManager::setResource(int key, KoShape *shape)
{
    QVariant v;
    v.setValue(shape);
    setResource(key, v);
}

void KoCanvasResourceManager::setResource(int key, const KoUnit &unit)
{
    QVariant v;
    v.setValue(unit);
    setResource(key, v);
}

KoColor KoCanvasResourceManager::koColorResource(int key) const
{
    return d->manager.koColorResource(key);
}

void KoCanvasResourceManager::setForegroundColor(const KoColor &color)
{
    setResource(ForegroundColor, color);
}

KoColor KoCanvasResourceManager::foregroundColor() const
{
    return koColorResource(ForegroundColor);
}

void KoCanvasResourceManager::setBackgroundColor(const KoColor &color)
{
    setResource(BackgroundColor, color);
}

KoColor KoCanvasResourceManager::backgroundColor() const
{
    return koColorResource(BackgroundColor);
}

KoShape *KoCanvasResourceManager::koShapeResource(int key) const
{
    return d->manager.koShapeResource(key);
}

KoUnit KoCanvasResourceManager::unitResource(int key) const
{
    return resource(key).value<KoUnit>();
}

void KoCanvasResourceManager::setActiveStroke(const KoShapeStroke &stroke)
{
    QVariant v;
    v.setValue(stroke);
    setResource(ActiveStroke, v);
}

KoShapeStroke KoCanvasResourceManager::activeStroke() const
{
    if (!d->manager.hasResource(ActiveStroke)) {
        KoShapeStroke empty;
        return empty;
    }
    return resource(ActiveStroke).value<KoShapeStroke>();
}

bool KoCanvasResourceManager::boolResource(int key) const
{
    return d->manager.boolResource(key);
}

int KoCanvasResourceManager::intResource(int key) const
{
    return d->manager.intResource(key);
}

QString KoCanvasResourceManager::stringResource(int key) const
{
    return d->manager.stringResource(key);
}

QSizeF KoCanvasResourceManager::sizeResource(int key) const
{
    return d->manager.sizeResource(key);
}

bool KoCanvasResourceManager::hasResource(int key) const
{
    return d->manager.hasResource(key);
}

void KoCanvasResourceManager::clearResource(int key)
{
    d->manager.clearResource(key);
    QVariant empty;
    Q_EMIT canvasResourceChanged(key, empty);
}
