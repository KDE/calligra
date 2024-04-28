/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHTOOLHANDLE_H
#define KOPATHTOOLHANDLE_H

#include "KoInteractionStrategy.h"
#include <KoPathPoint.h>

#include <QList>
#include <QRect>

class KoPathTool;
class KoParameterShape;
class KoViewConverter;
class KoPointerEvent;
class QPainter;
class KoPathShape;

class KoPathToolHandle
{
public:
    explicit KoPathToolHandle(KoPathTool *tool);
    virtual ~KoPathToolHandle();
    virtual void paint(QPainter &painter, const KoViewConverter &converter) = 0;
    virtual void repaint() const = 0;
    virtual KoInteractionStrategy *handleMousePress(KoPointerEvent *event) = 0;
    // test if handle is still valid
    virtual bool check(const QList<KoPathShape *> &selectedShapes) = 0;

protected:
    uint handleRadius() const;
    KoPathTool *m_tool;
};

class PointHandle : public KoPathToolHandle
{
public:
    PointHandle(KoPathTool *tool, KoPathPoint *activePoint, KoPathPoint::PointType activePointType);
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void repaint() const override;
    KoInteractionStrategy *handleMousePress(KoPointerEvent *event) override;
    bool check(const QList<KoPathShape *> &selectedShapes) override;
    KoPathPoint *activePoint() const;
    KoPathPoint::PointType activePointType() const;

private:
    KoPathPoint *m_activePoint;
    KoPathPoint::PointType m_activePointType;
    mutable QRectF m_oldRepaintedRect;
};

class ParameterHandle : public KoPathToolHandle
{
public:
    ParameterHandle(KoPathTool *tool, KoParameterShape *parameterShape, int handleId);
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void repaint() const override;
    KoInteractionStrategy *handleMousePress(KoPointerEvent *event) override;
    bool check(const QList<KoPathShape *> &selectedShapes) override;

protected:
    KoParameterShape *m_parameterShape;
    int m_handleId;
};

class ConnectionHandle : public ParameterHandle
{
public:
    ConnectionHandle(KoPathTool *tool, KoParameterShape *parameterShape, int handleId);

    // XXX: Later: create a paint even to distinguish a connection
    // handle  from another handle type
    KoInteractionStrategy *handleMousePress(KoPointerEvent *event) override;
};

#endif // KOPATHTOOLHANDLE_H
