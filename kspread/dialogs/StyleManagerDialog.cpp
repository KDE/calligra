/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2003 Laurent Montel <montel@kde.org>
   Copyright 2003 Norbert Andres <nandres@web.de>

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
   Boston, MA 02110-1301, USA.
*/

#include "StyleManagerDialog.h"

#include <QMap>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <kcombobox.h>
#include <kdebug.h>
#include <klocale.h>

#include "Cell.h"
#include "LayoutDialog.h"
#include "ui/Selection.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"

#include "commands/StyleCommand.h"

using namespace KSpread;

StyleManagerDialog::StyleManagerDialog(QWidget* parent, Selection* selection, StyleManager* manager)
        : KDialog(parent)
        , m_selection(selection)
        , m_styleManager(manager)
{
    setButtons(Apply | User1 | User2 | User3 | Close);
    setButtonText(User3, i18n("&New..."));
    setButtonText(User2, i18n("&Modify..."));
    setButtonText(User1, i18n("&Delete..."));
    setButtonsOrientation(Qt::Vertical);
    setCaption(i18n("Style Manager"));

    QWidget* widget = new QWidget(this);
    setMainWidget(widget);

    QVBoxLayout* layout = new QVBoxLayout(widget);

    m_styleList = new QTreeWidget(this);
    m_styleList->setHeaderLabel(i18n("Style"));
    layout->addWidget(m_styleList);

    m_displayBox = new KComboBox(false, this);
    m_displayBox->insertItem(0, i18n("All Styles"));
    m_displayBox->insertItem(1, i18n("Custom Styles"));
    m_displayBox->insertItem(2, i18n("Hierarchical"));
    layout->addWidget(m_displayBox);

    slotDisplayMode(0);
    enableButton(KDialog::User3, true);
    enableButton(KDialog::User2, true);
    enableButton(KDialog::User1, false);

    connect(m_displayBox, SIGNAL(activated(int)),
            this, SLOT(slotDisplayMode(int)));
    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotOk()));
    connect(this, SIGNAL(user3Clicked()),
            this, SLOT(slotNew()));
    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotEdit()));
    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotRemove()));
    connect(m_styleList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
            this, SLOT(slotEdit()));
    connect(m_styleList, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(selectionChanged(QTreeWidgetItem*)));
}

StyleManagerDialog::~StyleManagerDialog()
{
}

void StyleManagerDialog::fillComboBox()
{
    typedef QMap<CustomStyle*, QTreeWidgetItem*> Map;
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

    CustomStyles::iterator iter = m_styleManager->m_styles.begin();
    CustomStyles::iterator end  = m_styleManager->m_styles.end();

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
    kDebug() ;
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
        enableButton(KDialog::User1, false);
        return;
    }

    if (style->type() == Style::BUILTIN)
        enableButton(KDialog::User1, false);
    else
        enableButton(KDialog::User1, true);
}

#include "StyleManagerDialog.moc"
