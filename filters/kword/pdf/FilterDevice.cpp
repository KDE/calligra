/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "FilterDevice.h"

#include "GfxState.h"
#include "Link.h"

#include "FilterPage.h"
#include "misc.h"


FilterDevice::FilterDevice(QDomDocument &document, QDomElement &mainFrameset)
    : _document(document), _mainFrameset(mainFrameset), _page(0)
{}

FilterDevice::~FilterDevice()
{
    delete _page;
}

void FilterDevice::startPage(int, GfxState *)
{
    _page = new FilterPage(_document, _mainFrameset);
}

void FilterDevice::endPage()
{
    _page->coalesce();
    _page->prepare();
    _page->dump();
    delete _page;
    _page = 0;
}

void FilterDevice::updateFont(GfxState *state)
{
    _page->updateFont(state);
}

void FilterDevice::beginString(GfxState *state, GString *)
{
    _page->beginString(state, state->getCurX(), state->getCurY());
}

void FilterDevice::endString(GfxState *)
{
    _page->endString();
}

void FilterDevice::drawChar(GfxState *state, double x, double y,
                            double dx, double dy, double, double,
                            CharCode, Unicode *u, int uLen)
{
    _page->addChar(state, x, y, dx, dy, u, uLen);
}

void FilterDevice::drawLink(Link* link, Catalog *cat)
{
    double x1, y1, x2, y2, w;
    link->getBorder(&x1, &y1, &x2, &y2, &w);

    int ux1, uy1, ux2, uy2;
    cvtUserToDev(x1, y1, &ux1, &uy1);
    cvtUserToDev(x2, y2, &ux2, &uy2);

    LinkAction *action = link->getAction();
    FilterLink *flink = new FilterLink(ux1 ,ux2, uy1, uy2, *action, *cat);
    _page->addLink(flink);
}
