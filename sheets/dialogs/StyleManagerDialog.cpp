/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2003 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "StyleManagerDialog.h"

#include <QMap>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <kcombobox.h>
#include "SheetsDebug.h"
#include <KLocalizedString>

#include "Cell.h"
#include "LayoutDialog.h"
#include "ui/Selection.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"

#include "commands/StyleCommand.h"

using namespace Calligra::Sheets;

StyleManagerDialog::StyleManagerDialog(QWidget* parent, Selection* selection, StyleManager* manager)
        : KoDialog(parent)
        , m_selection(selection)
        , m_styleManager(manager)
{
    setButtons(Apply | Close);
    setCaption(i18n("Style Manager"));

    QWidget* widget = new QWidget(this);
    setMainWidget(widget);

    QHBoxLayout *hboxLayout = new QHBoxLayout(widget);
    hboxLayout->setMargin(0);

    QVBoxLayout* layout = new QVBoxLayout();

    m_styleList = new QTreeWidget(this);
    m_styleList->setHeaderLabel(i18n("Style"));
    layout->addWidget(m_styleList);

    m_displayBox = new KComboBox(false, this);
    m_displayBox->insertItem(0, i18n("All Styles"));
    m_displayBox->insertItem(1, i18n("Custom Styles"));
    m_displayBox->insertItem(2, i18n("Hierarchical"));
    layout->addWidget(m_displayBox);

    hboxLayout->addLayout(layout);

    // list buttons
    QVBoxLayout *listButtonLayout = new QVBoxLayout();

    m_newButton = new QPushButton(i18n("&New..."), this);
    listButtonLayout->addWidget(m_newButton);
    m_modifyButton = new QPushButton(i18n("&Modify..."), this);
    listButtonLayout->addWidget(m_modifyButton);
    m_deleteButton = new QPushButton(i18n("&Delete..."), this);
    listButtonLayout->addWidget(m_deleteButton);
    listButtonLayout->addStretch(1);

    hboxLayout->addLayout(listButtonLayout);

    slotDisplayMode(0);
    m_newButton->setEnabled(true);
    m_modifyButton->setEnabled(true);
    m_deleteButton->setEnabled(false);

    connect(m_displayBox, QOverload<int>::of(&KComboBox::activated),
            this, &StyleManagerDialog::slotDisplayMode);
    connect(this, &KoDialog::applyClicked,
            this, &StyleManagerDialog::slotOk);
    connect(m_newButton, &QAbstractButton::clicked,
            this, &StyleManagerDialog::slotNew);
    connect(m_modifyButton, &QAbstractButton::clicked,
            this, &StyleManagerDialog::slotEdit);
    connect(m_deleteButton, &QAbstractButton::clicked,
            this, &StyleManagerDialog::slotRemove);
    connect(m_styleList, &QTreeWidget::itemDoubleClicked,
            this, &StyleManagerDialog::slotEdit);
    connect(m_styleList, &QTreeWidget::currentItemChanged,
            this, &StyleManagerDialog::selectionChanged);
}

StyleManagerDialog::~StyleManagerDialog()
{
}

void StyleManagerDialog::fillComboBox()
{
    typedef QHash<CustomStyle*, QTreeWidgetItem*> Map;
    Map entries;

    entries.clear();
    entries[m_styleManager->defaultStyle()] = new QTreeWidgetItem(m_styleList, QStringList(i18n("Default")));

    CustomStyles::const_iterator iter = m_styleManager->m_styles.constBegin();
    CustomStyles::const_iterator end  = m_styleManager->m_styles.constEnd();

    while (entries.count() != m_styleManager->m_styles.count() + 1) {
        if (entries.find(iter.value()) == entries.end()) {
            if (iter.value()->parentName().isNull())
                entries[iter.value()] = new QTreeWidgetItem(entries[m_styleManager->defaultStyle()],
                        QStringList(iter.value()->name()));
            else {
                CustomStyle* parentStyle = m_styleManager->style(iter.value()->parentName());
                if (parentStyle) {
                    Map::const_iterator i = entries.constFind(parentStyle);
                    if (i != entries.constEnd())
                        entries[iter.value()] = new QTreeWidgetItem(i.value(), QStringList(iter.value()->name()));
                }
            }
        }

        ++iter;
        if (iter == end)
            iter = m_styleManager->m_styles.constBegin();
    }
    entries.clear();
}

void StyleManagerDialog::slotDisplayMode(int mode)
{
    m_styleList->clear();

    if (mode != 2) // NOT "Hierarchical"
        m_styleList->setRootIsDecorated(false);
    else { // "Hierarchical"
        m_styleList->setRootIsDecorated(true);
        fillComboBox();
        return;
    }

    if (mode != 1) // NOT "Custom Styles"
        new QTreeWidgetItem(m_styleList, QStringList(i18n("Default")));

    CustomStyles::ConstIterator iter = m_styleManager->m_styles.constBegin();
    CustomStyles::ConstIterator end  = m_styleManager->m_styles.constEnd();

    while (iter != end) {
        CustomStyle* styleData = iter.value();
        if (!styleData || styleData->name().isEmpty()) {
            ++iter;
            continue;
        }

        if (mode == 1) { // "Custom Styles"
            if (styleData->type() == Style::CUSTOM)
                new QTreeWidgetItem(m_styleList, QStringList(styleData->name()));
        } else
            new QTreeWidgetItem(m_styleList, QStringList(styleData->name()));

        ++iter;
    }
}

void StyleManagerDialog::slotOk()
{
    debugSheets ;
    QTreeWidgetItem* item = m_styleList->currentItem();

    if (!item) {
        accept();
        return;
    }

    QString name(item->text(0));
    if (name == i18n("Default")) {
        StyleCommand* command = new StyleCommand();
        command->setSheet(m_selection->activeSheet());
        command->setDefault();
        command->add(*m_selection);
        command->execute(m_selection->canvas());
    } else {
        StyleCommand* command = new StyleCommand();
        command->setSheet(m_selection->activeSheet());
        command->setParentName(name);
        command->add(*m_selection);
        command->execute(m_selection->canvas());
    }
    accept();
}

void StyleManagerDialog::slotNew()
{
    CustomStyle* parentStyle = 0;
    QTreeWidgetItem* item = m_styleList->currentItem();
    if (item) {
        const QString name = item->text(0);
        if (name == i18n("Default"))
            parentStyle = m_styleManager->defaultStyle();
        else
            parentStyle = m_styleManager->style(name);
    } else
        parentStyle = m_styleManager->defaultStyle();

    int i = 1;
    QString newName(i18n("style%1" , m_styleManager->count() + i));
    while (m_styleManager->style(newName) != 0) {
        ++i;
        newName = i18n("style%1" , m_styleManager->count() + i);
    }

    CustomStyle* style = new CustomStyle(newName, parentStyle);
    style->setType(Style::TENTATIVE);

    QPointer<CellFormatDialog> dialog = new CellFormatDialog(this, m_selection, style, m_styleManager);
    dialog->exec();
    delete dialog;

    if (style->type() == Style::TENTATIVE) {
        delete style;
        return;
    }

    m_styleManager->m_styles[ style->name()] = style;

    slotDisplayMode(m_displayBox->currentIndex());
}

void StyleManagerDialog::slotEdit()
{
    QTreeWidgetItem* item = m_styleList->currentItem();

    if (!item)
        return;

    CustomStyle* style = 0;

    QString name(item->text(0));
    if (name == i18n("Default"))
        style = m_styleManager->defaultStyle();
    else
        style = m_styleManager->style(name);

    if (!style)
        return;

    QPointer<CellFormatDialog> dialog = new CellFormatDialog(this, m_selection, style, m_styleManager);
    dialog->exec();

    if (dialog->result() == Accepted)
        m_selection->emitRefreshSheetViews();

    slotDisplayMode(m_displayBox->currentIndex());
    delete dialog;
}

void StyleManagerDialog::slotRemove()
{
    QTreeWidgetItem* item = m_styleList->currentItem();
    if (!item)
        return;

    const QString name = item->text(0);
    CustomStyle* style = 0;
    if (name == i18n("Default"))
        style = m_styleManager->defaultStyle();
    else
        style = m_styleManager->style(name);

    if (!style)
        return;

    if (style->type() != Style::CUSTOM)
        return;

    m_styleManager->takeStyle(style);
    slotDisplayMode(m_displayBox->currentIndex());
}

void StyleManagerDialog::selectionChanged(QTreeWidgetItem* item)
{
    if (!item)
        return;
    const QString name = item->text(0);
    CustomStyle* style = 0;
    if (name == i18n("Default"))
        style = m_styleManager->defaultStyle();
    else
        style = m_styleManager->style(name);
    if (!style) {
        m_deleteButton->setEnabled(false);
        return;
    }

    m_deleteButton->setEnabled(style->type() != Style::BUILTIN);
}
