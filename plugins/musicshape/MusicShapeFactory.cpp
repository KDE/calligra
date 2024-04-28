/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include <QFontDatabase>
#include <QStringList>

#include "MusicDebug.h"
#include <KLocalizedString>
#include <KPluginFactory>
#include <KoIcon.h>
#include <KoResourcePaths.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

#include "MusicShape.h"
#include "MusicToolFactory.h"
#include "SimpleEntryToolFactory.h"

#include "MusicShapeFactory.h"

K_PLUGIN_FACTORY_WITH_JSON(MusicShapePluginFactory, "calligra_shape_music.json", registerPlugin<MusicShapePlugin>();)

MusicShapePlugin::MusicShapePlugin(QObject *, const QVariantList &)
{
    KoShapeRegistry::instance()->add(new MusicShapeFactory());
    KoToolRegistry::instance()->add(new MusicToolFactory());
    KoToolRegistry::instance()->add(new SimpleEntryToolFactory());
}

MusicShapeFactory::MusicShapeFactory()
    : KoShapeFactoryBase(MusicShapeId, i18n("Music Shape"))
{
    setToolTip(i18n("A shape which provides a music editor"));
    setIconName(koIconNameNeededWithSubs("icon for the Music Shape", "musicshape", "music-note-16th"));
    setXmlElementNames("http://www.calligra.org/music", QStringList("shape"));
    setLoadingPriority(1);
}

KoShape *MusicShapeFactory::createDefaultShape(KoDocumentResourceManager *) const
{
    static bool loadedFont = false;
    if (!loadedFont) {
        QString fontFile = KoResourcePaths::locate("data", "calligra_shape_music/fonts/Emmentaler-14.ttf");
        if (QFontDatabase::addApplicationFont(fontFile) == -1) {
            warnMusic << "Could not load emmentaler font";
        }
        loadedFont = true;
    }
    MusicShape *shape = new MusicShape();
    shape->setSize(QSizeF(400, 300));
    shape->setShapeId(MusicShapeId);
    return shape;
}

bool MusicShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    return (e.localName() == "shape") && (e.namespaceURI() == "http://www.calligra.org/music");
}

#include "MusicShapeFactory.moc"
