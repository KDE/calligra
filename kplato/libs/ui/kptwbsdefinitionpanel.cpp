/* This file is part of the KDE project
   Copyright (C) 2005-2007 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

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
#include "kptproject.h"

#include <KComboBox>
#include <klocale.h>
#include <kdebug.h>

#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <QTableWidget>
#include <QStringList>
#include <QUndoCommand>

namespace KPlato
{

ComboBoxDelegate::ComboBoxDelegate(QStringList &list, QObject *parent)
    : QStyledItemDelegate(parent)
{
    kDebug();
    setObjectName("ComboBoxDelegate");
    m_list = list;
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    kDebug();
    QComboBox *editor = new KComboBox(parent);
    editor->installEventFilter(const_cast<ComboBoxDelegate*>(this));
    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    kDebug()<<value<<":"<<m_list;
    QComboBox *comboBox = static_cast<QComboBox*>(editor);

    comboBox->insertItems(0, m_list);
    comboBox->setCurrentIndex(comboBox->findText(value));
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    kDebug()<<comboBox->currentText();
    model->setData(index, comboBox->currentText());
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

//----------------------

WBSDefinitionPanel::WBSDefinitionPanel( Project &project, WBSDefinition &def, QWidget *p, const char *n)
    : QWidget(p),
      m_project( project ),
      m_def(def),
      selectedRow(-1)
{
    setObjectName(n);
    setupUi(this);
    
    projectCode->setText( m_def.projectCode() );
    projectSeparator->setText( m_def.projectSeparator() );
    QStringList codeList = m_def.codeList();
    kDebug()<<codeList;
    defaultSeparator->setText(m_def.defaultSeparator());
    defaultCode->addItems(codeList);
    defaultCode->setCurrentIndex(m_def.defaultCodeIndex());
    defaultCode->setFocus();
    levelsTable->setItemDelegateForColumn(0, new ComboBoxDelegate(codeList, this));
    levelsGroup->setChecked(m_def.isLevelsDefEnabled());
    int i = 0;
    const QMap<int, WBSDefinition::CodeDef> &lev = m_def.levelsDef();
    levelsTable->setRowCount(lev.count());
    QStringList sl;
    kDebug()<<"Map size="<<lev.count();
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
    
    connect(projectCode, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
    connect(projectSeparator, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
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

void WBSDefinitionPanel::setStartValues() {
}

QUndoCommand *WBSDefinitionPanel::buildCommand() {
    WBSDefinition def = m_def;
    def.setProjectCode( projectCode->text() );
    def.setProjectSeparator( projectSeparator->text() );
    def.setDefaultCode(defaultCode->currentIndex());
    def.setDefaultSeparator(defaultSeparator->text());
    
    def.setLevelsDefEnabled(levelsGroup->isChecked());

    def.clearLevelsDef();
    for (int i = 0; i < levelsTable->rowCount(); ++i) {
        def.setLevelsDef(levelsTable->verticalHeaderItem(i)->text().toInt(), levelsTable->item(i, 0)->text(), levelsTable->item(i, 1)->text());
    }
    WBSDefinitionModifyCmd *cmd = new WBSDefinitionModifyCmd( m_project, def, i18n("Modify WBS Code Definition"));
    return cmd;
}

bool WBSDefinitionPanel::ok() {
    kDebug();
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
    kDebug()<<s;
}

void WBSDefinitionPanel::slotRemoveBtnClicked() {
    kDebug()<<selectedRow;
    if (selectedRow == -1) {
        return;
    }
    levelsTable->removeRow(selectedRow);
    removeBtn->setEnabled(false);
    slotLevelChanged(level->value());
}

void WBSDefinitionPanel::slotAddBtnClicked() {
    kDebug();
    int i=levelsTable->rowCount()-1;
    for (; i >= 0; --i) {
        kDebug()<<"Checking row["<<i<<"]="<<levelsTable->verticalHeaderItem(i)->text()<<" with"<<level->value();
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
    
    kDebug()<<"Added row="<<i<<" level="<<level->value();
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
    kDebug();
    slotLevelChanged(level->value());
}


}  //KPlato namespace

#include "kptwbsdefinitionpanel.moc"
