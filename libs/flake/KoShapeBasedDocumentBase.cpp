/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006, 2010 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoShapeBasedDocumentBase.h"
#include "KoDocumentResourceManager.h"
#include "KoShapeRegistry.h"

#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>

class KoShapeBasedDocumentBasePrivate
{
public:
    KoShapeBasedDocumentBasePrivate()
        : resourceManager(new KoDocumentResourceManager())
    {
        KoShapeRegistry *registry = KoShapeRegistry::instance();
        foreach (const QString &id, registry->keys()) {
            KoShapeFactoryBase *shapeFactory = registry->value(id);
            shapeFactory->newDocumentResourceManager(resourceManager);
        }
        // read persistent application wide resources
        KSharedConfigPtr config = KSharedConfig::openConfig();
        if (config->hasGroup("Misc")) {
            KConfigGroup miscGroup = config->group("Misc");
            const qreal pasteOffset = miscGroup.readEntry("CopyOffset", 10.0);
            resourceManager->setPasteOffset(pasteOffset);
            const bool pasteAtCursor = miscGroup.readEntry("PasteAtCursor", true);
            resourceManager->enablePasteAtCursor(pasteAtCursor);
            const uint grabSensitivity = miscGroup.readEntry("GrabSensitivity", 3);
            resourceManager->setGrabSensitivity(grabSensitivity);
            const uint handleRadius = miscGroup.readEntry("HandleRadius", 3);
            resourceManager->setHandleRadius(handleRadius);
        }
    }

    ~KoShapeBasedDocumentBasePrivate()
    {
        delete resourceManager;
    }

    KoDocumentResourceManager *resourceManager;
};

KoShapeBasedDocumentBase::KoShapeBasedDocumentBase()
    : d(new KoShapeBasedDocumentBasePrivate())
{
}

KoShapeBasedDocumentBase::~KoShapeBasedDocumentBase()
{
    delete d;
}

void KoShapeBasedDocumentBase::shapesRemoved(const QList<KoShape *> & /*shapes*/, KUndo2Command * /*command*/)
{
}

KoDocumentResourceManager *KoShapeBasedDocumentBase::resourceManager() const
{
    return d->resourceManager;
}
