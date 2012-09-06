/*
 * imageshare.cpp -- Part of Krita
 *
 * Copyright (c) 2012 Boudewijn Rempt (boud@valdyas.org)
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

#include "imageshare.h"

#include <klocale.h>
#include <kcomponentdata.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kis_debug.h>
#include <kpluginfactory.h>
#include <kstandardaction.h>
#include <kactioncollection.h>

K_PLUGIN_FACTORY(ImageShareFactory, registerPlugin<ImageShare>();)
K_EXPORT_PLUGIN(ImageShareFactory("krita"))

ImageShare::ImageShare(QObject *parent, const QVariantList &)
        : KParts::Plugin(parent)
{
    if (parent->inherits("KisView2")) {
        setXMLFile(KStandardDirs::locate("data", "kritaplugins/imageshare.rc"), true);

        KAction *action  = new KAction(i18n("Share on Deviant Art..."), this);
        actionCollection()->addAction("imageshare", action);
        connect(action, SIGNAL(triggered()), this, SLOT(slotImageShare()));

    }
}

ImageShare::~ImageShare()
{
    m_view = 0;
}

void ImageShare::slotImageShare()
{
}

#include "imageshare.moc"
