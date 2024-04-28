/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CORNERSINWIPESTRATEGY_H
#define CORNERSINWIPESTRATEGY_H

#include <KPrPageEffectStrategy.h>

class CornersInWipeStrategy : public KPrPageEffectStrategy
{
public:
    explicit CornersInWipeStrategy(bool reverse);
    ~CornersInWipeStrategy() override;

    /// reimplemented from KPrPageEffectStrategy
    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;
    /// reimplemented from KPrPageEffectStrategy
    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;
    /// reimplemented from KPrPageEffectStrategy
    void next(const KPrPageEffect::Data &data) override;

private:
    /// calculates clip path for given step
    QPainterPath clipPath(int step, const QRect &area);
};

#endif // CORNERSINWIPESTRATEGY_H
