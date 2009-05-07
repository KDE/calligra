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

#include "BrainDumpView.h"

#include <klocale.h>

#include <KoShapeManager.h>
#include <KoSelection.h>
#include <Canvas.h>

#include "Document.h"

BrainDumpView::BrainDumpView(Document* document, QWidget* parent)
  : KoPAView(document, parent), m_document(document)
{
    Q_ASSERT(m_document);

    setXMLFile("braindump.rc");

    initializeActions();
    initializeGUI();

    connect(m_document, SIGNAL(updateGui()), this, SLOT(updateGui()));
}

BrainDumpView::~BrainDumpView()
{
}

Document* BrainDumpView::document() const
{
    return m_document;
}

void BrainDumpView::initializeGUI()
{
}

void BrainDumpView::initializeActions()
{
}

void BrainDumpView::updateGui()
{
    selectionChanged();
}

#include "BrainDumpView.moc"
