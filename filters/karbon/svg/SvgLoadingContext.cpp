/* This file is part of the KDE project
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
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

#include "SvgLoadingContext.h"
#include "SvgGraphicContext.h"
#include "SvgUtil.h"

#include <KDebug>

#include <QtCore/QStack>

class SvgLoadingContext::Private
{
public:
    Private()
    {

    }

    ~Private()
    {
        if (! gcStack.isEmpty())
            kWarning() << "the context stack is not empty (current count" << gcStack.size() << ", expected 0)";
        qDeleteAll(gcStack);
        gcStack.clear();
    }
    QStack<SvgGraphicsContext*> gcStack;
};

SvgLoadingContext::SvgLoadingContext()
    : d(new Private())
{

}

SvgLoadingContext::~SvgLoadingContext()
{
    delete d;
}

SvgGraphicsContext *SvgLoadingContext::currentGC()
{
    if (d->gcStack.isEmpty())
        return 0;

    return d->gcStack.top();
}

SvgGraphicsContext *SvgLoadingContext::pushGraphicsContext(const KoXmlElement &element, bool inherit)
{
    SvgGraphicsContext *gc = new SvgGraphicsContext;

    // copy data from current context
    if (! d->gcStack.isEmpty() && inherit)
        *gc = *(d->gcStack.top());

    gc->filterId.clear(); // filters are not inherited
    gc->clipPathId.clear(); // clip paths are not inherited
    gc->display = true; // display is not inherited
    gc->opacity = 1.0; // opacity is not inherited

    if (!element.isNull()) {
        if (element.hasAttribute("transform")) {
            QTransform mat = SvgUtil::parseTransform(element.attribute("transform"));
            gc->matrix = mat * gc->matrix;
        }
        if (element.hasAttribute("xml:base"))
            gc->xmlBaseDir = element.attribute("xml:base");
        if (element.hasAttribute("xml:space"))
            gc->preserveWhitespace = element.attribute("xml:space") == "preserve";
    }

    d->gcStack.push(gc);

    return gc;
}

void SvgLoadingContext::popGraphicsContext()
{
    delete(d->gcStack.pop());
}
