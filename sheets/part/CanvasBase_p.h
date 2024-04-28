/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2006 Robert Knight <robertknight@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CANVASBASE_PRIVATE_H
#define CANVASBASE_PRIVATE_H

#include "CanvasBase.h"
#include <QPointer>

class KoShapeManager;
class KoToolProxy;

namespace Calligra
{
namespace Sheets
{

class Q_DECL_HIDDEN CanvasBase::Private
{
public:
    Doc *doc;
    // Non-visible range top-left from current screen
    // Example: If the first visible column is 'E', then offset stores
    // the width of the invisible columns 'A' to 'D'.
    // Example: If the first visible row is '5', then offset stores
    // the height of the invisible rows '1' to '4'.
    QPointF offset;

    // flake
    KoShapeManager *shapeManager;
    QPointer<KoToolProxy> toolProxy;
};

} // namespace Sheets
} // namespace Calligra

#endif
