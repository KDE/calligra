/* This file is part of the KOffice project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
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

#include "Page.h"
#include "KWPage.h"

using namespace Scripting;

Page::Page( QObject* parent, KWPage* page )
    : QObject( parent )
    , m_page( page )
{
}

Page::~Page()
{
}

int Page::pageNumber() const { return m_page->pageNumber(); }

QString Page::pageSide() const
{
    switch( m_page->pageSide() ) {
        case KWPage::Left: return "Left";
        case KWPage::Right: return "Right";
        case KWPage::PageSpread: return "Spread";
    }
    return QString();
}

void Page::setPageSide(const QString& ps)
{
    if(ps == "Left")
        m_page->setPageSide(KWPage::Left);
    else if(ps == "Right")
        m_page->setPageSide(KWPage::Right);
    else if(ps == "Spread")
        m_page->setPageSide(KWPage::PageSpread);
}

double Page::width() const { return m_page->width(); }
void Page::setWidth(double x) { m_page->setWidth(x); }

double Page::height() const { return m_page->height(); }
void Page::setHeight(double y) { m_page->setHeight(y); }

double Page::topMargin() const { return m_page->topMargin(); }
void Page::setTopMargin(double x) { m_page->setTopMargin(x); }

double Page::bottomMargin() const { return m_page->bottomMargin(); }
void Page::setBottomMargin(double y) { m_page->setBottomMargin(y); }

double Page::leftMargin() const { return m_page->leftMargin(); }
void Page::setLeftMargin(double l) { m_page->setLeftMargin(l); }

double Page::rightMargin() const { return m_page->rightMargin(); }
void Page::setRightMargin(double r) { m_page->setRightMargin(r); }

#include "Page.moc"
