/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#ifndef KOENHANCEDPATHSHAPE_H
#define KOENHANCEDPATHSHAPE_H

#include <KoParameterShape.h>
#include <QList>
#include <QMap>

#define KoEnhancedPathShapeId "KoEnhancedPathShape"

class KoEnhancedPathCommand;
class KoEnhancedPathHandle;
class KoEnhancedPathFormula;
class KoEnhancedPathParameter;

/**
 * An enhanced shape is a custom shape which can be defined
 * by enhanced geometry data.
 * The data consists of a list of commands like moveto,
 * lineto, curveto, etc. which are used to create the outline
 * of the shape. The coordinates or parameters of the commands
 * can be constant values, named variables (identifiers),
 * modifiers, functions or formulae.
*/
class KoEnhancedPathShape : public KoParameterShape
{
public:
    KoEnhancedPathShape();
    ~KoEnhancedPathShape();

    /**
     * Evaluates the given reference to a identifier, modifier or formula.
     * @param reference the reference to evaluate
     * @return the result of the evaluation
     */
    double evaluateReference( const QString &reference );

    /**
     * Attempts to modify a given reference.
     *
     * Only modifiers can me modified, others silently ignore the attempt.
     *
     * @param reference the reference to modify
     * @param value the new value
     */
    void modifyReference( const QString &reference, double value );

    // from KoShape
    virtual void resize( const QSizeF &newSize );

protected:
    // from KoParameterShape
    void moveHandleAction( int handleId, const QPointF & point, Qt::KeyboardModifiers modifiers = Qt::NoModifier );
    // from KoParameterShape
    void updatePath( const QSizeF &size );
private:
    /// Returns parameter from given textual representation
    KoEnhancedPathParameter * parameter( const QString & text );

    typedef QMap<QString, KoEnhancedPathFormula*> FormulaStore;
    typedef QList<double> ModifierStore;
    typedef QMap<QString, KoEnhancedPathParameter*> ParameterStore;

    QList<KoEnhancedPathCommand*> m_commands; ///< the commands creating the outline
    QList<KoEnhancedPathHandle*> m_enhancedHandles; ///< the handles for modifiying the shape
    FormulaStore m_formulae;     ///< the formulae
    ModifierStore m_modifiers;   ///< the modifier values
    ParameterStore m_parameters; ///< the shared parameters
};

#endif // KOENHANCEDPATHSHAPE_H
