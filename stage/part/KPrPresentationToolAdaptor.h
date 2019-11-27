/* This file is part of the KDE project
 * Copyright (C) 2010 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
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

#ifndef KPRPRESENTATIONTOOLADAPTOR_H
#define KPRPRESENTATIONTOOLADAPTOR_H

#ifndef QT_NO_DBUS
#include "KPrPresentationTool.h"

#include <QDBusAbstractAdaptor>

class KPrViewModePresentation;
class KPrPresentationTool;

class KPrPresentationToolAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.calligra.presentation.tool")

public:
    explicit KPrPresentationToolAdaptor(KPrPresentationTool *tool);
    ~KPrPresentationToolAdaptor() override;

public Q_SLOTS: // METHODS

    /**
     * Blank the presentation.
     */
    void blankPresentation();

    /**
     * Highlight the presentation.
     */
    Q_NOREPLY void highlightPresentation(int pointx, int pointy);

    /**
     * Draw on presentation.
     * @p color can take the color values as a string. Eg: red, green, black.
     */
    void startDrawPresentation(int pointx, int pointy, int penSize, const QString &color);

    /**
     * Start drawing on presentation tool.
     * The parameters @p pointx and @p pointy specify a point in the path.
     */
    Q_NOREPLY void drawOnPresentation(int pointx, int pointy);

    /**
     * Stop drawing path. The current path is stopped.
     */
    void stopDrawPresentation();

    /**
     * Normal presentation mode.
     */
    void normalPresentation();

private:
    KPrPresentationTool *m_tool;
    KPrViewModePresentation &m_viewModePresentation;
};
#endif // QT_NO_DBUS

#endif /* KPRPRESENTATIONTOOLADAPTOR_H */
