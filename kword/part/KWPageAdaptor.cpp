/* This file is part of the KOffice project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

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

#include "KWPageAdaptor.h"
#include "KWPage.h"

KWPageAdaptor::KWPageAdaptor( QObject* parent, KWPage* page )
    //FIXME inherit KWPage from QObject and maybe even merge the adaptor-functionality direct into KWPage?
    : QDBusAbstractAdaptor( parent )
    , m_page( page )
{
}

KWPageAdaptor::~KWPageAdaptor()
{
}

double KWPageAdaptor::width() const { return m_page->width(); }
void KWPageAdaptor::setWidth(double x) { m_page->setWidth(x); }

double KWPageAdaptor::height() const { return m_page->height(); }
void KWPageAdaptor::setHeight(double y) { m_page->setHeight(y); }

double KWPageAdaptor::topMargin() const { return m_page->topMargin(); }
void KWPageAdaptor::setTopMargin(double x) { m_page->setTopMargin(x); }

double KWPageAdaptor::bottomMargin() const { return m_page->bottomMargin(); }
void KWPageAdaptor::setBottomMargin(double y) { m_page->setBottomMargin(y); }

double KWPageAdaptor::leftMargin() const { return m_page->leftMargin(); }
void KWPageAdaptor::setLeftMargin(double l) { m_page->setLeftMargin(l); }

double KWPageAdaptor::rightMargin() const { return m_page->rightMargin(); }
void KWPageAdaptor::setRightMargin(double r) { m_page->setRightMargin(r); }

#include "KWPageAdaptor.moc"
