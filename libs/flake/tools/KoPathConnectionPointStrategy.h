/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@kde.org>
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KO_PATH_CONNECTION_POINT_STRATEGY
#define KO_PATH_CONNECTION_POINT_STRATEGY

#include "KoParameterChangeStrategy.h"
#include "flake_export.h"

class KoConnectionShape;
class QPointF;
class KoPathConnectionPointStrategyPrivate;

/**
 * @brief Strategy for moving end points of a connection shape.
 */
class FLAKE_EXPORT KoPathConnectionPointStrategy : public KoParameterChangeStrategy
{
public:
    KoPathConnectionPointStrategy(KoToolBase *tool, KoConnectionShape *parameterShape, int handleId);
    ~KoPathConnectionPointStrategy() override;
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;
    KUndo2Command *createCommand() override;

private:
    Q_DECLARE_PRIVATE(KoPathConnectionPointStrategy)
};

#endif
