/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2008-2009 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPathPointMoveCommand.h"
#include "KoPathPoint.h"
#include <KLocalizedString>

namespace
{
constexpr int PathPointMoveCommandId = 0x4b50504d;
}

class KoPathPointMoveCommandPrivate
{
public:
    KoPathPointMoveCommandPrivate()
        : undoCalled(true)
    {
    }
    void applyOffset(qreal factor);

    bool undoCalled; // this command stores diffs; so calling undo twice will give wrong results. Guard against that.
    QMap<KoPathPointData, QPointF> points;
    QSet<KoPathShape *> paths;
};

KoPathPointMoveCommand::KoPathPointMoveCommand(const QList<KoPathPointData> &pointData, const QPointF &offset, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new KoPathPointMoveCommandPrivate())
{
    setText(kundo2_i18n("Move points"));

    foreach (const KoPathPointData &data, pointData) {
        if (!d->points.contains(data)) {
            d->points[data] = offset;
            d->paths.insert(data.pathShape);
        }
    }
}

KoPathPointMoveCommand::KoPathPointMoveCommand(const QList<KoPathPointData> &pointData, const QVector<QPointF> &offsets, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new KoPathPointMoveCommandPrivate())
{
    Q_ASSERT(pointData.count() == offsets.count());

    setText(kundo2_i18n("Move points"));

    uint dataCount = pointData.count();
    for (uint i = 0; i < dataCount; ++i) {
        const KoPathPointData &data = pointData[i];
        if (!d->points.contains(data)) {
            d->points[data] = offsets[i];
            d->paths.insert(data.pathShape);
        }
    }
}

KoPathPointMoveCommand::~KoPathPointMoveCommand()
{
    delete d;
}

void KoPathPointMoveCommand::redo()
{
    KUndo2Command::redo();
    if (!d->undoCalled)
        return;

    d->applyOffset(1.0);
    d->undoCalled = false;
}

void KoPathPointMoveCommand::undo()
{
    KUndo2Command::undo();
    if (d->undoCalled)
        return;

    d->applyOffset(-1.0);
    d->undoCalled = true;
}

void KoPathPointMoveCommandPrivate::applyOffset(qreal factor)
{
    foreach (KoPathShape *path, paths) {
        // repaint old bounding rect
        path->update();
    }

    QMap<KoPathPointData, QPointF>::ConstIterator it(points.constBegin());
    for (; it != points.constEnd(); ++it) {
        KoPathShape *path = it.key().pathShape;
        // transform offset from document to shape coordinate system
        QPointF shapeOffset = path->documentToShape(factor * it.value()) - path->documentToShape(QPointF());
        QTransform matrix;
        matrix.translate(shapeOffset.x(), shapeOffset.y());

        KoPathPoint *p = path->pointByIndex(it.key().pointIndex);
        if (p)
            p->map(matrix);
    }

    foreach (KoPathShape *path, paths) {
        path->normalize();
        // repaint new bounding rect
        path->update();
    }
}

int KoPathPointMoveCommand::id() const
{
    return PathPointMoveCommandId;
}

bool KoPathPointMoveCommand::mergeWith(const KUndo2Command *command)
{
    const auto other = dynamic_cast<const KoPathPointMoveCommand *>(command);
    if (!other || other->d->paths != d->paths || other->d->points.keys() != d->points.keys())
        return false;

    for (auto it = d->points.begin(); it != d->points.end(); ++it)
        it.value() += other->d->points.value(it.key());
    return true;
}
