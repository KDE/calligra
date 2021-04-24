/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001-2003 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCURVEFIT_H
#define KOCURVEFIT_H

#include <QList>
#include <QPointF>

#include "flake_export.h"

class KoPathShape;

/*
 * Fits bezier curve to given list of points.
 *
 * An Algorithm for Automatically Fitting Digitized Curves
 * by Philip J. Schneider
 * from "Graphics Gems", Academic Press, 1990
 *
 * http://www.acm.org/pubs/tog/GraphicsGems/gems/FitCurves.c
 * http://www.acm.org/pubs/tog/GraphicsGems/gems/README
 *
 * @param points the list of points to fit curve to
 * @param error the max. fitting error
 * @return a path shape representing the fitted curve
 */

FLAKE_EXPORT KoPathShape * bezierFit(const QVector<QPointF> &points, float error);

#endif

