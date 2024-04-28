/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Johannes Schaub <litb_devel@web.de>
   SPDX-FileCopyrightText: 2011 Arjen Hiemstra <ahiemstra@heimr.nl>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _KOZOOMMODE_H_
#define _KOZOOMMODE_H_

#include "kowidgets_export.h"
#include <KLazyLocalizedString>
#include <QFlags>
#include <QString>

/**
 * The ZoomMode container
 */
class KOWIDGETS_EXPORT KoZoomMode
{
public:
    enum Mode {
        ZOOM_CONSTANT = 0, ///< zoom x %
        ZOOM_WIDTH = 1, ///< zoom pagewidth
        ZOOM_PAGE = 2, ///< zoom to pagesize
        ZOOM_PIXELS = 4, ///< zoom to actual pixels
        ZOOM_TEXT = 8 ///< zoom to actual pixels
    };

    Q_DECLARE_FLAGS(Modes, Mode)

    /// \param mode the mode name
    /// \return the to Mode converted QString \c mode
    static Mode toMode(const QString &mode);

    /// \return the to QString converted and translated Mode \c mode
    static QString toString(Mode mode);

    /// \param mode the mode name
    /// \return true if \c mode isn't dependent on windowsize
    static bool isConstant(const QString &mode)
    {
        return toMode(mode) == ZOOM_CONSTANT;
    }

    /**
     * Return the minimum zoom possible for documents.
     *
     * \return The minimum zoom possible.
     */
    static qreal minimumZoom();
    /**
     * Return the maximum zoom possible for documents.
     *
     * \return The maximum zoom possible.
     */
    static qreal maximumZoom();
    /**
     * Clamp the zoom value so that mimimumZoom <= zoom <= maximumZoom.
     *
     * \param zoom The value to clamp.
     *
     * \return minimumZoom if zoom < minimumZoom, maximumZoom if zoom >
     * maximumZoom, zoom if otherwise.
     */
    static qreal clampZoom(qreal zoom);

    /**
     * Set the minimum zoom possible for documents.
     *
     * Note that after calling this, any existing KoZoomAction instances
     * should be recreated.
     *
     * \param zoom The minimum zoom to use.
     */
    static void setMinimumZoom(qreal zoom);
    /**
     * Set the maximum zoom possible for documents.
     *
     * Note that after calling this, any existing KoZoomAction instances
     * should be recreated.
     *
     * \param zoom The maximum zoom to use.
     */
    static void setMaximumZoom(qreal zoom);

private:
    static std::vector<std::optional<KLazyLocalizedString>> const modes;
    static qreal minimumZoomValue;
    static qreal maximumZoomValue;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoZoomMode::Modes)

#endif
