/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kptwbsdefinitionpanel.h"
#include "kptwbsdefinition.h"
#include "kptcommand.h"
#include "kptpart.h"

#include <klocale.h>
#include <kdebug.h>

#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <q3groupbox.h>
#include <q3memarray.h>
#include <QPushButton>
#include <qstringlist.h>
#include <QSpinBox>
#include <q3table.h>

namespace KPlato
{

WBSDefinitionPanel::WBSDefinitionPanel(WBSDefinition &def, QWidget *p, const char *n)
    : WBSDefinitionPanelBase(p, n),
      m_def(def)
{
    removeBtn->setEnabled(false);
    
    QStringList codeList = def.codeList();
    defaultSeparator->setText(def.defaultSeparator());
    defaultCode->insertStringList(codeList);
    defaultCode->setCurrentItem(def.defaultCodeIndex());
    defaultCode->setFocus();
    
    levelsGroup->setChecked(def.isLevelsDefEnabled());
    int i = 0;
    const QMap<int, WBSDefinition::CodeDef> &lev = def.levelsDef();
    levelsTable->setNumRows(lev.count());
    kDebug()<<"Map size="<<lev.count()<<endl;
    QMap<int, WBSDefinition::CodeDef>::const_iterator it;
    for (it = lev.begin(); it != lev.end(); ++it) {
        levelsTable->verticalHeader()->setLabel(i, QString("%1").arg(it.key()));
        Q3ComboTableItem *item = new Q3ComboTableItem(levelsTable, codeList, true);
        item->setCurrentItem(it.value().code);
        levelsTable->setItem(i, 0, item);
        levelsTable->setText(i, 1, it.value().separator);
        i++;
    }
    levelsTable->setColumnStretchable(0, true);
    slotLevelChanged(level->value());
    
    connect(defaultCode, SIGNAL(activated(int)), SLOT(slotChanged()));
    connect(defaultSeparator, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
    connect(levelsGroup, SIGNAL(toggled(bool)), SLOT(slotLevelsGroupToggled(bool)));
    connect(levelsTable, SIGNAL(valueChanged(int, int)), SLOT(slotChanged()));
    connect(levelsTable, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
    connect(level, SIGNAL(valueChanged(int)), SLOT(slotLevelChanged(int)));
    connect(removeBtn, SIGNAL(clicked()), SLOT(slotRemoveBtnClicked()));
    connect(addBtn, SIGNAL(clicked()), SLOT(slotAddBtnClicked()));
}

void WBSDefinitionPanel::setStartValues(Part */*part*/) {
}

KMacroCommand *WBSDefinitionPanel::buildCommand(Part */*part*/) {
    KMacroCommand *cmd = new KMacroCommand(i18n("Modify WBS Definition"));

    return cmd;
}

bool WBSDefinitionPanel::ok() {
    m_def.setDefaultCode(defaultCode->currentItem());
    m_def.setDefaultSeparator(defaultSeparator->text());
    
    m_def.setLevelsDefEnabled(levelsGroup->isChecked());

    m_def.clearLevelsDef();
    for (int i = 0; i < levelsTable->numRows(); ++i) {
        m_def.setLevelsDef(levelsTable->verticalHeader()->label(i).toInt(), levelsTable->text(i, 0), levelsTable->text(i, 1));
    }
    return true;
}

void WBSDefinitionPanel::slotChanged() {
    emit changed(true);
}

void WBSDefinitionPanel::slotSelectionChanged() {
    QString s;
    bool rowSelected = false;
    for (int i=0; i < levelsTable->numRows(); ++i) {
        if (levelsTable->isRowSelected(i, true)) {
            s += QString("Row[%1]=selected ").arg(i);
            rowSelected = true;
        }
    }
    removeBtn->setEnabled(rowSelected);
    if (s.isEmpty()) s = "None selected";
    kDebug()<<k_funcinfo<<s<<endl;
}

void WBSDefinitionPanel::slotRemoveBtnClicked() {
    Q3MemArray<int> rows;
    for (int i=0; i < levelsTable->numRows(); ++i) {
        if (levelsTable->isRowSelected(i)) {
            rows.resize(rows.size()+1);
            rows[rows.size()-1] = i;
        }
    }
    levelsTable->removeRows(rows);
    removeBtn->setEnabled(false);
    slotLevelChanged(level->value());
}

void WBSDefinitionPanel::slotAddBtnClicked() {
    kDebug()<<k_funcinfo<<endl;
    int i=levelsTable->numRows()-1;
    for (; i >= 0; --i) {
        kDebug()<<k_funcinfo<<"Checking row["<<i<<"]="<<levelsTable->verticalHeader()->label(i)<<" with "<<level->value()<<endl;
        if (level->value() > levelsTable->verticalHeader()->label(i).toInt()) {
            break;
        }
    }
    i++;
    levelsTable->insertRows(i);
    levelsTable->verticalHeader()->setLabel(i, QString("%1").arg(level->value()));
    Q3ComboTableItem *item = new Q3ComboTableItem(levelsTable, m_def.codeList(), true);
    levelsTable->setItem(i, 0, item);
    levelsTable->clearSelection();
    levelsTable->selectCells(i, 0, i, 0);
    levelsTable->setCurrentCell(i, 0);
    addBtn->setEnabled(false);
    slotChanged();
    
    kDebug()<<k_funcinfo<<"Added row="<<i<<" level="<<level->value()<<endl;
}

void WBSDefinitionPanel::slotLevelChanged(int value) {
    for (int i=0; i < levelsTable->numRows(); ++i) {
        if (value == levelsTable->verticalHeader()->label(i).toInt()) {
            addBtn->setEnabled(false);
            return;
        }
    }
    addBtn->setEnabled(levelsGroup->isChecked());
    slotChanged();
}
void WBSDefinitionPanel::slotLevelsGroupToggled(bool /*on*/) {
    slotLevelChanged(level->value());
}


}  //KPlato namespace

#include "kptwbsdefinitionpanel.moc"
