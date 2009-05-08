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

#include <QSortFilterProxyModel>
#include <KMenu>

#include "DocumentModel.h"
#include "View.h"

SectionsBoxDock::SectionsBoxDock() : m_model(0), m_proxy(new QSortFilterProxyModel(this)) {
  QWidget* mainWidget = new QWidget(this);
  setWidget(mainWidget);

  m_wdgSectionsBox.setupUi(mainWidget);
  
  // Setup list sections
  connect(m_wdgSectionsBox.listSections, SIGNAL(clicked(const QModelIndex&)), SLOT(slotSectionActivated(const QModelIndex&)));
  m_wdgSectionsBox.listSections->setModel(m_proxy);

  // Setup the view mode button
  KMenu* m_viewModeMenu = new KMenu(this);
  QActionGroup *group = new QActionGroup(this);
  QList<QAction*> actions;

  actions << m_viewModeMenu->addAction(KIcon("view-list-text"),
                                        i18n("Minimal View"), this, SLOT(slotMinimalView()));
  actions << m_viewModeMenu->addAction(KIcon("view-list-details"),
                                        i18n("Detailed View"), this, SLOT(slotDetailedView()));
  actions << m_viewModeMenu->addAction(KIcon("view-preview"),
                                        i18n("Thumbnail View"), this, SLOT(slotThumbnailView()));

  for (int i = 0, n = actions.count(); i < n; ++i) {
      actions[i]->setCheckable(true);
      actions[i]->setActionGroup(group);
  }
  actions[1]->trigger(); //TODO save/load previous state
  
  m_wdgSectionsBox.bnViewMode->setMenu(m_viewModeMenu);
  m_wdgSectionsBox.bnViewMode->setPopupMode(QToolButton::InstantPopup);
  m_wdgSectionsBox.bnViewMode->setIcon(KIcon("view-choose"));
  m_wdgSectionsBox.bnViewMode->setText(i18n("View mode"));
  
  // Setup the search box
  connect(m_wdgSectionsBox.searchLine, SIGNAL(textChanged(QString)), SLOT(searchBoxUpdated(QString)));
}

SectionsBoxDock::~SectionsBoxDock()
{
}

void SectionsBoxDock::setup(Document* document, View* view)
{
  m_view = view;
  DocumentModel* model = new DocumentModel(this, document);
  m_proxy->setSourceModel(model);
  delete m_model;
  m_model = model;
}

void SectionsBoxDock::slotSectionActivated(const QModelIndex& index)
{
  m_view->doUpdateActiveSection(m_model->dataFromIndex(index));
}

void SectionsBoxDock::slotMinimalView()
{
  m_wdgSectionsBox.listSections->setDisplayMode(KoDocumentSectionView::MinimalMode);
}

void SectionsBoxDock::slotDetailedView()
{
  m_wdgSectionsBox.listSections->setDisplayMode(KoDocumentSectionView::DetailedMode);
}

void SectionsBoxDock::slotThumbnailView()
{
  m_wdgSectionsBox.listSections->setDisplayMode(KoDocumentSectionView::ThumbnailMode);
}

void SectionsBoxDock::searchBoxUpdated(QString str)
{
  m_proxy->setFilterRegExp(QRegExp(str));
}


#include "SectionsBoxDock.moc"
