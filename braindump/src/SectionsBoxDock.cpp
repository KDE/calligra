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

#include <QMenu>

#include <KoIcon.h>

#include "DocumentModel.h"
#include "Section.h"
#include "TreeSortFilter.h"
#include "View.h"
#include "RootSection.h"
#include "Canvas.h"

#include "commands/InsertSectionCommand.h"
#include "commands/RemoveSectionCommand.h"

SectionsBoxDock::SectionsBoxDock() : m_view(0), m_model(0), m_proxy(new TreeSortFilter(this))
{
    QWidget* mainWidget = new QWidget(this);
    setWidget(mainWidget);
    setWindowTitle(i18n("Whiteboards"));

    m_wdgSectionsBox.setupUi(mainWidget);

    // Setup list sections
    connect(m_wdgSectionsBox.listSections, SIGNAL(clicked(QModelIndex)), SLOT(slotSectionActivated(QModelIndex)));
    m_wdgSectionsBox.listSections->setModel(m_proxy);

    // Setup the view mode button
    QMenu* m_viewModeMenu = new QMenu(this);
    QActionGroup *group = new QActionGroup(this);
    QList<QAction*> actions;

    actions << m_viewModeMenu->addAction(koIcon("view-list-text"),
                                         i18n("Minimal View"), this, SLOT(slotMinimalView()));
    actions << m_viewModeMenu->addAction(koIcon("view-list-details"),
                                         i18n("Detailed View"), this, SLOT(slotDetailedView()));
    actions << m_viewModeMenu->addAction(koIcon("view-preview"),
                                         i18n("Thumbnail View"), this, SLOT(slotThumbnailView()));

    for(int i = 0, n = actions.count(); i < n; ++i) {
        actions[i]->setCheckable(true);
        actions[i]->setActionGroup(group);
    }
    actions[1]->trigger(); //TODO save/load previous state

    m_wdgSectionsBox.bnViewMode->setMenu(m_viewModeMenu);
    m_wdgSectionsBox.bnViewMode->setPopupMode(QToolButton::InstantPopup);
    m_wdgSectionsBox.bnViewMode->setIcon(koIcon("view-choose"));
    m_wdgSectionsBox.bnViewMode->setText(i18n("View mode"));

    // Setup the search box
    connect(m_wdgSectionsBox.searchLine, SIGNAL(textChanged(QString)), m_proxy, SLOT(setFilterWildcard(QString)));

    // Setup the add button
    m_wdgSectionsBox.bnAdd->setIcon(koIcon("list-add"));

    QMenu* newSectionMenu = new QMenu(this);
    m_wdgSectionsBox.bnAdd->setMenu(newSectionMenu);
    m_wdgSectionsBox.bnAdd->setPopupMode(QToolButton::MenuButtonPopup);
    connect(m_wdgSectionsBox.bnAdd, SIGNAL(clicked()), SLOT(slotNewSectionBellowCurrent()));
    newSectionMenu->addAction(i18n("Add new whiteboard below current."), this, SLOT(slotNewSectionBellowCurrent()));
    newSectionMenu->addAction(i18n("Add new whiteboard above current."), this, SLOT(slotNewSectionAboveCurrent()));
    m_newSectionAsChild = newSectionMenu->addAction(i18n("Add new whiteboard as child of current."), this, SLOT(slotNewSectionAsChildOfCurrent()));

    // Setup the delete button
    m_wdgSectionsBox.bnDelete->setIcon(koIcon("list-remove"));
    connect(m_wdgSectionsBox.bnDelete, SIGNAL(clicked()), SLOT(slotRmClicked()));

    // Setup the raise button
    m_wdgSectionsBox.bnRaise->setEnabled(false);
    m_wdgSectionsBox.bnRaise->setIcon(koIcon("go-up"));
    connect(m_wdgSectionsBox.bnRaise, SIGNAL(clicked()), SLOT(slotRaiseClicked()));

    // Setup the lower button
    m_wdgSectionsBox.bnLower->setEnabled(false);
    m_wdgSectionsBox.bnLower->setIcon(koIcon("go-down"));
    connect(m_wdgSectionsBox.bnLower, SIGNAL(clicked()), SLOT(slotLowerClicked()));

    // Setup the duplicate button
    m_wdgSectionsBox.bnDuplicate->setIcon(koIcon("edit-copy"));
    connect(m_wdgSectionsBox.bnDuplicate, SIGNAL(clicked()), SLOT(slotDuplicateClicked()));

}

SectionsBoxDock::~SectionsBoxDock()
{
}

void SectionsBoxDock::updateGUI()
{
    m_wdgSectionsBox.bnDelete->setEnabled(m_view->activeSection());
    m_newSectionAsChild->setEnabled(m_view->activeSection());
}

void SectionsBoxDock::setup(RootSection* document, View* view)
{
    m_view = view;
    DocumentModel* model = new DocumentModel(this, document);
    m_proxy->setSourceModel(model);
    delete m_model;
    m_model = model;

    connect(m_model, SIGNAL(activeSectionChanged(Section*)), SLOT(slotSectionActivated(Section*)));
    connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(insertedSection(QModelIndex,int)));
    connect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), SLOT(removedSection()));

    updateGUI();
}

void SectionsBoxDock::slotSectionActivated(const QModelIndex& index)
{
    Section* section = qVariantValue<Section*>(m_proxy->data(index, DocumentModel::SectionPtr));
    m_view->setActiveSection(section);
}

void SectionsBoxDock::slotSectionActivated(Section* section)
{
    m_view->setActiveSection(section);
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
    Q_ASSERT(m_view->activeSection());

    m_view->rootSection()->addCommand(m_view->activeSection(), new RemoveSectionCommand(m_view->activeSection(), m_model));
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
    if(m_view->activeSection()) {
        Section* section = new Section(*m_view->activeSection());
        m_view->rootSection()->addCommand(section,
                                          new InsertSectionCommand(m_view->rootSection()->sectionsIO(), section, m_view->activeSection()->sectionParent(), m_model,
                                                  m_view->activeSection()->sectionParent()->nextSection(m_view->activeSection())));
    }
}

void SectionsBoxDock::slotNewSectionAsChildOfCurrent()
{
    Q_ASSERT(m_view->activeSection());
    Section* section = new Section(m_view->rootSection());
    section->setName(SectionGroup::nextName());
    m_view->rootSection()->addCommand(section, new InsertSectionCommand(m_view->rootSection()->sectionsIO(), section, m_view->activeSection(), m_model, 0));
}

void SectionsBoxDock::slotNewSectionAboveCurrent()
{
    SectionGroup* parentSection = m_view->activeSection() ? m_view->activeSection()->sectionParent() : m_view->rootSection();
    Section* section = new Section(m_view->rootSection());
    section->setName(SectionGroup::nextName());
    m_view->rootSection()->addCommand(section, new InsertSectionCommand(m_view->rootSection()->sectionsIO(), section, parentSection, m_model, m_view->activeSection()));
}

void SectionsBoxDock::slotNewSectionBellowCurrent()
{
    SectionGroup* parentSection = m_view->activeSection() ? m_view->activeSection()->sectionParent() : m_view->rootSection();
    Section* above = parentSection->nextSection(m_view->activeSection());
    Section* section = new Section(m_view->rootSection());
    section->setName(SectionGroup::nextName());
    m_view->rootSection()->addCommand(section, new InsertSectionCommand(m_view->rootSection()->sectionsIO(), section, parentSection, m_model, above));
}

void SectionsBoxDock::selectSection(Section* section)
{
    QModelIndex index = m_proxy->mapFromSource(m_model->index(section));
    m_wdgSectionsBox.listSections->setExpanded(index, true);
    m_wdgSectionsBox.listSections->setCurrentIndex(index);
    slotSectionActivated(index);
}

void SectionsBoxDock::removedSection()
{
    if(m_model->rowCount() == 0) {
        m_view->setActiveSection(0);
    } else {
        slotSectionActivated(m_wdgSectionsBox.listSections->currentIndex());
    }
}

void SectionsBoxDock::insertedSection(const QModelIndex& parent, int idx)
{
    QModelIndex index = m_proxy->mapFromSource(m_model->index(idx, 0, parent));
    m_wdgSectionsBox.listSections->setExpanded(index, true);
    m_wdgSectionsBox.listSections->setCurrentIndex(index);
    slotSectionActivated(index);
}
