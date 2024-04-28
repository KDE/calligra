/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef MOVESTARTOFFSETSTRATEGY_H
#define MOVESTARTOFFSETSTRATEGY_H

#include <KoInteractionStrategy.h>
#include <QList>

class KoPathShape;
class ArtisticTextShape;
class KoToolBase;

/// A strategy to change the offset of a text when put on a path
class MoveStartOffsetStrategy : public KoInteractionStrategy
{
public:
    MoveStartOffsetStrategy(KoToolBase *tool, ArtisticTextShape *text);
    ~MoveStartOffsetStrategy() override;

    // reimplemnted from KoInteractionStrategy
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    // reimplemnted from KoInteractionStrategy
    KUndo2Command *createCommand() override;
    // reimplemnted from KoInteractionStrategy
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;

private:
    ArtisticTextShape *m_text; ///< the text shape we are working on
    KoPathShape *m_baselineShape; ///< path shape the text is put on
    qreal m_oldStartOffset; ///< the initial start offset
    QList<qreal> m_segmentLengths; ///< cached lengths of baseline path segments
    qreal m_totalLength; ///< total length of baseline path
};

#endif // MOVESTARTOFFSETSTRATEGY_H
