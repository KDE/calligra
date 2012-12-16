/* This file is part of the KDE project
   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "CoverImage.h"

#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoStoreDevice.h>

#include <kdebug.h>
#include <kmimetype.h>
#include <klocale.h>
#include <QFile>
#include <QFileDialog>

QByteArray CoverImage::m_coverData;
QString CoverImage::m_coverMimeType;

CoverImage::CoverImage()
{
}

bool CoverImage::saveCoverImage(KoStore *store, KoXmlWriter *manifestWriter)
{
    // There is no cover to save.
    if (m_coverData.isEmpty())
        return true;

    if (!store->open("Author-Profile/cover." + m_coverMimeType)) {
        kDebug(31000) << "Unable to open Author-Profile/cover."<<m_coverMimeType;
        return false;
    }

    KoStoreDevice device(store);
    device.write(m_coverData, m_coverData.size());
    store->close();

    const QString mimetype(KMimeType::findByPath("Author-Profile/cover." + m_coverMimeType, 0 , true)->name());
    manifestWriter->addManifestEntry("Author-Profile/cover." + m_coverMimeType, mimetype);

    return true;
}

void CoverImage::setCoverData(QString path)
{
    QFile file (path);
    if (!file.open(QIODevice::ReadOnly)) {
        kDebug(31000) << "Unable to open" << path;
    }
    QByteArray data = file.readAll();

    m_coverMimeType = path.right(3);
    m_coverData = data;

    file.close();
}

void CoverImage::getCoverPath()
{
    QString path = QFileDialog::getOpenFileName(0, i18n("Open File"),
                                                QDir::currentPath(),
                                                      i18n("Images (*.png *.xpm *.jpg)"));
    if (!path.isEmpty())
        setCoverData(path);
}
