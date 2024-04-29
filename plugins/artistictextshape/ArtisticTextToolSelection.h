/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ARTISTICTEXTTOOLSELECTION_H
#define ARTISTICTEXTTOOLSELECTION_H

#include <KoToolSelection.h>
#include <QPainterPath>

class ArtisticTextShape;
class KoCanvasBase;
class KoViewConverter;
class QPainter;

class ArtisticTextToolSelection : public KoToolSelection
{
public:
    explicit ArtisticTextToolSelection(KoCanvasBase *canvas, QObject *parent = nullptr);
    ~ArtisticTextToolSelection() override;

    // reimplemented from KoToolSelection
    bool hasSelection() override;

    /// Sets the currently selected text shape
    void setSelectedShape(ArtisticTextShape *textShape);

    /// Returns the currently selected text shape
    ArtisticTextShape *selectedShape() const;

    /// Selects specified range of characters
    void selectText(int from, int to);

    /// Returns the start character index of the selection
    int selectionStart() const;

    /// Returns number of selected characters
    int selectionCount() const;

    /// Clears the selection
    void clear();

    /// Paints the selection
    void paint(QPainter &painter, const KoViewConverter &converter);

    /// Triggers a repaint of the selection
    void repaintDecoration();

private:
    /// Returns the outline of the selection in document coordinates
    QPainterPath outline();

    KoCanvasBase *m_canvas;
    ArtisticTextShape *m_currentShape; ///< the currently selected text shape
    int m_selectionStart;
    int m_selectionCount;
};

#endif // ARTISTICTEXTTOOLSELECTION_H
