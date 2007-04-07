/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#ifndef KWANCHORSTRATEGY_H
#define KWANCHORSTRATEGY_H

#include <KoTextDocumentLayout.h>

#include <QList>

class KoTextAnchor;
class KWAnchorStrategy;
class KWFrame;

class KWAnchorStrategy {
public:
    KWAnchorStrategy(KoTextAnchor *anchor);
    virtual ~KWAnchorStrategy();

    bool checkState(KoTextDocumentLayout::LayoutState *state);
    bool shouldRemove();
    KoShape * anchoredShape() const;

    KoTextAnchor *anchor() { return m_anchor; }

private:
    KoTextAnchor *const m_anchor;
    int m_knowledgePoint; // the cursor position at which the layout process has gathered enough info to do our work
    bool m_finished;
    double m_currentLineY;
    int m_pass;
};

#endif
