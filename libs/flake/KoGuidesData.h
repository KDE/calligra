/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOGUIDESDATA_H
#define KOGUIDESDATA_H

#include "flake_export.h"
#include <QList>
#include <Qt>
#include "KoXmlReaderForward.h"

class QPainter;
class KoViewConverter;
class QRectF;
class QColor;
class KoXmlWriter;

/**
 * XXX
 */
class FLAKE_EXPORT KoGuidesData
{
public:
    KoGuidesData();
    ~KoGuidesData();

    /**
     * @brief Set the positions of the horizontal guide lines
     *
     * @param lines a list of positions of the horizontal guide lines
     */
    void setHorizontalGuideLines(const QList<qreal> &lines);

    /**
     * @brief Set the positions of the vertical guide lines
     *
     * @param lines a list of positions of the vertical guide lines
     */
    void setVerticalGuideLines(const QList<qreal> &lines);

    /**
     * @brief Add a guide line to the canvas.
     *
     * @param orientation the orientation of the guide line
     * @param position the position in document coordinates of the guide line
     */
    void addGuideLine(Qt::Orientation orientation, qreal position);

    /**
     * @brief Display or not guide lines
     */
    bool showGuideLines() const;

    /**
     * @param show display or not guide line
     */
    void setShowGuideLines(bool show);

    /// Returns the list of horizontal guide lines.
    QList<qreal> horizontalGuideLines() const;

    /// Returns the list of vertical guide lines.
    QList<qreal> verticalGuideLines() const;

    /**
     * Paints the guides using the given painter and viewconverter.
     * Only guides intersecting the given area are painted.
     * @param painter the painter
     * @param converter the view converter
     * @param area the area in need of updating
     */
    void paintGuides(QPainter &painter, const KoViewConverter &converter, const QRectF &area) const;

    /**
     * Sets the color of the guide lines.
     * @param color the new guides color
     */
    void setGuidesColor(const QColor &color);

    /// Returns the color of the guide lines.
    QColor guidesColor() const;

    /// Loads guide lines from the given setting xml document
    bool loadOdfSettings(const KoXmlDocument &settingsDoc);

    /// Saves guide lines to the given settings xml writer
    void saveOdfSettings(KoXmlWriter &settingsWriter);

private:
    class Private;
    Private * const d;
};


#endif

