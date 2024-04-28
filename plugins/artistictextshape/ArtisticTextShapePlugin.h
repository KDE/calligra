/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ARTISTICTEXTSHAPEPLUGIN_H
#define ARTISTICTEXTSHAPEPLUGIN_H

#include <QObject>
#include <QVariantList>

class ArtisticTextShapePlugin : public QObject
{
    Q_OBJECT

public:
    ArtisticTextShapePlugin(QObject *parent, const QVariantList &);
    ~ArtisticTextShapePlugin() override;
};

#endif // ARTISTICTEXTSHAPEPLUGIN_H
