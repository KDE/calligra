/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef IMAGEEFFECTCONFIGWIDGET_H
#define IMAGEEFFECTCONFIGWIDGET_H

#include "KoFilterEffectConfigWidgetBase.h"

class KoFilterEffect;
class ImageEffect;
class QLabel;

class ImageEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    explicit ImageEffectConfigWidget(QWidget *parent = nullptr);

    /// reimplemented from KoFilterEffectConfigWidgetBase
    bool editFilterEffect(KoFilterEffect *filterEffect) override;

private Q_SLOTS:
    void selectImage();

private:
    ImageEffect *m_effect;
    QLabel *m_image;
};

#endif // IMAGEEFFECTCONFIGWIDGET_H
