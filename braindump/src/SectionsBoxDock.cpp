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

#include "SectionsBoxDock.h"

#include "DocumentModel.h"
#include "View.h"

SectionsBoxDock::SectionsBoxDock() : m_model(0) {
  QWidget* mainWidget = new QWidget(this);
  setWidget(mainWidget);

  m_wdgSectionsBox.setupUi(mainWidget);
  connect(m_wdgSectionsBox.listSections, SIGNAL(clicked(const QModelIndex&)), SLOT(slotSectionActivated(const QModelIndex&)));
}

SectionsBoxDock::~SectionsBoxDock()
{
}

void SectionsBoxDock::setup(Document* document, View* view)
{
  m_view = view;
  DocumentModel* model = new DocumentModel(this, document);
  m_wdgSectionsBox.listSections->setModel(model);
  delete m_model;
  m_model = model;
}

void SectionsBoxDock::slotSectionActivated(const QModelIndex& index)
{
  m_view->doUpdateActiveSection(m_model->dataFromIndex(index));
}

#include "SectionsBoxDock.moc"
