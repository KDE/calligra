/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRPRESENTATIONTOOLADAPTOR_H
#define KPRPRESENTATIONTOOLADAPTOR_H

#ifdef WITH_QTDBUS
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
#endif // WITH_QTDBUS

#endif /* KPRPRESENTATIONTOOLADAPTOR_H */
