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
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QStringList>

namespace KPlato
{

ComboBoxDelegate::ComboBoxDelegate(QStringList &list, QObject *parent)
    : QItemDelegate(parent)
{
    kDebug()<<k_funcinfo<<endl;
    setObjectName("ComboBoxDelegate");
    m_list = list;
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    kDebug()<<k_funcinfo<<endl;
    QComboBox *editor = new QComboBox(parent);
    editor->installEventFilter(const_cast<ComboBoxDelegate*>(this));
    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    kDebug()<<k_funcinfo<<value<<": "<<m_list<<endl;
    QComboBox *comboBox = static_cast<QComboBox*>(editor);

    comboBox->insertItems(0, m_list);
    comboBox->setCurrentIndex(comboBox->findText(value));
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    kDebug()<<k_funcinfo<<comboBox->currentText()<<endl;
    model->setData(index, comboBox->currentText());
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

//----------------------

WBSDefinitionPanel::WBSDefinitionPanel(WBSDefinition &def, QWidget *p, const char *n)
    : QWidget(p),
      selectedRow(-1),
      m_def(def)
{
    setObjectName(n);
    setupUi(this);
    
    QStringList codeList = def.codeList();
    kDebug()<<k_funcinfo<<codeList<<endl;
    defaultSeparator->setText(def.defaultSeparator());
    defaultCode->addItems(codeList);
    defaultCode->setCurrentIndex(def.defaultCodeIndex());
    defaultCode->setFocus();
    levelsTable->setItemDelegateForColumn(0, new ComboBoxDelegate(codeList, this));
    levelsGroup->setChecked(def.isLevelsDefEnabled());
    int i = 0;
    const QMap<int, WBSDefinition::CodeDef> &lev = def.levelsDef();
    levelsTable->setRowCount(lev.count());
    QStringList sl;
    kDebug()<<"Map size="<<lev.count()<<endl;
    QMap<int, WBSDefinition::CodeDef>::const_iterator it;
    for (it = lev.begin(); it != lev.end(); ++it) {
        sl << QString("%1").arg(it.key());
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, it.value().code);
        levelsTable->setItem(i, 0, item);
        item = new QTableWidgetItem();
        item->setText(it.value().separator);
        levelsTable->setItem(i, 1, item);
        i++;
    }
    levelsTable->setVerticalHeaderLabels(sl);
    //levelsTable->setColumnStretchable(0, true);
    slotLevelChanged(level->value());
    
    connect(defaultCode, SIGNAL(activated(int)), SLOT(slotChanged()));
    connect(defaultSeparator, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
    connect(levelsGroup, SIGNAL(toggled(bool)), SLOT(slotLevelsGroupToggled(bool)));
    connect(levelsTable, SIGNAL(cellChanged(int, int)), SLOT(slotChanged()));
    connect(levelsTable, SIGNAL(itemSelectionChanged()), SLOT(slotSelectionChanged()));
    connect(level, SIGNAL(valueChanged(int)), SLOT(slotLevelChanged(int)));
    connect(removeBtn, SIGNAL(clicked(bool)), SLOT(slotRemoveBtnClicked()));
    connect(addBtn, SIGNAL(clicked(bool)), SLOT(slotAddBtnClicked()));

    removeBtn->setEnabled(false);
}

void WBSDefinitionPanel::setStartValues(Part */*part*/) {
}

KMacroCommand *WBSDefinitionPanel::buildCommand(Part */*part*/) {
    KMacroCommand *cmd = new KMacroCommand(i18n("Modify WBS Definition"));

    return cmd;
}

bool WBSDefinitionPanel::ok() {
    kDebug()<<k_funcinfo<<endl;
    m_def.setDefaultCode(defaultCode->currentIndex());
    m_def.setDefaultSeparator(defaultSeparator->text());
    
    m_def.setLevelsDefEnabled(levelsGroup->isChecked());

    m_def.clearLevelsDef();
    for (int i = 0; i < levelsTable->rowCount(); ++i) {
        m_def.setLevelsDef(levelsTable->verticalHeaderItem(i)->text().toInt(), levelsTable->item(i, 0)->text(), levelsTable->item(i, 1)->text());
    }
    return true;
}

void WBSDefinitionPanel::slotChanged() {
    emit changed(true);
}

void WBSDefinitionPanel::slotSelectionChanged() {
    QString s;
    selectedRow = -1;
    QList<QTableWidgetItem *> items = levelsTable->selectedItems();
    if (items.count() == 2 && items[0]->row() == items[1]->row()) {
        selectedRow = items[0]->row();
        s = QString("Row[%1]=selected ").arg(selectedRow);
    } else {
        s = "None selected";
    }
    removeBtn->setEnabled(selectedRow != -1);
    kDebug()<<k_funcinfo<<s<<endl;
}

void WBSDefinitionPanel::slotRemoveBtnClicked() {
    kDebug()<<k_funcinfo<<selectedRow<<endl;
    if (selectedRow == -1) {
        return;
    }
    levelsTable->removeRow(selectedRow);
    removeBtn->setEnabled(false);
    slotLevelChanged(level->value());
}

void WBSDefinitionPanel::slotAddBtnClicked() {
    kDebug()<<k_funcinfo<<endl;
    int i=levelsTable->rowCount()-1;
    for (; i >= 0; --i) {
        kDebug()<<k_funcinfo<<"Checking row["<<i<<"]="<<levelsTable->verticalHeaderItem(i)->text()<<" with "<<level->value()<<endl;
        if (level->value() > levelsTable->verticalHeaderItem(i)->text().toInt()) {
            break;
        }
    }
    i++;
    levelsTable->insertRow(i);
    levelsTable->setVerticalHeaderItem(i, new QTableWidgetItem(QString("%1").arg(level->value())));
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, (m_def.codeList())[m_def.defaultCodeIndex()]);
    levelsTable->setItem(i, 0, item);
    item = new QTableWidgetItem();
    item->setText(m_def.defaultSeparator());
    levelsTable->setItem(i, 1, item);
    
    addBtn->setEnabled(false);
    slotChanged();
    
    kDebug()<<k_funcinfo<<"Added row="<<i<<" level="<<level->value()<<endl;
}

void WBSDefinitionPanel::slotLevelChanged(int value) {
    for (int i=0; i < levelsTable->rowCount(); ++i) {
        if (value == levelsTable->verticalHeaderItem(i)->text().toInt()) {
            addBtn->setEnabled(false);
            return;
        }
    }
    addBtn->setEnabled(levelsGroup->isChecked());
    slotChanged();
}
void WBSDefinitionPanel::slotLevelsGroupToggled(bool /*on*/) {
    kDebug()<<k_funcinfo<<endl;
    slotLevelChanged(level->value());
}


}  //KPlato namespace

#include "kptwbsdefinitionpanel.moc"
