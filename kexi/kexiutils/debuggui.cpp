/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "utils.h"
#include "utils_p.h"
#include <KexiIcon.h>

#include <KDbUtils>

#include <kguiitem.h>

#include <QThread>
#include <QHeaderView>
#include <QPushButton>
#include <QTabWidget>
#include <QDebug>

static DebugWindow* debugWindow = 0;
static QTabWidget* debugWindowTab = 0;
static KexiDBDebugTreeWidget* kexiDBDebugPage = 0;
static QTreeWidget* kexiAlterTableActionDebugPage = 0;

static void addKexiDBDebug(const QString& text)
{
    // (this is internal code - do not use i18n() here)
    if (!debugWindowTab)
        return;
    if (QThread::currentThread() != debugWindowTab->thread()) {
//! @todo send debug using async. signal
        qWarning() << "Debugging from different thread not supported.";
        return;
    }
    if (!kexiDBDebugPage) {
        QWidget *page = new QWidget(debugWindowTab);
        QVBoxLayout *vbox = new QVBoxLayout(page);
        QHBoxLayout *hbox = new QHBoxLayout(page);
        vbox->addLayout(hbox);
        hbox->addStretch(1);
        QPushButton *btn_copy = new QPushButton(page);
        btn_copy->setIcon(koSmallIcon("edit-copy"));
        hbox->addWidget(btn_copy);
        QPushButton *btn_clear = new QPushButton(KGuiItem("Clear", koIconName("edit-clear-locationbar-rtl")), page);
        hbox->addWidget(btn_clear);

        kexiDBDebugPage = new KexiDBDebugTreeWidget(page);
        kexiDBDebugPage->setObjectName("kexiDbDebugPage");
        kexiDBDebugPage->setFont(KexiUtils::smallFont(kexiDBDebugPage));
        QObject::connect(btn_copy, SIGNAL(clicked()), kexiDBDebugPage, SLOT(copy()));
        QObject::connect(btn_clear, SIGNAL(clicked()), kexiDBDebugPage, SLOT(clear()));
        vbox->addWidget(kexiDBDebugPage);
        kexiDBDebugPage->setHeaderLabel(QString());
        kexiDBDebugPage->header()->hide();
        kexiDBDebugPage->setSortingEnabled(false);
        kexiDBDebugPage->setAllColumnsShowFocus(true);
        kexiDBDebugPage->header()->setResizeMode(0, QHeaderView::Stretch);
        kexiDBDebugPage->header()->setStretchLastSection(true);
        kexiDBDebugPage->setRootIsDecorated(true);
        kexiDBDebugPage->setWordWrap(true);
        kexiDBDebugPage->setAlternatingRowColors(true);
        debugWindowTab->addTab(page, "KexiDB");
        debugWindowTab->setCurrentWidget(page);
        kexiDBDebugPage->show();
    }
    //add \n after (about) every 30 characters
    QTreeWidgetItem * lastItem = kexiDBDebugPage->invisibleRootItem()->child(
        kexiDBDebugPage->invisibleRootItem()->childCount()-1);
    QTreeWidgetItem* li;
    if (lastItem) {
        li = new QTreeWidgetItem(kexiDBDebugPage, lastItem);
    }
    else {
        li = new QTreeWidgetItem(kexiDBDebugPage->invisibleRootItem());
    }
    li->setText(0, text);
    li->setToolTip(0, text);
    li->setExpanded(true);
}

static void addAlterTableActionDebug(const QString& text, int nestingLevel)
{
    // (this is internal code - do not use i18n() here)
    if (!debugWindowTab)
        return;
    if (!kexiAlterTableActionDebugPage) {
        QWidget *page = new QWidget(debugWindowTab);
        QVBoxLayout *vbox = new QVBoxLayout(page);
        QHBoxLayout *hbox = new QHBoxLayout(page);
        vbox->addLayout(hbox);
        hbox->addStretch(1);
        QPushButton *btn_exec = new QPushButton(KGuiItem("Real Alter Table", koIconName("document-save")), page);
        btn_exec->setObjectName("executeRealAlterTable");
        hbox->addWidget(btn_exec);
        QPushButton *btn_clear = new QPushButton(KGuiItem("Clear", koIconName("edit-clear-locationbar-rtl")), page);
        hbox->addWidget(btn_clear);
        QPushButton *btn_sim = new QPushButton(KGuiItem("Simulate Execution", koIconName("system-run")), page);
        btn_sim->setObjectName("simulateAlterTableExecution");
        hbox->addWidget(btn_sim);

        kexiAlterTableActionDebugPage = new QTreeWidget(page);
        kexiAlterTableActionDebugPage->setFont(KexiUtils::smallFont(kexiAlterTableActionDebugPage));
        kexiAlterTableActionDebugPage->setObjectName("kexiAlterTableActionDebugPage");
        QObject::connect(btn_clear, SIGNAL(clicked()), kexiAlterTableActionDebugPage, SLOT(clear()));
        vbox->addWidget(kexiAlterTableActionDebugPage);
        kexiAlterTableActionDebugPage->setHeaderLabel(QString());
        kexiAlterTableActionDebugPage->header()->hide();
        kexiAlterTableActionDebugPage->setSortingEnabled(false);
        kexiAlterTableActionDebugPage->setAllColumnsShowFocus(true);
        kexiAlterTableActionDebugPage->header()->setResizeMode(0, QHeaderView::Stretch);
        kexiAlterTableActionDebugPage->setRootIsDecorated(true);
        debugWindowTab->addTab(page, "AlterTable Actions");
        debugWindowTab->setCurrentWidget(page);
        page->show();
    }
    if (text.isEmpty()) //don't move up!
        return;
    QTreeWidgetItem * li;
    int availableNestingLevels = 0;
    // compute availableNestingLevels
    QTreeWidgetItem * lastItem = kexiAlterTableActionDebugPage->invisibleRootItem()->child(
        kexiAlterTableActionDebugPage->invisibleRootItem()->childCount()-1);
    //qDebug() << "lastItem: " << (lastItem ? lastItem->text(0) : QString());
    while (lastItem) {
        lastItem = lastItem->parent();
        availableNestingLevels++;
    }
    //qDebug() << "availableNestingLevels: " << availableNestingLevels;
    //go up (availableNestingLevels-levelsToGoUp) levels
    lastItem = kexiAlterTableActionDebugPage->invisibleRootItem()->child(
        kexiAlterTableActionDebugPage->invisibleRootItem()->childCount()-1);
    int levelsToGoUp = availableNestingLevels - nestingLevel;
    while (levelsToGoUp > 0 && lastItem) {
        lastItem = lastItem->parent();
        levelsToGoUp--;
    }
    //qDebug() << "lastItem2: " << (lastItem ? lastItem->text(0) : QString());
    if (lastItem) {
        if (lastItem->childCount() > 0) {
               li = new QTreeWidgetItem(lastItem, lastItem->child(lastItem->childCount()-1));   //child, after
        }
        else {
               li = new QTreeWidgetItem(lastItem);   //1st child
        }
    } else {
        lastItem = kexiAlterTableActionDebugPage->invisibleRootItem()->child(
            kexiAlterTableActionDebugPage->invisibleRootItem()->childCount()-1);
        while (lastItem && lastItem->parent()) {
            lastItem = lastItem->parent();
        }
        //qDebug() << "lastItem2: " << (lastItem ? lastItem->text(0) : QString());
        if (lastItem && lastItem->parent())
             li = new QTreeWidgetItem(lastItem->parent(), lastItem);   //after
        else if (!lastItem)
             li = new QTreeWidgetItem(kexiAlterTableActionDebugPage->invisibleRootItem());
        else if (!lastItem->parent())
             li = new QTreeWidgetItem(kexiAlterTableActionDebugPage->invisibleRootItem(), lastItem);
    }
        li->setText(0, text);
        li->setExpanded(true);
}

QWidget *KexiUtils::createDebugWindow(QWidget *parent)
{
    Q_UNUSED(parent);
    KexiDB::setDebugGUIHandler(addKexiDBDebug);
    KexiDB::setAlterTableActionDebugHandler(addAlterTableActionDebug);

    // (this is internal code - do not use i18n() here)
    debugWindow = new DebugWindow(parent);
    QBoxLayout *lyr = new QVBoxLayout(debugWindow);
    debugWindowTab = new QTabWidget(debugWindow);
    debugWindowTab->setObjectName("debugWindowTab");
    lyr->addWidget(debugWindowTab);
    debugWindow->resize(900, 600);
    debugWindow->setWindowIcon(koIcon("document-properties"));
    debugWindow->setWindowTitle("Kexi Internal Debugger");
    debugWindow->show();
    return debugWindow;
}

void KexiUtils::connectPushButtonActionForDebugWindow(const char* actionName,
        const QObject *receiver, const char* slot)
{
    if (debugWindow) {
        QPushButton* btn = KexiUtils::findFirstChild<QPushButton*>(
                               debugWindow, "QPushButton", actionName);
        if (btn)
            QObject::connect(btn, SIGNAL(clicked()), receiver, slot);
    }
}
