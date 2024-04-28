/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Johannes Schaub <litb_devel@web.de>
   SPDX-FileCopyrightText: 2011 Arjen Hiemstra <ahiemstra@heimr.nl>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoZoomMode.h"
#include <KLocalizedString>

const std::vector<std::optional<KLazyLocalizedString>> KoZoomMode::modes = {kli18n("%1%"),
                                                                            kli18n("Fit Page Width"),
                                                                            kli18n("Fit Page"),
                                                                            std::nullopt,
                                                                            kli18n("Actual Pixels"),
                                                                            std::nullopt,
                                                                            std::nullopt,
                                                                            std::nullopt,
                                                                            kli18n("Fit Text Width")};

qreal KoZoomMode::minimumZoomValue = 0.2;
qreal KoZoomMode::maximumZoomValue = 5.0;

QString KoZoomMode::toString(Mode mode)
{
    auto m = modes[mode];
    return m ? m->toString() : QString{};
}

KoZoomMode::Mode KoZoomMode::toMode(const QString &mode)
{
    if (mode == modes[ZOOM_WIDTH]->toString())
        return ZOOM_WIDTH;
    else if (mode == modes[ZOOM_PAGE]->toString())
        return ZOOM_PAGE;
    else if (mode == modes[ZOOM_PIXELS]->toString())
        return ZOOM_PIXELS;
    else if (mode == modes[ZOOM_TEXT]->toString())
        return ZOOM_TEXT;
    else
        return ZOOM_CONSTANT;
    // we return ZOOM_CONSTANT else because then we can pass '10%' or '15%'
    // or whatever, it's automatically converted. ZOOM_CONSTANT is
    // changeable, whereas all other zoom modes (non-constants) are normal
    // text like "Fit to xxx". they let the view grow/shrink according
    // to windowsize, hence the term 'non-constant'
}

qreal KoZoomMode::minimumZoom()
{
    return minimumZoomValue;
}

qreal KoZoomMode::maximumZoom()
{
    return maximumZoomValue;
}

qreal KoZoomMode::clampZoom(qreal zoom)
{
    return qMin(maximumZoomValue, qMax(minimumZoomValue, zoom));
}

void KoZoomMode::setMinimumZoom(qreal zoom)
{
    Q_ASSERT(zoom > 0.0f);
    minimumZoomValue = zoom;
}

void KoZoomMode::setMaximumZoom(qreal zoom)
{
    Q_ASSERT(zoom > 0.0f);
    maximumZoomValue = zoom;
}
