/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KO_SHAPE_CONFIG_FACTORY_
#define _KO_SHAPE_CONFIG_FACTORY_

#include "flake_export.h"

#include <QString>

class KoShape;
class KoShapeConfigWidgetBase;

/**
 * A factory that creates config panels (widgets) for just a created shape.
 * The KoCreateShapesTool is able to show a number of configuration panels after
 * it created a shape via user interaction.  Each shape configuration panel type
 * has its own factory, which will inherit from this class.
 * @see KoShapeFactoryBase::panelFactories()
 * @see KoShapeConfigWidgetBase
 */
class FLAKE_EXPORT KoShapeConfigFactoryBase
{
public:
    /// default constructor
    KoShapeConfigFactoryBase();
    virtual ~KoShapeConfigFactoryBase();

    /**
     * create a new config widget, initialized with the param shape
     * @param shape the shape that will be configured in the config widget.
     * @see KoShapeConfigWidgetBase::open()
     */
    virtual KoShapeConfigWidgetBase *createConfigWidget(KoShape *shape) = 0;
    /// return the (translated) name of this configuration
    virtual QString name() const = 0;

    /**
     * Return a sorting ordering to specify where in the list of config widgets this
     * one will be shown.
     * Higher sorting numbers will be shown first. The default is 1.
     */
    virtual int sortingOrder() const
    {
        return 1;
    }

    /**
     * Return true if the createConfigWidget() should be called at all for a shape of
     * the specified type.
     * @param id an ID like the KoShapeFactoryBase::shapeId()
     */
    virtual bool showForShapeId(const QString &id) const
    {
        Q_UNUSED(id);
        return true;
    }

    /// \internal a compare for sorting.
    static bool compare(KoShapeConfigFactoryBase *f1, KoShapeConfigFactoryBase *f2)
    {
        return f1->sortingOrder() - f2->sortingOrder() > 0;
    }
};

#endif
