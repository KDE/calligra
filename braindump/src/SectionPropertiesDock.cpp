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

#include "SectionPropertiesDock.h"
#include "LayoutFactoryRegistry.h"
#include "Section.h"
#include "Layout.h"
#include "commands/ChangeLayoutCommand.h"
#include "RootSection.h"

SectionPropertiesDock::SectionPropertiesDock() :
    m_currentSection(0), m_rootSection(0)
{
    QWidget* mainWidget = new QWidget(this);
    setWidget(mainWidget);
    setWindowTitle(i18n("Dump properties"));

    m_wdgSectionProperties.setupUi(mainWidget);

    typedef QPair<QString, QString> PairType;
    foreach(const PairType & pair, LayoutFactoryRegistry::instance()->factories()) {
        m_wdgSectionProperties.comboBoxLayout->addItem(pair.second, pair.first);
    }
    connect(m_wdgSectionProperties.comboBoxLayout, SIGNAL(currentIndexChanged(int)), SLOT(layoutChanged(int)));
}

SectionPropertiesDock::~SectionPropertiesDock()
{
}

void SectionPropertiesDock::setRootSection(RootSection* _rootSection)
{
    Q_ASSERT(m_rootSection == 0);
    m_rootSection = _rootSection;
    connect(m_rootSection, SIGNAL(commandExecuted()), SLOT(reload()));
}

void SectionPropertiesDock::setSection(Section* _section)
{
    m_currentSection = _section;
    m_wdgSectionProperties.comboBoxLayout->setEnabled(m_currentSection);
    reload();
}

void SectionPropertiesDock::reload()
{
    if(m_currentSection) {
        for(int i = 0; i < m_wdgSectionProperties.comboBoxLayout->count(); ++i) {
            if(m_wdgSectionProperties.comboBoxLayout->itemData(i) == m_currentSection->layout()->id()) {
                bool v = m_wdgSectionProperties.comboBoxLayout->blockSignals(true);
                m_wdgSectionProperties.comboBoxLayout->setCurrentIndex(i);
                m_wdgSectionProperties.comboBoxLayout->blockSignals(v);
                break;
            }
        }
    }
}

void SectionPropertiesDock::layoutChanged(int index)
{
    Q_ASSERT(m_currentSection);
    Q_ASSERT(m_rootSection);

    m_rootSection->addCommand(m_currentSection,
                              new ChangeLayoutCommand(m_currentSection,
                                      m_wdgSectionProperties.comboBoxLayout->itemData(index).toString()));
}
