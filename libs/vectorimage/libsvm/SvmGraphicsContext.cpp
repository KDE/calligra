/* This file is part of the Calligra project

  SPDX-FileCopyrightText: 2011 Inge Wallin <inge@lysator.liu.se>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "SvmGraphicsContext.h"

#include <QtGlobal>

namespace Libsvm
{

SvmGraphicsContext::SvmGraphicsContext()
    : lineColor(Qt::black)
    , lineColorSet(true)
    , fillColor(Qt::white)
    , fillColorSet(false)
    , textColor(Qt::black)
    , textFillColor(Qt::black)
    , textFillColorSet(false)
    , textAlign(ALIGN_TOP) // FIXME: Correct?
    , mapMode()
    , font("Helvetica", 300) // 300 is of course a completely arbitrary value
    , overlineColor(Qt::black)
    , overlineColorSet(false)
    //... more here
    , changedItems(0xffffffff) // Everything changed the first time.
{
}
}
