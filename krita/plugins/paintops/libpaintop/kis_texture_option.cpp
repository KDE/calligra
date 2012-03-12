/* This file is part of the KDE project
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2008
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
#include "kis_texture_option.h"

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QBuffer>

#include <klocale.h>

#include <kis_pattern.h>

class KisTextureOptionWidget : public QWidget
{
public:

    KisTextureOptionWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
    }
};

KisTextureOption::KisTextureOption(QObject *parent) :
    KisPaintOpOption(i18n("Pattern"), KisPaintOpOption::textureCategory(), true)
{
    setChecked(false);
    m_optionWidget = new KisTextureOptionWidget;
    m_optionWidget->hide();
    setConfigurationPage(m_optionWidget);
}

KisTextureOption::~KisTextureOption()
{
    if (m_ownsPattern) {
        delete m_pattern;
    }
}


void KisTextureOption::writeOptionSetting(KisPropertiesConfiguration* setting) const
{
    if (!m_pattern) return;

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    m_pattern->image().save(&buffer, "PNG"); // writes image into ba in PNG format

    setting->setProperty("Texture/Pattern/Pattern", ba.toBase64());
    setting->setProperty("Texture/Pattern/PatternFileName", m_pattern->filename());
    setting->setProperty("Texture/Pattern/Name", m_pattern->name());
    setting->setProperty("Texture/Pattern/Scale", m_scale);
    setting->setProperty("Texture/Pattern/Rotation", m_rotation);
    setting->setProperty("Texture/Pattern/Offset", m_offset);
    setting->setProperty("Texture/Pattern/Strength", m_strength);
    setting->setProperty("Texture/Pattern/Invert", m_invert);
    setting->setProperty("Texture/Pattern/Channel", int(m_activeChannel));
}

void KisTextureOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
    QString pattern = setting->getString("Texture/Pattern/Pattern");
    QByteArray ba = QByteArray::fromBase64(pattern.toAscii());
    QImage img;
    img.loadFromData(ba, "PNG");
    QString name = setting->getString("Texture/Pattern/Name");
    if (name.isEmpty()) {
        QString name = setting->getString("Texture/Pattern/FileName");
    }
    m_pattern = new KisPattern(img, name);
    m_ownsPattern = true;
    m_scale = setting->getDouble("Texture/Pattern/Scale");
    m_rotation = setting->getDouble("Texture/Pattern/Rotation");
    m_offset = setting->getInt("Texture/Pattern/Offset");
    m_strength = setting->getDouble("Texture/Pattern/Strength");
    m_invert = setting->getBool("Texture/Pattern/Invert");
    m_activeChannel = (TextureChannel)setting->getInt("Texture/Pattern/Channel");
}
