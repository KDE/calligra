/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeContainerDefaultModel.h"

#include "KoShapeContainer.h"

class Q_DECL_HIDDEN KoShapeContainerDefaultModel::Private
{
public:
    class Relation
    {
    public:
        explicit Relation(KoShape *child)
            : inside(false)
            , inheritsTransform(false)
            , m_child(child)
        {
        }

        KoShape *child()
        {
            return m_child;
        }

        uint inside : 1; ///< if true, the child will be clipped by the parent.
        uint inheritsTransform : 1;

    private:
        KoShape *m_child;
    };

    ~Private()
    {
        qDeleteAll(relations);
    }

    Relation *findRelation(const KoShape *child) const
    {
        foreach (Relation *relation, relations) {
            if (relation->child() == child) {
                return relation;
            }
        }
        return nullptr;
    }

    // TODO use a QMap<KoShape*, bool> instead this should speed things up a bit
    QList<Relation *> relations;
};

KoShapeContainerDefaultModel::KoShapeContainerDefaultModel()
    : d(new Private())
{
}

KoShapeContainerDefaultModel::~KoShapeContainerDefaultModel()
{
    delete d;
}

void KoShapeContainerDefaultModel::add(KoShape *child)
{
    Private::Relation *r = new Private::Relation(child);
    d->relations.append(r);
}

void KoShapeContainerDefaultModel::proposeMove(KoShape *shape, QPointF &move)
{
    KoShapeContainer *parent = shape->parent();
    bool allowedToMove = true;
    while (allowedToMove && parent) {
        allowedToMove = parent->isEditable();
        parent = parent->parent();
    }
    if (!allowedToMove) {
        move.setX(0);
        move.setY(0);
    }
}

void KoShapeContainerDefaultModel::setClipped(const KoShape *child, bool clipping)
{
    Private::Relation *relation = d->findRelation(child);
    if (relation == nullptr)
        return;
    if (relation->inside == clipping)
        return;
    relation->child()->update(); // mark old canvas-location as in need of repaint (aggregated)
    relation->inside = clipping;
    relation->child()->notifyChanged();
    relation->child()->update(); // mark new area as in need of repaint
}

bool KoShapeContainerDefaultModel::isClipped(const KoShape *child) const
{
    Private::Relation *relation = d->findRelation(child);
    return relation ? relation->inside : false;
}

void KoShapeContainerDefaultModel::remove(KoShape *child)
{
    Private::Relation *relation = d->findRelation(child);
    if (relation == nullptr)
        return;
    d->relations.removeAll(relation);
    delete relation;
}

int KoShapeContainerDefaultModel::count() const
{
    return d->relations.count();
}

QList<KoShape *> KoShapeContainerDefaultModel::shapes() const
{
    QList<KoShape *> answer;
    foreach (Private::Relation *relation, d->relations) {
        answer.append(relation->child());
    }
    return answer;
}

bool KoShapeContainerDefaultModel::isChildLocked(const KoShape *child) const
{
    return child->isGeometryProtected();
}

void KoShapeContainerDefaultModel::containerChanged(KoShapeContainer *, KoShape::ChangeType)
{
}

void KoShapeContainerDefaultModel::setInheritsTransform(const KoShape *shape, bool inherit)
{
    Private::Relation *relation = d->findRelation(shape);
    if (relation == nullptr)
        return;
    if (relation->inheritsTransform == inherit)
        return;
    relation->child()->update(); // mark old canvas-location as in need of repaint (aggregated)
    relation->inheritsTransform = inherit;
    relation->child()->notifyChanged();
    relation->child()->update(); // mark new area as in need of repaint
}

bool KoShapeContainerDefaultModel::inheritsTransform(const KoShape *shape) const
{
    Private::Relation *relation = d->findRelation(shape);
    return relation ? relation->inheritsTransform : false;
}
