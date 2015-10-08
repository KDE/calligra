/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <KLocalizedString>

#include <KoIcon.h>

#include "WebToolFactory.h"
#include "WebTool.h"
#include "WebShape.h"

WebToolFactory::WebToolFactory()
    : KoToolFactoryBase("WebToolFactoryID")
{
    setToolTip(i18n("Web shape editing"));
    setToolType(dynamicToolType());
    setIconName(koIconName("applications-internet"));
    setPriority(1);
    setActivationShapeId(WEBSHAPEID);
}

WebToolFactory::~WebToolFactory()
{
}

KoToolBase* WebToolFactory::createTool(KoCanvasBase * canvas)
{
    return new WebTool(canvas);
}
