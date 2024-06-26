/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCREATESHAPESTOOL_H
#define KOCREATESHAPESTOOL_H

#include "KoInteractionTool.h"

#include "flake_export.h"

#include <QString>

class KoCanvasBase;
class KoProperties;
class KoCreateShapesToolPrivate;

#define KoCreateShapesTool_ID "CreateShapesTool"

/**
 * A tool to create shapes with.
 */
class FLAKE_EXPORT KoCreateShapesTool : public KoInteractionTool
{
    Q_OBJECT
public:
    /**
     * Create a new tool; typically not called by applications, only by the KoToolManager
     * @param canvas the canvas this tool works for.
     */
    explicit KoCreateShapesTool(KoCanvasBase *canvas);
    /// destructor
    ~KoCreateShapesTool() override;
    void mouseReleaseEvent(KoPointerEvent *event) override;
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;

    void paint(QPainter &painter, const KoViewConverter &converter) override;

    /**
     * Each shape-type has an Id; as found in KoShapeFactoryBase::id().id(), to choose which
     * shape this controller should actually create; set the id before the user starts to
     * create the new shape.
     * @param id the SHAPEID of the to be generated shape
     */
    void setShapeId(const QString &id);
    /**
     * return the shape Id that is to be created.
     * @return the shape Id that is to be created.
     */
    QString shapeId() const;

    /**
     * Set the shape properties that the create controller will use for the next shape it will
     * create. The tool does not take ownership of the object.
     * @param properties the properties or 0 if the default shape should be created.
     */
    void setShapeProperties(const KoProperties *properties);
    /**
     * return the properties to be used for creating the next shape
     * @return the properties to be used for creating the next shape
     */
    const KoProperties *shapeProperties();

protected:
    KoInteractionStrategy *createStrategy(KoPointerEvent *event) override;

private:
    Q_DECLARE_PRIVATE(KoCreateShapesTool)
};

#endif
