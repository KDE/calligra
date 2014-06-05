/* This file is part of the KDE project
 * Copyright (C) 2012 Boudewijn Rempt <boud@kogmbh.com>
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
#ifndef KRITA_STEAM_COMPONENT_H
#define KRITA_STEAM_COMPONENT_H

#include <QDeclarativeItem>

#include "sketch/krita_sketch_export.h"

class KisSteamComponent : public QDeclarativeItem
{
    Q_OBJECT
    /* PROPERTIES
    Q_PROPERTY(QObject* selectionManager READ selectionManager NOTIFY viewChanged)
    Q_PROPERTY(QObject* selectionExtras READ selectionExtras NOTIFY viewChanged)
    Q_PROPERTY(QObject* view READ view NOTIFY viewChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(QString fileTitle READ fileTitle NOTIFY fileChanged);
    Q_PROPERTY(bool modified READ isModified NOTIFY modifiedChanged)

    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged);
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged);

    Q_PROPERTY(int imageHeight READ imageHeight NOTIFY imageSizeChanged)
    Q_PROPERTY(int imageWidth READ imageWidth NOTIFY imageSizeChanged)
    */

public:
    KisSteamComponent(QDeclarativeItem* parent = 0);
    virtual ~KisSteamComponent();

    //QObject* doc() const;
    //QObject* view() const;

public Q_SLOTS:
    void saveAs(const QString& fileName, const QString& mimeType);

Q_SIGNALS:
    //void loadingFinished();


private:
    class Private;
    Private * const d;

    // Q_PRIVATE_SLOT(d, void imageUpdated(const QRect &updated))
};

#endif // KRITA_STEAM_COMPONENT_H
