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

#include <KMenu>

#include "DocumentModel.h"
#include "Section.h"
#include "TreeSortFilter.h"
#include "View.h"
#include "Document.h"

SectionsBoxDock::SectionsBoxDock() : m_model(0), m_proxy(new TreeSortFilter(this)) {
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
  connect(m_wdgSectionsBox.searchLine, SIGNAL(textChanged(QString)), m_proxy, SLOT(setFilterWildcard(QString)));
  
  // Setup the add button
  m_wdgSectionsBox.bnAdd->setIcon(SmallIcon("list-add"));
    
  KMenu* newSectionMenu = new KMenu(this);
  m_wdgSectionsBox.bnAdd->setMenu(newSectionMenu);
  m_wdgSectionsBox.bnAdd->setPopupMode(QToolButton::MenuButtonPopup);
  connect(m_wdgSectionsBox.bnAdd, SIGNAL(clicked()), SLOT(slotNewSectionBellowCurrent()));
  newSectionMenu->addAction(i18n("Add new section bellow current."), this, SLOT(slotNewSectionBellowCurrent()));
  newSectionMenu->addAction(i18n("Add new section as child of current."), this, SLOT(slotNewSectionAsChildOfCurrent()));
      
  // Setup the delete button
  m_wdgSectionsBox.bnDelete->setIcon(SmallIcon("list-remove"));
  connect(m_wdgSectionsBox.bnDelete, SIGNAL(clicked()), SLOT(slotRmClicked()));

  // Setup the raise button
  m_wdgSectionsBox.bnRaise->setEnabled(false);
  m_wdgSectionsBox.bnRaise->setIcon(SmallIcon("go-up"));
  connect(m_wdgSectionsBox.bnRaise, SIGNAL(clicked()), SLOT(slotRaiseClicked()));

  // Setup the lower button
  m_wdgSectionsBox.bnLower->setEnabled(false);
  m_wdgSectionsBox.bnLower->setIcon(SmallIcon("go-down"));
  connect(m_wdgSectionsBox.bnLower, SIGNAL(clicked()), SLOT(slotLowerClicked()));

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
  Section* section = qVariantValue<Section*>(m_proxy->data(index, DocumentModel::SectionPtr));
  m_view->doUpdateActiveSection(section);
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

void SectionsBoxDock::slotRmClicked()
{
  m_model->removeSection(m_view->activeSection());
  slotSectionActivated(m_wdgSectionsBox.listSections->currentIndex());
}
void SectionsBoxDock::slotRaiseClicked()
{
  qFatal("Unimplemented");
}

void SectionsBoxDock::slotLowerClicked()
{
  qFatal("Unimplemented");
}

void SectionsBoxDock::slotDuplicateClicked()
{
}

void SectionsBoxDock::slotNewSectionAsChildOfCurrent()
{
  Section* section = new Section();
  m_model->insertSection( section, m_view->activeSection(), 0);
  m_wdgSectionsBox.listSections->setCurrentIndex(m_model->index(section));
}

void SectionsBoxDock::slotNewSectionBellowCurrent()
{
  Section* section = new Section();
  m_model->insertSection( section, m_view->activeSection()->sectionParent(), m_view->activeSection());
  Q_ASSERT(section->sectionParent());
  Q_ASSERT(section->sectionParent() == m_view->activeSection()->sectionParent());
  m_wdgSectionsBox.listSections->setCurrentIndex( m_proxy->mapFromSource( m_model->index(section)) );
}

#include "SectionsBoxDock.moc"
