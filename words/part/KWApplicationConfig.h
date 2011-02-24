/* This file is part of the KDE project
 * Copyright (C) 2007-2010 Thomas Zander <zander@kde.org>
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
#ifndef KWAPPLICATIONCONFIG_H
#define KWAPPLICATIONCONFIG_H

#include <KoZoomMode.h>

class KConfigGroup;
class KWDocument;
class KoUnit;

/**
 * A set of config settings for application-specific stuff (compared to document-specific stuff).
 */
class KWApplicationConfig
{
public:
    KWApplicationConfig();

    /// load the kword settings.
    void load(KWDocument *document);
    /// save the config settings.
    void save();

    void setViewFrameBorders(bool on) {
        m_viewFrameBorders = on;
    }
    bool viewFrameBorders() const {
        return m_viewFrameBorders;
    }

    void setViewRulers(bool on) {
        m_viewRulers = on;
    }
    bool viewRulers() const {
        return m_viewRulers;
    }

    void setStatusBarShowPage(bool on) {
        m_statusBarShowPage = on;
    }

    bool statusBarShowPage() {
        return m_statusBarShowPage;
    }

    void setStatusBarShowModified(bool on) {
        m_statusBarShowModified = on;
    }

    bool statusBarShowModified() {
        return m_statusBarShowModified;
    }

    void setStatusBarShowMouse(bool on) {
        m_statusBarShowMouse = on;
    }

    bool statusBarShowMouse() {
        return m_statusBarShowMouse;
    }

    void setStatusBarShowZoom(bool on) {
        m_statusBarShowZoom = on;
    }

    bool statusBarShowZoom() {
        return m_statusBarShowZoom;
    }

    /**
     * Set the zoom level for new views and for next startups of kword.
     * Documents shown at 100% will show the page on screen at the same amount of
     * metric units as the user set them to be. In other words; the paper and the
     * screen versions should be exactly the same.
     * @param percent the new zoom level to be persisted between sessions.
     */
    void setZoom(int percent) {
        m_zoom = percent;
    }

    /**
     * Return the percentage of zoom.
     * @return the percentage of zoom.
     * @see setZoom(int)
     */
    int zoom() const {
        return m_zoom;
    }

    /**
     * Set the mode of zooming for new views and to be persisted between sessions.
     * @param mode the new mode
     */
    void setZoomMode(KoZoomMode::Mode mode) {
        m_zoomMode = mode;
    }

    /**
     * Return the zoomMode to be used for new views.
     */
    KoZoomMode::Mode zoomMode() const {
        return m_zoomMode;
    }

    qreal defaultColumnSpacing() const {
        return m_defaultColumnSpacing;
    }

    void setUnit(const KoUnit &unit);

private:
    bool m_viewFrameBorders, m_viewRulers, m_viewFormattingChars, m_viewFormattingBreak, m_viewFormattingSpace;
    bool m_viewFormattingEndParag, m_viewFormattingTabs;
    bool m_createBackupFile;
    bool m_statusBarShowPage, m_statusBarShowModified;
    bool m_statusBarShowMouse, m_statusBarShowZoom;

    int m_zoom; /// < zoom level in percent
    KoZoomMode::Mode m_zoomMode;

    int m_autoSaveSeconds;
    qreal m_defaultColumnSpacing;

    Q_DISABLE_COPY(KWApplicationConfig)
};

#endif
