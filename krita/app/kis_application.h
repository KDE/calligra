/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIS_APPLICATION_H
#define KIS_APPLICATION_H

#include <kapplication.h>

class QSplashScreen;

/**
 * XXX: mostly ported stuff from KoApplication, should move back there when
 * everything is de-parted
 */
class KisApplication : public KApplication
{
    Q_OBJECT
public:
    KisApplication();

    static void createCommandLineOptions();

    /**
     *  Destructor.
     */
    virtual ~KoApplication();

    // Overridden to handle exceptions from event handlers.
    bool notify(QObject *receiver, QEvent *event);

};

#endif // KIS_APPLICATION_H
