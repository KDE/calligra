/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CalloutShape.h"
#include "CalloutContainerModel.h"
#include "CalloutDebug.h"
#include "CalloutShapeFactory.h"

#include "EnhancedPathCommand.h"
#include "EnhancedPathFormula.h"
#include "EnhancedPathHandle.h"

#include <KoColorBackground.h>
#include <KoOdfWorkaround.h>
#include <KoPathPoint.h>
#include <KoShapeSavingContext.h>
#include <KoShapeStroke.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <KoShape_p.h>

#include <QPainter>
#include <QPainterPath>

PathShape::PathShape(const QRect &viewBox)
    : EnhancedPathShape(viewBox)
{
    qInfo() << Q_FUNC_INFO << this;
    setShapeId(CalloutPathShapeId);
    // FIXME: shapes needs more fine-grained control of interactions:
    // The tool just checks isEditable() (KoShape::ResizeAllowed|KoShape::MoveAllowed)
    setAllowedInteractions(KoShape::SelectionAllowed);
}

// void PathShape::shapeChanged(ChangeType type, KoShape *shape)
// {
//     qInfo()<<Q_FUNC_INFO<<type<<shape;
// //     if (parent() && parent()->model() && type == KoShape::ParameterChanged) {
// //         parent()->model()->childChanged(this, type);
// //         return;
// //     }
//     EnhancedPathShape::shapeChanged(type, shape);
// }

KoProperties PathShape::parameters() const
{
    KoProperties params;

    params.setProperty("viewBox", m_viewBox);

    if (m_pathStretchPointX != -1) {
        params.setProperty("path-stretchpoint-x", m_pathStretchPointX);
    }
    if (m_pathStretchPointY != -1) {
        params.setProperty("path-stretchpoint-y", m_pathStretchPointY);
    }

    if (m_mirrorHorizontally) {
        params.setProperty("mirror-horizontal", "true");
    }
    if (m_mirrorVertically) {
        params.setProperty("mirror-vertical", "true");
    }

    QString modifiers;
    foreach (qreal modifier, m_modifiers) {
        modifiers += QString::number(modifier) + ' ';
    }
    params.setProperty("modifiers", modifiers.trimmed());

    if (m_textArea.count() >= 4) {
        params.setProperty("text-areas", m_textArea.join(" "));
    }

    QStringList path;
    foreach (EnhancedPathCommand *c, m_commands) {
        path << c->toString();
    }
    params.setProperty("commands", path);

    CalloutShapeFactory::ComplexType formulae;
    FormulaStore::const_iterator i = m_formulae.constBegin();
    for (; i != m_formulae.constEnd(); ++i) {
        formulae[i.key()] = i.value()->toString();
    }
    params.setProperty("formulae", formulae);

    CalloutShapeFactory::ListType handles;
    CalloutShapeFactory::ComplexType handle;
    foreach (EnhancedPathHandle *h, m_enhancedHandles) {
        QString pos = h->positionX() + ' ' + h->positionY();
        handle["draw:handle-position"] = pos;
    }
    handles.append(QVariant(handle));
    params.setProperty("handles", handles);

    return params;
}

void PathShape::setModifiers(const QList<qreal> &modifiers)
{
    if (m_modifiers.count() < 2) {
        m_modifiers = modifiers;
    } else {
        modifyReference("$0", modifiers.at(0));
        modifyReference("$1", modifiers.at(1));
    }
}

void PathShape::setViewBox(const QRect &box)
{
    m_viewBox = box;
}

QList<qreal> PathShape::parseModifiers(const QString &m) const
{
    QList<qreal> lst;
    if (m.isEmpty()) {
        return lst;
    }
    QStringList tokens = m.simplified().split(' ');
    int tokenCount = tokens.count();
    for (int i = 0; i < tokenCount; ++i) {
        lst.append((qreal)tokens[i].toDouble());
    }
    return lst;
}

void PathShape::setParameters(const KoProperties &params)
{
    reset();

    QVariant viewboxData;
    params.property("viewBox", viewboxData);
    setViewBox(viewboxData.toRect());
    //     qInfo()<<Q_FUNC_INFO<<"viewBox:"<<m_viewBox;

    QList<qreal> modifiers = parseModifiers(params.stringProperty("modifiers"));
    Q_ASSERT(modifiers.count() >= 2);
    m_modifiers = modifiers;
    //     qInfo()<<Q_FUNC_INFO<<"modifiers:"<<m_modifiers;

    setStroke(new KoShapeStroke(1.0));

    CalloutShapeFactory::ListType handles = params.property("handles").toList();
    foreach (const QVariant &v, handles) {
        addHandle(v.toMap());
        //         qInfo()<<Q_FUNC_INFO<<"handle:"<<v.toMap();
    }
    CalloutShapeFactory::ComplexType formulae = params.property("formulae").toMap();
    CalloutShapeFactory::ComplexType::const_iterator formula = formulae.constBegin();
    CalloutShapeFactory::ComplexType::const_iterator lastFormula = formulae.constEnd();
    for (; formula != lastFormula; ++formula) {
        addFormula(formula.key(), formula.value().toString());
        //         qInfo()<<Q_FUNC_INFO<<"formula:"<<formula.key()<<formula.value().toString();
    }
    QStringList commands = params.property("commands").toStringList();
    foreach (const QString &cmd, commands) {
        addCommand(cmd);
        //         qInfo()<<Q_FUNC_INFO<<"command:"<<cmd;
    }
    QVariant color;
    if (params.property("background", color)) {
        setBackground(QSharedPointer<KoColorBackground>(new KoColorBackground(color.value<QColor>())));
    }
    if (params.contains("text-areas")) {
        m_textArea = params.stringProperty("text-areas").split(' ');
    }
}

KoTextShapeDataBase *PathShape::textData() const
{
    KoTextShapeDataBase *textData = nullptr;
    KoShape *ts = textShape();
    if (ts) {
        textData = qobject_cast<KoTextShapeDataBase *>(ts->userData());
    }
    return textData;
}

//-----------------------------------------------------------------
CalloutShape::CalloutShape(const KoProperties *params)
    : KoShapeContainer(new CalloutContainerModel())
    , m_path(nullptr)
    , m_type("callout")
{
    if (params->contains("type")) {
        m_type = params->stringProperty("type");
    }
    m_path = new PathShape();
    m_path->setParameters(*params);
    addShape(m_path);
    setClipped(m_path, false);
    setInheritsTransform(m_path, true);
}

CalloutShape::~CalloutShape() = default;

void CalloutShape::saveOdf(KoShapeSavingContext &context) const
{
    Q_ASSERT(m_path);

    context.xmlWriter().startElement("draw:custom-shape");

    // NOTE:
    // A callout shape position and size shall be the position and size
    // of the 'bubble' part of the callout.
    // The viewbox shall also be the size of the 'bubble' part.
    // This implies that the 'pointer' part will (normally)
    // go *outside* the viewbox (and shape).
    // This is imo (danders) not according to odf spec,
    // but is the way LO/OO does it.

    saveOdfAttributes(context, OdfTransformation | OdfSize);

    m_path->saveOdfAttributes(context, OdfAllAttributes & ~OdfTransformation & ~OdfSize & ~OdfViewbox);
    m_path->saveText(context);
    m_path->saveEnhancedGeometry(context);
}

bool CalloutShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    const KoXmlElement enhancedGeometry(KoXml::namedItemNS(element, KoXmlNS::draw, "enhanced-geometry"));
    if (!enhancedGeometry.isNull()) {
        m_type = enhancedGeometry.attributeNS(KoXmlNS::draw, "type", "callout");

        m_path->loadEnhancedGeometry(enhancedGeometry, context);
    }

    loadOdfAttributes(element, context, OdfAllAttributes);

    return true;
}

KoTextShapeDataBase *CalloutShape::textData() const
{
    return m_path ? m_path->textData() : nullptr;
}

void CalloutShape::setTextArea(const QRectF &rect)
{
    Q_UNUSED(rect);
    // setPreferredTextRect(rect);
}

QRectF CalloutShape::boundingRect() const
{
    return KoShapeContainer::boundingRect() | m_path->boundingRect();
}

void CalloutShape::paintComponent(QPainter & /*painter*/, const KoViewConverter & /*converter*/, KoShapePaintingContext & /*paintcontext*/)
{
}
