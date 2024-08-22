/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoColorDisplayRendererInterface.h"

#include <QGlobalStatic>

#include <KoChannelInfo.h>
#include <KoColorSpaceRegistry.h>

Q_GLOBAL_STATIC(KoDumbColorDisplayRenderer, s_instance)

KoColorDisplayRendererInterface::KoColorDisplayRendererInterface() = default;

KoColorDisplayRendererInterface::~KoColorDisplayRendererInterface() = default;

QColor KoDumbColorDisplayRenderer::toQColor(const KoColor &c) const
{
    return c.toQColor();
}

KoColor KoDumbColorDisplayRenderer::approximateFromRenderedQColor(const QColor &c) const
{
    KoColor color;
    color.fromQColor(c);
    return color;
}

KoColor KoDumbColorDisplayRenderer::fromHsv(int h, int s, int v, int a) const
{
    h = qBound(0, h, 359);
    s = qBound(0, s, 255);
    v = qBound(0, v, 255);
    a = qBound(0, a, 255);
    QColor qcolor(QColor::fromHsv(h, s, v, a));
    return KoColor(qcolor, KoColorSpaceRegistry::instance()->rgb8());
}

void KoDumbColorDisplayRenderer::getHsv(const KoColor &srcColor, int *h, int *s, int *v, int *a) const
{
    QColor qcolor = toQColor(srcColor);
    qcolor.getHsv(h, s, v, a);
}

KoColorDisplayRendererInterface *KoDumbColorDisplayRenderer::instance()
{
    return s_instance;
}

qreal KoDumbColorDisplayRenderer::minVisibleFloatValue(const KoChannelInfo *chaninfo) const
{
    Q_ASSERT(chaninfo);
    return chaninfo->getUIMin();
}

qreal KoDumbColorDisplayRenderer::maxVisibleFloatValue(const KoChannelInfo *chaninfo) const
{
    Q_ASSERT(chaninfo);
    return chaninfo->getUIMax();
}
