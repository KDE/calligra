/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRFADEOVERCOLORSTRATEGY_H
#define KPRFADEOVERCOLORSTRATEGY_H

#include "pageeffects/KPrPageEffectStrategy.h"

#include <QColor>

class KPrFadeOverColorStrategy : public KPrPageEffectStrategy
{
public:
    KPrFadeOverColorStrategy();
    ~KPrFadeOverColorStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;

    void finish(const KPrPageEffect::Data &data) override;

    // reimplemented
    void saveOdfSmilAttributes(KoXmlWriter &xmlWriter) const override;

    // reimplemented
    void saveOdfSmilAttributes(KoGenStyle &style) const override;

    // reimplemented
    void loadOdfSmilAttributes(const KoXmlElement &element) override;

private:
    QColor m_fadeColor;
};

#endif /* KPRFADEOVERCOLORSTRATEGY_H */
