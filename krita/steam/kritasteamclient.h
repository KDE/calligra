/* This file is part of the KDE project
 * Copyright (C) 2014 Stuart Dickson <stuartmd@kogmbh.com>
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

#ifndef KRITASTEAMCLIENT_H
#define KRITASTEAMCLIENT_H






#include <QObject>

#include "steam/steam_api.h"
#include "sketch/krita_sketch_export.h"

// forward declaration
class QTimer;

/**
 * Handles interaction with the Steam API
 */
class KRITA_SKETCH_EXPORT KritaSteamClient : public QObject
{
    Q_OBJECT
public:
    static KritaSteamClient* instance();

    virtual ~KritaSteamClient();

    // Steam Functions
    bool initialise(uint32 appId);
    void shutdown();
    bool isInBigPictureMode();

private:
    // Singleton has private constructor
    explicit KritaSteamClient(QObject *parent = 0);
    static KritaSteamClient *sm_instance;

    bool m_bigPictureMode;

    // Steam callbacks
    STEAM_CALLBACK( KritaSteamClient, onGameOverlayActivated, GameOverlayActivated_t, m_gameOverlayActivatedCallback );
    STEAM_CALLBACK( KritaSteamClient, onSteamShutdown, SteamShutdown_t, m_steamShutdownCallback );

    bool m_initialisedWithoutError;
    QTimer* m_callbackTimer;
signals:
    void overlayActivated();
    void overlayDeactivated();

public slots:
    void runCallbacks();
    void mainWindowCreated();
    void mainWindowBeingDestroyed();
};

#endif // KRITASTEAMCLIENT_H
