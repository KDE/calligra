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

#include "FrameSet.h"
#include "TextDocument.h"

#include <KWFrameSet.h>
#include <KWTextFrameSet.h>

using namespace Scripting;

FrameSet::FrameSet( QObject* parent, KWFrameSet* frameset )
    : QObject( parent )
    , m_frameset( frameset )
{
}

FrameSet::~FrameSet()
{
}

const QString FrameSet::name() { return m_frameset->name(); }
void FrameSet::setName(const QString &name) { m_frameset->setName(name); }

QObject* FrameSet::textDocument()
{
    KWTextFrameSet* textframeset = dynamic_cast< KWTextFrameSet* >( (KWFrameSet*)m_frameset );
    return textframeset ? new TextDocument(this, textframeset->document()) : 0;
}

#include "FrameSet.moc"
