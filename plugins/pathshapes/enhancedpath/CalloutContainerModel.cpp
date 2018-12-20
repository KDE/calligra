/* This file is part of the KDE project
 * Copyright (C) 2018 Dag Andersen <danders@get2net.dk>
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

#include "CalloutContainerModel.h"

#include "CalloutShape.h"
#include "EnhancedPathShape.h"
#include "CalloutDebug.h"

#include <KoTextShapeDataBase.h>
#include <KoShape.h>
#include <KoPathPoint.h>

#include <QSizeF>
#include <QRectF>
#include <QTransform>
#include <QtMath>
#include <QDebug>

void decompose(const QTransform &m, qreal &scaleX, qreal &scaleY, qreal &rotation, qreal &skewX, qreal &skewY, qreal &transX, qreal &transY)
{
    scaleX=0; scaleY=1; rotation=0; skewX=0; skewY=0; transX=0; transY=0;
    qreal a = m.m11();
    qreal b = m.m12();
    qreal c = m.m21();
    qreal d = m.m22();
    qreal e = m.m31();
    qreal f = m.m32();
    
    qreal Delta = a * d - b * c;
    
    if (a != 0 || b != 0) {
        qreal r = qSqrt(a*a+b*b);
        rotation = b > 0 ? qAcos(a/r) : -qAcos(a/r);
        scaleX = r;
        scaleY = Delta/r;
        skewX = qAtan((a*c+b*d)/(r*r));
    } else if (c != 0 || d != 0) {
        qreal s = qSqrt(c*c+d*d);
        rotation = M_PI_2 - (d > 0 ? qAcos(-c/s) : -qAcos(c/s));
        scaleX = Delta/s;
        scaleY = s;
        skewY = qAtan((a*c+b*d)/(s*s));
    } else { // a = b = c = d = 0
        scaleX = 0.0;
        scaleY = 0.0;
    }
    debugCallout<<"decomposed:"<<m<<endl<<'\t'<<scaleX<<scaleY<<qRadiansToDegrees(rotation)<<qRadiansToDegrees(skewX)<<qRadiansToDegrees(skewY)<<transX<<transY;
}

QTransform normalize(const QTransform &m)
{
    qreal scaleX, scaleY, rotation, skewX, skewY, transX, transY;
    decompose(m, scaleX, scaleY, rotation, skewX, skewY, transX, transY);
    QTransform n;
    //n.scale(-scaleX, -scaleY); //TODO
    n.rotateRadians(-rotation);
    n.shear(-skewX, -skewY);
    n = m * n;
    debugCallout<<"normalized:"<<n;
    return n;
}

QPointF position(const KoShapeContainer *container)
{
    QTransform m = normalize(container->transformation());
    QPointF center(0.5*container->size().width(), 0.5*container->size().height());
    return m.map(center) - center;
}

CalloutContainerModel::CalloutContainerModel()
    : KoShapeContainerDefaultModel()
    , m_resizing(false)
{
}

void CalloutContainerModel::setIgnore(KoShape *shape, bool state)
{
    m_ignore.insert(shape, state);
}

bool CalloutContainerModel::ignore(KoShape *shape) const
{
    return m_ignore.contains(shape) && m_ignore[shape];
}

void CalloutContainerModel::containerChanged(KoShapeContainer *container, KoShape::ChangeType type)
{
    switch (type) {
        case KoShape::PositionChanged: {
            QPointF center(0.5*container->size().width(), 0.5*container->size().height());
            QPointF pos = container->position();
            m_prevPosition = position(container);
            debugCallout<<type<<"org:"<<pos<<"norm:"<<m_prevPosition;
            break;
        }
        case KoShape::SizeChanged:
            if (!m_resizing) {
                CalloutShape *callout = dynamic_cast<CalloutShape*>(container);
                Q_ASSERT(callout);
                QPointF newPos = position(callout);
                resizePath(callout->pathShape(), newPos, callout->size());
                m_prevPosition = newPos;
                m_prevSize = container->size();
            }
            break;
        case KoShape::BeginResize: {
            m_resizing = true;
            m_prevPosition = position(container);
            m_prevSize = container->size();
            break;
        }
        case KoShape::EndResize: {
            debugCalloutF<<type<<">>>";
            CalloutShape *callout = dynamic_cast<CalloutShape*>(container);
            QPointF newPos = position(callout);
            resizePath(callout->pathShape(), newPos, callout->size());
            m_prevPosition = newPos;
            m_prevSize = container->size();
            m_resizing = false;
            debugCalloutF<<type<<"<<<";
            break;
        }
        case KoShape::GenericMatrixChange:
        case KoShape::RotationChanged: {
            //debugCalloutF<<type<<container->position()<<container->size()<<'('<<m_prevPosition<<m_prevSize<<')'<<endl<<'\t'<<container->absoluteTransformation(0);
            //debugCalloutF<<type<<container->absolutePosition();
            break;
        }
        default:
            //debugCalloutF<<type;
            break;
    }
}

void CalloutContainerModel::childChanged(KoShape *shape, KoShape::ChangeType type)
{
//     debugCalloutF<<type;
//     if (type == KoShape::Deleted) {
//         return;
//     }
//     if (type != KoShape::ParameterChanged) {
// //         return;
//     }
//     PathShape *path = dynamic_cast<PathShape*>(shape);
//     debugCalloutF<<type<<path->outlineRect();
//     if (!path) {
//         return;
//     }
    //KoProperties params = path->parameters();
    //debugCalloutF<<params.property("modifiers");
    
}

// void decompose(const QTransform &m, qreal &scaleX, qreal &scaleY, qreal &rotation, qreal &skewX, qreal &skewY. qreal &transX, qreal &transY)
// {
//     E=(m00+m11)/2;
//     F=(m00-m11)/2;
//     G=(m10+m01)/2;
//     H=(m10-m01)/2;
//     Q=sqrt(E^2+H^2);
//     R=sqrt(F^2+G^2);
//     sx=Q+R;
//     sy=Q-R;
//     a1=atan2(G,F);
//     a2=atan2(H,E);
//     theta=(a2-a1)/2;
//     phi=(a2+a1)/2;
// }


bool CalloutContainerModel::isChildLocked(const KoShape *child) const
{
    Q_UNUSED(child)
    return false;
}

// TODO: Shearing Y does not work
void CalloutContainerModel::resizePath(PathShape *path, const QPointF &newPos, const QSizeF &newSize)
{
    debugCalloutF<<">>>>>";

    if (newSize == m_prevSize) {
        debugCalloutF<<"No change"<<"<<<<<";
        return;
    }
    KoProperties params = path->parameters();
    QSizeF prevPathSize = path->size();
    QSizeF newPathSize;
    debugCallout<<'\t'<<"prev:"<<m_prevPosition<<m_prevSize<<"new:"<<newPos<<newSize<<"diff:"<<(newSize-m_prevSize);
    QList<qreal> modifiers = path->modifiers();
    Q_ASSERT(modifiers.count() >= 2);
    
    QVariant viewboxData;
    params.property("viewBox", viewboxData);
    QRect viewBox = path->viewBox();
    
    qreal currViewboxModifierRatioX = modifiers[0] / viewBox.width();
    qreal currViewboxModifierRatioY = modifiers[1] / viewBox.height();
    qreal viewboxModifierRatioX = 1.0;
    qreal viewboxModifierRatioY = 1.0;
    if (m_prevSize.isValid()) {
        bool pointerLeft = modifiers[0] < viewBox.left();
        bool pointerRight = modifiers[0] > viewBox.right();
        bool pointerTop = modifiers[1] < viewBox.top();
        bool pointerBottom = modifiers[1] > viewBox.bottom();
        if (pointerTop) debugCallout<<'\t'<<"pointer above";
        if (pointerRight) debugCallout<<'\t'<<"pointer right";
        if (pointerLeft) debugCallout<<'\t'<<"pointer left";
        if (pointerBottom) debugCallout<<'\t'<<"pointer below";

        bool movedX = qAbs(m_prevPosition.x() - newPos.x()) > 0.001;
        bool movedY = qAbs(m_prevPosition.y() - newPos.y()) > 0.001;
        if (movedX) debugCallout<<'\t'<<"x moved"<<"prev:"<<m_prevPosition.x()<<"new:"<<newPos.x();
        if (movedY) debugCallout<<'\t'<<"y moved"<<"prev:"<<m_prevPosition.y()<<"new:"<<newPos.y();
        
        if ((pointerLeft && !movedX) || (pointerRight && movedX)) {
            // Width has been changed in a way that affects path size
            newPathSize.setWidth(prevPathSize.width() + (newSize.width() - m_prevSize.width()));
            debugCallout<<'\t'<<"Width changed:"<<"new width:"<<newPathSize.width();
        } else {
            newPathSize.setWidth(prevPathSize.width());
            debugCallout<<'\t'<<"Width not changed:"<<"Keep path width";
        }
        
        if ((pointerTop && !movedY) || (pointerBottom && movedY)) {
            // Height has been changed in a way that affects path size
            newPathSize.setHeight(prevPathSize.height() + (newSize.height() - m_prevSize.height()));
            debugCallout<<'\t'<<"Height changed:"<<"new height:"<<newPathSize.height();
        } else {
            newPathSize.setHeight(prevPathSize.height());
            debugCallout<<'\t'<<"Height not changed:"<<"Keep path height";
        }
        //debugCallout<<'\t'<<"Path size:"<<"prev:"<<prevPathSize<<"new:"<<newPathSize<<"diff:"<<(newPathSize-prevPathSize);

        // Calculate new modifiers (the callout pointer tip)
        // The pointer tip shall stay in the same *global* position
        // so since it is given in viewbox coordinates (which corresponds to the bubble part),
        // it needs to recalculated when the bubble part is moved/resized.
        // Note: the viewbox coordinates never changes.
        //debugCallout<<'\t'<<"Current modifiers:"<<modifiers;
        if (pointerLeft || pointerRight) {
            debugCallout<<'\t'<<"modifier X is ouside viewbox";
            qreal ax = newSize.width();
            qreal bx = newPathSize.width();
            qreal vbx = viewBox.width();
            qreal rx = (bx - ax) / ax;
            qreal mx = pointerLeft ? -vbx * rx : vbx + (vbx * rx);
            modifiers[0] = mx;
        } else if (movedX) {
            debugCallout<<'\t'<<"left side resize and modifier X is inside viewbox";
            qreal ax = newSize.width();
            qreal bx = m_prevSize.width();
            qreal vbx = viewBox.width();
            qreal rvbx = ax / bx;
            qreal vbxDiff = (vbx * rvbx) - vbx;
            qreal x = modifiers[0] + vbxDiff;
            qreal rx = x / (vbx + vbxDiff);
            qreal mx = vbx * rx;
            modifiers[0] = mx;            
            debugCallout<<'\t'<<"vbxDiff:"<<vbxDiff<<"x:"<<x<<"rx:"<<rx;
        } else {
            debugCallout<<'\t'<<"right side resize and modifier X is inside viewbox";
            qreal ax = newSize.width();
            qreal bx = m_prevSize.width();
            qreal vbx = viewBox.width();
            qreal rx = ax / bx;
            qreal x = modifiers[0] - viewBox.left();
            qreal mx = x / rx;
            modifiers[0] = mx;            
            debugCallout<<'\t'<<"rx:"<<rx;
        }
        if (pointerTop || pointerBottom) {
            debugCallout<<'\t'<<"modifier Y is ouside viewbox";
            qreal ay = newSize.height();
            qreal by = newPathSize.height();
            qreal vby = viewBox.height();
            qreal ry = (by - ay) / ay;
            qreal my = pointerTop ? -vby * ry : vby + (vby * ry);
            modifiers[1] = my;
        } else if (movedY) {
            debugCallout<<'\t'<<"top side resize and modifier Y is inside viewbox";
            qreal a = newSize.height();
            qreal b = m_prevSize.height();
            qreal vb = viewBox.height();
            qreal rvb = a / b;
            qreal vbDiff = (vb * rvb) - vb;
            qreal y = modifiers[1] + vbDiff;
            qreal ry = y / (vb + vbDiff);
            qreal m = vb * ry;
            modifiers[1] = m;        
            debugCallout<<'\t'<<"vbDiff:"<<vbDiff<<"y:"<<y<<"ry:"<<ry;
        } else {
            debugCallout<<'\t'<<"bottom side resize and modifier Y is inside viewbox";
            qreal a = newSize.height();
            qreal b = m_prevSize.height();
            qreal vb = viewBox.height();
            qreal ry = a / b;
            qreal y = modifiers[1] - viewBox.top();
            qreal m = y / ry;
            modifiers[1] = m;            
            debugCallout<<'\t'<<"ry:"<<ry;
        }
        debugCallout<<'\t'<<"New modifiers:"<<modifiers;

        path->setModifiers(modifiers);
        params = path->parameters(); // get the new parameters
        path->setParameters(params); // set new parameters to get everything updated
    }

    QRectF pathRect(viewBox);
    if (modifiers[0] < pathRect.left()) {
        pathRect.setLeft(modifiers[0]);
    } else if (modifiers[0] > pathRect.right()) {
        pathRect.setRight(modifiers[0]);
    }
    if (modifiers[1] < pathRect.top()) {
        pathRect.setTop(modifiers[1]);
    } else if (modifiers[1] > pathRect.bottom()) {
        pathRect.setBottom(modifiers[1]);
    }
    //debugCallout<<'\t'<<"pathrect:"<<pathRect<<"viewbox:"<<viewBox;
    qreal size_ratioX = pathRect.width() / viewBox.width(); 
    qreal size_ratioY = pathRect.height() / viewBox.height(); 
    qreal pw = newSize.width() * size_ratioX;
    qreal ph = newSize.height() * size_ratioY;
    QSizeF pathSize = QSizeF(pw, ph);
    //debugCallout<<'\t'<<newSize<<"size ratio:"<<size_ratioX<<size_ratioY<<"path size:"<<pathSize;
    path->setSize(pathSize);
    
    qreal x = 0.0;
    qreal y = 0.0;
    if (pathRect.left() < 0.0) {
        x = newSize.width() - pathSize.width();
    }
    if (pathRect.top() < 0.0) {
        y = newSize.height() - pathSize.height();
    }
    path->setPosition(QPointF(x, y));

    KoShape *textShape = path->text();
    if (textShape) {
        textShape->setSize(newSize);
    }
    debugCalloutF<<"<<<<";
}

