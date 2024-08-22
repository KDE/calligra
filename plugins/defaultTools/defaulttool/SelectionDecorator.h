/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SELECTIONDECORATOR_H
#define SELECTIONDECORATOR_H

#include <KoFlake.h>
#include <KoViewConverter.h>
#include <QPainter>

class KoSelection;

/**
 * The SelectionDecorator is used to paint extra user-interface items on top of a selection.
 */
class SelectionDecorator
{
public:
    /**
     * Constructor.
     * @param arrows the direction that needs highlighting. (currently unused)
     * @param rotationHandles if true; the rotation handles will be drawn
     * @param shearHandles if true; the shearhandles will be drawn
     */
    SelectionDecorator(KoFlake::SelectionHandle arrows, bool rotationHandles, bool shearHandles);
    ~SelectionDecorator() = default;

    /**
     * paint the decorations.
     * @param painter the painter to paint to.
     * @param converter to convert between internal and view coordinates.
     */
    void paint(QPainter &painter, const KoViewConverter &converter);

    /**
     * set the selection that is to be painted.
     * @param selection the current selection.
     */
    void setSelection(KoSelection *selection);

    /**
     * set the radius of the selection handles
     * @param radius the new handle radius
     */
    void setHandleRadius(int radius);

    /// Sets the hot position to highlight
    static void setHotPosition(KoFlake::Position hotPosition);

    /// Returns the hot position
    static KoFlake::Position hotPosition();

private:
#if 0
    bool m_rotationHandles, m_shearHandles;
    KoFlake::SelectionHandle m_arrows;
#endif
    static KoFlake::Position m_hotPosition;
    KoSelection *m_selection;
    int m_handleRadius;
    int m_lineWidth;
};

#endif
