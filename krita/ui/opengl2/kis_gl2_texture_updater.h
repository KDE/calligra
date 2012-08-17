/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#ifndef KIS_GL2_TEXTURE_UPDATER_H
#define KIS_GL2_TEXTURE_UPDATER_H

#include <QThread>

#include "kis_types.h"

class QRect;
class KisGL2TextureUpdater : public QThread
{
    Q_OBJECT
public:
    explicit KisGL2TextureUpdater(KisImageWSP image, uint texture, QObject* parent = 0);
    virtual ~KisGL2TextureUpdater();

public Q_SLOTS:
    void imageUpdated(const QRect& rect);
    void timeout();

protected:
    virtual void run();

private:
    class Private;
    Private * const d;
};

#endif // KIS_GL2_TEXTURE_UPDATER_H
