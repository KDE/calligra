/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoTemplate.h"

#include <QImage>
#include <QPixmap>

#include <MainDebug.h>

#include <KIconLoader>

KoTemplate::KoTemplate(const QString &name,
                       const QString &description,
                       const QString &file,
                       const QString &picture,
                       const QString &fileName,
                       const QString &_measureSystem,
                       const QString &color,
                       const QString &swatch,
                       const QString &variantName,
                       bool wide,
                       bool hidden,
                       bool touched)
    : m_name(name)
    , m_descr(description)
    , m_file(file)
    , m_picture(picture)
    , m_fileName(fileName)
    , m_color(color)
    , m_swatch(swatch)
    , m_variantName(variantName)
    , m_wide(wide)
    , m_hidden(hidden)
    , m_touched(touched)
    , m_cached(false)
    , m_measureSystem(_measureSystem)
{
}

const QPixmap &KoTemplate::loadPicture()
{
    if (m_cached)
        return m_pixmap;
    m_cached = true;
    if (m_picture[0] == '/') {
        QImage img(m_picture);
        if (img.isNull()) {
            qWarning() << "Couldn't find icon " << m_picture;
            m_pixmap = QPixmap();
            return m_pixmap;
        }
        const int maxHeightWidth = 128; // ### TODO: some people would surely like to have 128x128
        if (img.width() > maxHeightWidth || img.height() > maxHeightWidth) {
            img = img.scaled(maxHeightWidth, maxHeightWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        m_pixmap = QPixmap::fromImage(img);
        return m_pixmap;
    } else { // relative path
        m_pixmap = KIconLoader::global()->loadIcon(m_picture, KIconLoader::Desktop, 128);
        return m_pixmap;
    }
}
