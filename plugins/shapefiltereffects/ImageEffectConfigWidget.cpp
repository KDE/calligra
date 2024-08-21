/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "ImageEffectConfigWidget.h"
#include "ImageEffect.h"

#include "KoFilterEffect.h"
#include <KoFileDialog.h>

#include <KLocalizedString>

#include <KConfigGroup>
#include <QGridLayout>
#include <QImageReader>
#include <QLabel>
#include <QPushButton>

ImageEffectConfigWidget::ImageEffectConfigWidget(QWidget *parent)
    : KoFilterEffectConfigWidgetBase(parent)
    , m_effect(nullptr)
{
    QGridLayout *g = new QGridLayout(this);

    m_image = new QLabel(this);
    QPushButton *button = new QPushButton(i18n("Select image..."), this);

    g->addWidget(m_image, 0, 0, Qt::AlignCenter);
    g->addWidget(button, 0, 1);

    setLayout(g);

    connect(button, &QAbstractButton::clicked, this, &ImageEffectConfigWidget::selectImage);
}

bool ImageEffectConfigWidget::editFilterEffect(KoFilterEffect *filterEffect)
{
    m_effect = dynamic_cast<ImageEffect *>(filterEffect);
    if (!m_effect)
        return false;

    m_image->setPixmap(QPixmap::fromImage(m_effect->image().scaledToWidth(80)));

    return true;
}

void ImageEffectConfigWidget::selectImage()
{
    if (!m_effect)
        return;

    KoFileDialog dialog(nullptr, KoFileDialog::OpenFile, "OpenDocument");
    dialog.setCaption(i18n("Select image"));
    dialog.setImageFilters();

    QString fname = dialog.filename();

    if (fname.isEmpty())
        return;

    QImage newImage;
    if (!newImage.load(fname))
        return;

    m_effect->setImage(newImage);
    editFilterEffect(m_effect);

    Q_EMIT filterChanged();
}
