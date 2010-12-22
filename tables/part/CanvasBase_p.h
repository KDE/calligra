/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Robert Knight <robertknight@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef CANVASBASE_PRIVATE_H
#define CANVASBASE_PRIVATE_H

#include <QPointer>
#include "CanvasBase.h"

class QLabel;

class KoShapeManager;
class KoToolProxy;

namespace KSpread
{
class View;

class CanvasBase::Private
{
public:
    Doc *doc;
    // Non-visible range top-left from current screen
    // Example: If the first visible column is 'E', then offset stores
    // the width of the invisible columns 'A' to 'D'.
    // Example: If the first visible row is '5', then offset stores
    // the height of the invisible rows '1' to '4'.
    QPointF offset;

    QLabel *validationInfo;

    // flake
    KoShapeManager* shapeManager;
    QPointer<KoToolProxy> toolProxy;
};

} //namespace KSpread

#endif
