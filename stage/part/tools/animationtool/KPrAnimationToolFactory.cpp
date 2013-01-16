/* This file is part of the KDE project
 *
 *  Copyright (C) 2008 C. Boemann <cbo@boemann.dk>
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

#include "KPrAnimationToolFactory.h"
#include "KPrAnimationTool.h"
#include "KPrView.h"
#include <KoPACanvas.h>
#include <KoIcon.h>

#include <klocale.h>
#include <kdebug.h>

KPrAnimationToolFactory::KPrAnimationToolFactory()
    : KoToolFactoryBase("Animation Tool")
{
    setToolTip(i18n("Animation"));
    setToolType("calligrastage");
    setPriority(40);
    setIconName(koIconNameCStr("animation-stage"));
    setActivationShapeId("flake/edit");
}

KPrAnimationToolFactory::~KPrAnimationToolFactory()
{
}

bool KPrAnimationToolFactory::canCreateTool(KoCanvasBase *canvas) const
{
    KoPACanvas *paCanvas =dynamic_cast<KoPACanvas *>(canvas);
    return paCanvas!=0; // we only work in KPresenter
}

KoToolBase* KPrAnimationToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KPrAnimationTool(canvas);
}
