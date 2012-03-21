/* This file is part of the KDE project
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2012
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
#include <QCheckBox>
#include <QBuffer>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QTransform>
#include <QPainter>

#include <klocale.h>

#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

#include <kis_resource_server_provider.h>
#include <kis_pattern_chooser.h>
#include <kis_slider_spin_box.h>
#include <kis_multipliers_double_slider_spinbox.h>
#include <kis_pattern.h>
#include <kis_paint_device.h>
#include <kis_fill_painter.h>
#include <kis_painter.h>
#include <kis_iterator_ng.h>
#include <kis_fixed_paint_device.h>

class KisTextureOptionWidget : public QWidget
{
public:

    KisTextureOptionWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
        QFormLayout *formLayout = new QFormLayout(this);
        chooser = new KisPatternChooser(this);
        chooser->setGrayscalePreview(true);
        chooser->setMaximumHeight(250);
        chooser->setCurrentItem(0, 0);
        formLayout->addRow(chooser);

        scaleSlider = new KisMultipliersDoubleSliderSpinBox(this);
        scaleSlider->setRange(0.0, 2.0, 2);
        scaleSlider->setValue(1.0);
        scaleSlider->addMultiplier(0.1);
        scaleSlider->addMultiplier(2);
        scaleSlider->addMultiplier(10);

        formLayout->addRow(i18n("Scale:"), scaleSlider);

        offsetSliderX = new KisSliderSpinBox(this);
        offsetSliderX->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        formLayout->addRow(i18n("Horizontal Offset:"), offsetSliderX);

        offsetSliderY = new KisSliderSpinBox(this);
        offsetSliderY->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        formLayout->addRow(i18n("Vertical Offset:"), offsetSliderY);

        strengthSlider = new KisDoubleSliderSpinBox(this);
        strengthSlider->setRange(0.0, 1.0, 2);
        strengthSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        formLayout->addRow(i18n("Strength:"), strengthSlider);

        chkInvert = new QCheckBox("", this);
        chkInvert->setChecked(false);
        formLayout->addRow(i18n("Invert Texture:"), chkInvert);

        cmbChannel = new QComboBox(this);
        cmbChannel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        QStringList channels;
        channels << i18n("Alpha") << i18n("Hue") << i18n("Saturation") << i18n("Lightness");
        cmbChannel->addItems(channels);
        formLayout->addRow(i18n("Channel:"), cmbChannel);

        setLayout(formLayout);
    }
    KisPatternChooser *chooser;
    KisMultipliersDoubleSliderSpinBox *scaleSlider;
    KisSliderSpinBox *offsetSliderX;
    KisSliderSpinBox *offsetSliderY;
    KisDoubleSliderSpinBox *strengthSlider;
    QCheckBox *chkInvert;
    QComboBox *cmbChannel;
};

KisTextureOption::KisTextureOption(QObject *)
    : KisPaintOpOption(i18n("Pattern"), KisPaintOpOption::textureCategory(), true)
{
    setChecked(false);
    m_optionWidget = new KisTextureOptionWidget;
    m_optionWidget->hide();
    setConfigurationPage(m_optionWidget);

    connect(m_optionWidget->chooser, SIGNAL(resourceSelected(KoResource*)), SLOT(resetGUI(KoResource*)));

    resetGUI(m_optionWidget->chooser->currentResource());
}

KisTextureOption::~KisTextureOption()
{
    delete m_optionWidget;
}

void KisTextureOption::writeOptionSetting(KisPropertiesConfiguration* setting) const
{
    if (!m_optionWidget->chooser->currentResource()) return;
    KisPattern *pattern = static_cast<KisPattern*>(m_optionWidget->chooser->currentResource());
    if (!pattern) return;

    qreal scale = m_optionWidget->scaleSlider->value();
    int offsetX = m_optionWidget->offsetSliderX->value();
    int offsetY = m_optionWidget->offsetSliderY->value();
    qreal strength = m_optionWidget->strengthSlider->value();
    bool invert = (m_optionWidget->chkInvert->checkState() == Qt::Checked);
    TextureChannel activeChannel = (TextureChannel)m_optionWidget->cmbChannel->currentIndex();

    setting->setProperty("Texture/Pattern/Scale", scale);
    setting->setProperty("Texture/Pattern/OffsetX", offsetX);
    setting->setProperty("Texture/Pattern/OffsetY", offsetY);
    setting->setProperty("Texture/Pattern/Strength", strength);
    setting->setProperty("Texture/Pattern/Invert", invert);
    setting->setProperty("Texture/Pattern/Channel", int(activeChannel));

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    pattern->image().save(&buffer, "PNG");

    setting->setProperty("Texture/Pattern/Pattern", ba.toBase64());
    setting->setProperty("Texture/Pattern/PatternFileName", pattern->filename());
    setting->setProperty("Texture/Pattern/Name", pattern->name());

    setting->setProperty("Texture/Pattern/Enabled", isChecked());
}

void KisTextureOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
    QByteArray ba = QByteArray::fromBase64(setting->getString("Texture/Pattern/Pattern").toAscii());
    QImage img;
    img.loadFromData(ba, "PNG");
    QString name = setting->getString("Texture/Pattern/Name");
    if (name.isEmpty()) {
        name = setting->getString("Texture/Pattern/FileName");
    }

    KisPattern *pattern = 0;
    if (!img.isNull()) {
        pattern = new KisPattern(img, name);
    }
    // now check whether the pattern already occurs, if not, add it to the
    // resources.
    if (pattern) {
        bool found = false;
        foreach(KoResource *res, KisResourceServerProvider::instance()->patternServer()->resources()) {
            KisPattern *pat = static_cast<KisPattern *>(res);
            if (pat == pattern) {
                delete pattern;
                pattern = pat;
                found = true;
                break;
            }
        }
        if (!found) {
            KisResourceServerProvider::instance()->patternServer()->addResource(pattern, false);
        }
        m_optionWidget->offsetSliderX->setRange(0, pattern->image().width() / 2);
        m_optionWidget->offsetSliderY->setRange(0, pattern->image().height() / 2);
    }
    else {
        pattern = static_cast<KisPattern*>(m_optionWidget->chooser->currentResource());
    }
    m_optionWidget->chooser->setCurrentPattern(pattern);

    m_optionWidget->scaleSlider->setValue(setting->getDouble("Texture/Pattern/Scale", 1.0));
    m_optionWidget->offsetSliderX->setValue(setting->getInt("Texture/Pattern/OffsetX"));
    m_optionWidget->offsetSliderY->setValue(setting->getInt("Texture/Pattern/OffsetY"));
    m_optionWidget->strengthSlider->setValue(setting->getDouble("Texture/Pattern/Strength"));
    m_optionWidget->chkInvert->setChecked(setting->getBool("Texture/Pattern/Invert"));
    m_optionWidget->cmbChannel->setCurrentIndex(setting->getInt("Texture/Pattern/Channel"));

    setChecked(setting->getBool("Texture/Pattern/Enabled"));
}

void KisTextureOption::resetGUI(KoResource* res)
{
    KisPattern *pattern = static_cast<KisPattern *>(res);
    m_optionWidget->scaleSlider->setValue(1.0);
    m_optionWidget->offsetSliderX->setRange(0, pattern->image().width() / 2);
    m_optionWidget->offsetSliderX->setValue(0);
    m_optionWidget->offsetSliderY->setRange(0, pattern->image().height() / 2);
    m_optionWidget->offsetSliderY->setValue(0);
    m_optionWidget->strengthSlider->setValue(1.0);
    m_optionWidget->chkInvert->setChecked(false);
    m_optionWidget->cmbChannel->setCurrentIndex(0);
}

void KisTextureProperties::recalculateMask()
{
    if (!pattern) return;

    delete m_mask;
    m_mask = 0;

    QImage mask = pattern->image();

    if (!qFuzzyCompare(scale, 0.0)) {
        QTransform tf;
        tf.scale(scale, scale);
        QRect rc = tf.mapRect(mask.rect());
        mask = mask.scaled(rc.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QRgb* pixel = reinterpret_cast<QRgb*>( mask.bits() );
    int width = mask.width();
    int height = mask.height();

    const KoColorSpace *cs = KoColorSpaceRegistry::instance()->alpha8();
    m_mask = new KisPaintDevice(cs);

    KisHLineIteratorSP iter = m_mask->createHLineIteratorNG(0, 0, width);

    for (int row = 0; row < height; ++row ) {
        for (int col = 0; col < width; ++col ) {
            const QRgb currentPixel = pixel[row * width + col];

            const int red = qRed(currentPixel);
            const int green = qGreen(currentPixel);
            const int blue = qBlue(currentPixel);

            const int grayValue = (red * 11 + green * 16 + blue * 5) / 32;
            float maskValue = (grayValue / 255.0) * strength;
            if (invert) {
                maskValue = 1 - maskValue;
            }
            cs->setOpacity(iter->rawData(), maskValue, 1);
            iter->nextPixel();
        }
        iter->nextRow();
    }
    m_mask->convertToQImage(0).save("bla.png");
}


void KisTextureProperties::fillProperties(const KisPropertiesConfiguration *setting)
{
    QByteArray ba = QByteArray::fromBase64(setting->getString("Texture/Pattern/Pattern").toAscii());
    QImage img;
    img.loadFromData(ba, "PNG");
    QString name = setting->getString("Texture/Pattern/Name");
    if (name.isEmpty()) {
        name = setting->getString("Texture/Pattern/FileName");
    }

    pattern = 0;
    if (!img.isNull()) {
        pattern = new KisPattern(img, name);
    }
    if (pattern) {
        foreach(KoResource *res, KisResourceServerProvider::instance()->patternServer()->resources()) {
            KisPattern *pat = static_cast<KisPattern *>(res);
            if (pat == pattern) {
                delete pattern;
                pattern = pat;
                break;
            }
        }
    }

    enabled = setting->getBool("Texture/Pattern/Enabled", false);
    scale = setting->getDouble("Texture/Pattern/Scale", 1.0);
    offsetX = setting->getInt("Texture/Pattern/OffsetX");
    offsetY = setting->getInt("Texture/Pattern/OffsetY");
    strength = setting->getDouble("Texture/Pattern/Strength");
    invert = setting->getBool("Texture/Pattern/Invert");
    activeChannel = (KisTextureOption::TextureChannel)setting->getInt("Texture/Pattern/Channel");

    qDebug() << "fillproperties Enabled:" << enabled;

    recalculateMask();
}

void KisTextureProperties::apply(KisFixedPaintDeviceSP dab, const QPoint &offset)
{
    if (!enabled) return;

    KisPaintDeviceSP fillDevice = new KisPaintDevice(KoColorSpaceRegistry::instance()->alpha8());
    QRect bounds = m_mask->exactBounds();
    QRect rect = dab->bounds();

    int x = offset.x() % bounds.width() - offsetX;
    int y = offset.y() % bounds.height() - offsetY;

    fillDevice->setX(x);
    fillDevice->setY(y);

    KisFillPainter fillPainter(fillDevice);
    fillPainter.fillRect(x, y, rect.width(), rect.height(), m_mask, bounds);

    fillDevice->setX(0);
    fillDevice->setY(0);

    quint8 *alphaBytes = new quint8(rect.width() * rect.height());
    fillDevice->readBytes(alphaBytes, rect);

    dab->colorSpace()->applyAlphaU8Mask(dab->data(), alphaBytes, rect.width() * rect.height());

    dab->convertToQImage(0).save("result.png");
}

