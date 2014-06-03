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
#include <QString>

#ifdef Q_CC_GNU
// The Steam API has a lot of interfaces containing default, non-virtual dtors.
// Since we cannot fix the Steam API, disable the warning about it instead.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
#include "steam/steam_api.h"
#ifdef Q_CC_GNU
#pragma GCC diagnostic pop
#endif

#include "sketch/krita_sketch_export.h"

// forward declaration
class QTimer;
class KisSteamCloudStorage;

/**
 * Handles interaction with the Steam API
 */
class KRITA_SKETCH_EXPORT KritaSteamClient : public QObject
{
    Q_OBJECT
public:
    static KritaSteamClient* instance();
    static void MiniDumpFunction(const QString& comment, unsigned int nExceptionCode, void *pException);

    virtual ~KritaSteamClient();

    // Steam Functions
    /**
     * @brief Initialise the Steam API
     * @param appId
     * @return true if Krita was launched from Steam, false otherwise
     */
    bool initialise(uint32 appId = 0);
    void shutdown();
    bool restartInSteam();

    // Steam Cloud Functions
    KisSteamCloudStorage* remoteStorage();
    bool checkCloudStorage();

    bool isInBigPictureMode();
    bool isInitialised();

private:
    // Singleton has private constructor
    explicit KritaSteamClient(QObject *parent = 0);
    static KritaSteamClient *sm_instance;

    class Private;
    Private * const d;

    // Steam callbacks
    STEAM_CALLBACK( KritaSteamClient, onGameOverlayActivated, GameOverlayActivated_t, m_gameOverlayActivatedCallback );
    STEAM_CALLBACK( KritaSteamClient, onSteamShutdown, SteamShutdown_t, m_steamShutdownCallback );

signals:
    void overlayActivated();
    void overlayDeactivated();

public slots:
    void runCallbacks();
    void mainWindowCreated();
    void mainWindowBeingDestroyed();
    void showDialog();
    void showOpenFileDialog();
    void showSaveAsFileDialog();
    void showWorkshopDialog();
};

#endif // KRITASTEAMCLIENT_H
