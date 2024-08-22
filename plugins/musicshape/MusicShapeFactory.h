/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_SHAPE_FACTORY
#define MUSIC_SHAPE_FACTORY

#include <KoShapeFactoryBase.h>
#include <QVariantList>

class KoShape;

class MusicShapePlugin : public QObject
{
    Q_OBJECT

public:
    MusicShapePlugin(QObject *parent, const QVariantList &);
    ~MusicShapePlugin() override = default;
};

class MusicShapeFactory : public KoShapeFactoryBase
{
public:
    MusicShapeFactory();
    ~MusicShapeFactory() override = default;

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;
};

#endif
