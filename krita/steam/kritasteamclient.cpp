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

#include "kritasteamclient.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <unistd.h>
#include "steam/steam_api.h"

// Can test if SteamTenfoot=1
#define BIGPICTURE_ENVVARNAME "SteamTenfoot"


KritaSteamClient *KritaSteamClient::sm_instance = 0;
KritaSteamClient* KritaSteamClient::instance() {
    if (!sm_instance) {
        sm_instance = new KritaSteamClient(QCoreApplication::instance());
    }
    return sm_instance;
}

//-----------------------------------------------------------------------------
// Callback hook for debug text emitted from the Steam API
//-----------------------------------------------------------------------------
extern "C" void __cdecl SteamAPIDebugTextHook( int nSeverity, const char *pchDebugText )
{
    // if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
    // if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
    qDebug( pchDebugText );

    if ( nSeverity >= 1 )
    {
        // place to set a breakpoint for catching API errors
        int x = 3;
        x = x;
    }
}

//-----------------------------------------------------------------------------
// Purpose: Wrapper around SteamAPI_WriteMiniDump which can be used directly
// as a se translator
//-----------------------------------------------------------------------------
void KritaSteamClient::MiniDumpFunction(const QString& comment, unsigned int nExceptionCode, void *pException )
{
    SteamAPI_SetMiniDumpComment( comment.toUtf8().constData() );

    // The 0 here is a build ID, which is not set
#ifdef Q_OS_WIN
    SteamAPI_WriteMiniDump( nExceptionCode, (EXCEPTION_POINTERS*) pException, 0 );
#endif
}



KritaSteamClient::KritaSteamClient(QObject *parent) :
    QObject(parent),
    m_initialisedWithoutError(false),
    m_callbackTimer(0),
    m_bigPictureMode(false),
    m_gameOverlayActivatedCallback( this, &KritaSteamClient::onGameOverlayActivated ),
    m_steamShutdownCallback( this, &KritaSteamClient::onSteamShutdown )
{
}


KritaSteamClient::~KritaSteamClient()
{
}

bool KritaSteamClient::initialise(uint32 appId)
{
    const char* steamLanguage = 0;

    if (appId != k_uAppIdInvalid) {
        if (SteamAPI_RestartAppIfNecessary(appId))
        {
            // if Steam isn't running or Krita is not launched from Steam, this
            // starts the local client and launches it again.
            qDebug("Starting the local Steam client and launching through Steam");
            return false;
        }
    }

    // Check for Big Picture mode
    QByteArray bigPictureEnvVar = qgetenv(BIGPICTURE_ENVVARNAME);
    if (!bigPictureEnvVar.isEmpty()) {
        qDebug("Steam is in BIG PICTURE mode");
        if (strcmp(bigPictureEnvVar.constData(), "1")==0) {
            m_bigPictureMode = true;
        }
    }

    if (m_bigPictureMode) {
        qDebug("Steam is in BIG PICTURE mode");
    } else {
        qDebug("Steam is in not in BIG PICTURE mode");
    }


    m_initialisedWithoutError = SteamAPI_Init();
    if (m_initialisedWithoutError) {
        qDebug("Steam initialised correctly!");

        // set our debug handler
        SteamClient()->SetWarningMessageHook( &SteamAPIDebugTextHook );

        // Get Steam Language
        steamLanguage = SteamUtils()->GetSteamUILanguage();

        // Print Debug info
        qDebug() << QString("Steam UI language: ") << QString(steamLanguage);

        /* Sample code for initialising the SteamController
        char rgchCWD[1024];
        getcwd( rgchCWD, sizeof( rgchCWD ) );
        char rgchFullPath[1024];
    #if defined(_WIN32)
        _snprintf( rgchFullPath, sizeof( rgchFullPath ), "%s\\%s", rgchCWD, "controller.vdf" );
    #else
        _snprintf( rgchFullPath, sizeof( rgchFullPath ), "%s/%s", rgchCWD, "controller.vdf" );
    #endif
        qDebug("Initialising Steam Controller");
        SteamController()->Init( rgchFullPath );
        */
    } else {
        qDebug("Steam did not initialise!");
        return false;
    }
    return true;
}

void KritaSteamClient::shutdown()
{
    if(m_initialisedWithoutError) {
        //SteamController()->Shutdown();
        SteamAPI_Shutdown();
    }
}

bool KritaSteamClient::isInBigPictureMode() {
    return m_bigPictureMode;
}

// Slots
void KritaSteamClient::runCallbacks()
{
    // Call the SteamAPI callback mechanism
    // This should be at >10Hz
    SteamAPI_RunCallbacks();
}

void KritaSteamClient::mainWindowCreated()
{
    // Register >10Hz timer to process SteamAPI callback mechanism
    m_callbackTimer = new QTimer(this);
    m_callbackTimer->setInterval(100); // 10Hz
    m_callbackTimer->setSingleShot(false);
    connect(m_callbackTimer, SIGNAL(timeout()), this, SLOT(runCallbacks()));
    m_callbackTimer->start();
}

void KritaSteamClient::mainWindowBeingDestroyed()
{
    if (m_callbackTimer != 0) {
        m_callbackTimer->disconnect();
        m_callbackTimer->stop();
        delete m_callbackTimer;
    }
}


// Steam Callbacks

void KritaSteamClient::onGameOverlayActivated( GameOverlayActivated_t *callback )
{
    if (callback->m_bActive) {
        qDebug("Emitting OverlayActivated");
        emit(overlayActivated());
    } else {
        qDebug("Emitting OverlayDeactivated");
        emit(overlayDeactivated());
    }
}

void KritaSteamClient::onSteamShutdown( SteamShutdown_t *callback )
{
    qDebug("Steam shutdown request, TODO: shutdown");
}

#include "kritasteamclient.moc"
