/*
 *  Copyright (c) 2012 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "flipbook_item.h"

#include <kis_doc2.h>
#include <kis_part2.h>

#include <QImage>
#include <QFileInfo>
#include <QSize>

FlipbookItem::FlipbookItem(const QString &filename)
    : m_filename(filename)
    , m_document(0)
    , m_part(0)
{
    m_icon.load(filename);
    m_imageSize = m_icon.size();
    m_icon = m_icon.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

FlipbookItem::~FlipbookItem()
{
    delete m_part;
    delete m_document;
}

KisDoc2 *FlipbookItem::document()
{
    if (!m_document) {
        KisPart2 *part = new KisPart2(0);
        m_document = new KisDoc2(part);
        part->setDocument(m_document);
        m_document->openUrl(KUrl(m_filename));
    }
    return m_document;
}

QImage FlipbookItem::icon() const
{
    return m_icon;
}

QString FlipbookItem::filename() const
{
    return m_filename;
}

QString FlipbookItem::name() const
{
    QFileInfo info(m_filename);
    return info.fileName();
}

int FlipbookItem::width() const
{
    return m_imageSize.width();
}

int FlipbookItem::height() const
{
    return m_imageSize.height();
}

const QSize &FlipbookItem::size() const
{
    return m_imageSize;
}

