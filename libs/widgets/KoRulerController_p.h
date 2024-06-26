/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KoRulerController_p_h
#define KoRulerController_p_h

#include "KoRulerController.h"
#include "KoText.h"
#include "styles/KoParagraphStyle.h"

#include <KoCanvasResourceManager.h>
#include <KoTextDocument.h>

#include <WidgetsDebug.h>

#include <QLocale>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>
#include <QTextOption>
#include <QVariant>

#include <KoRuler.h>

#include <algorithm>

static int compareTabs(KoText::Tab &tab1, KoText::Tab &tab2)
{
    return tab1.position < tab2.position;
}

class Q_DECL_HIDDEN KoRulerController::Private
{
public:
    Private(KoRuler *r, KoCanvasResourceManager *crp)
        : ruler(r)
        , resourceManager(crp)
        , lastPosition(-1)
        , originalTabIndex(-2)
        , currentTabIndex(-2)
    {
    }

    void canvasResourceChanged(int key)
    {
        if (key != KoText::CurrentTextPosition && key != KoText::CurrentTextDocument && key != KoCanvasResourceManager::ActiveRange)
            return;

        QTextBlock block = currentBlock();
        if (!block.isValid()) {
            ruler->setShowIndents(false);
            ruler->setShowTabs(false);
            return;
        }

        QRectF activeRange = resourceManager->resource(KoCanvasResourceManager::ActiveRange).toRectF();
        ruler->setOverrideActiveRange(activeRange.left(), activeRange.right());
        lastPosition = block.position();
        currentTabIndex = -2;
        tabList.clear();

        QTextBlockFormat format = block.blockFormat();
        ruler->setRightToLeft(block.layout()->textOption().textDirection() == Qt::RightToLeft);
        ruler->setParagraphIndent(format.leftMargin());
        ruler->setFirstLineIndent(format.textIndent());
        ruler->setEndIndent(format.rightMargin());
        ruler->setRelativeTabs(relativeTabs());

        QList<KoRuler::Tab> tabs;
        QVariant variant = format.property(KoParagraphStyle::TabPositions);
        if (!variant.isNull()) {
            foreach (const QVariant &var, qvariant_cast<QList<QVariant>>(variant)) {
                KoText::Tab textTab = var.value<KoText::Tab>();
                KoRuler::Tab tab;
                tab.position = textTab.position;
                tab.type = textTab.type;
                tabs.append(tab);
            }
        }
        qreal tabStopDistance = format.doubleProperty(KoParagraphStyle::TabStopDistance);
        /*        if (tabStopDistance <= 0) {
        // kotextdocumentlayout hardly reachable from here :(
            tabStopDistance = block.document()->documentLayout()->defaultTabSpacing();
                } */
        ruler->updateTabs(tabs, tabStopDistance);

        ruler->setShowIndents(true);
        ruler->setShowTabs(true);
    }

    void indentsChanged()
    {
        QTextBlock block = currentBlock();
        if (!block.isValid())
            return;
        QTextCursor cursor(block);
        QTextBlockFormat bf = cursor.blockFormat();
        bf.setLeftMargin(ruler->paragraphIndent());
        bf.setTextIndent(ruler->firstLineIndent());
        bf.setRightMargin(ruler->endIndent());
        cursor.setBlockFormat(bf);
    }

    void tabChanged(int originalIndex, KoRuler::Tab *tab)
    {
        QVariant docVar = resourceManager->resource(KoText::CurrentTextDocument);
        if (docVar.isNull())
            return;
        QTextDocument *doc = static_cast<QTextDocument *>(docVar.value<void *>());
        if (doc == nullptr)
            return;
        const int position = resourceManager->intResource(KoText::CurrentTextPosition);
        const int anchor = resourceManager->intResource(KoText::CurrentTextAnchor);

        QTextCursor cursor(doc);
        cursor.setPosition(anchor);
        cursor.setPosition(position, QTextCursor::KeepAnchor);

        if (originalTabIndex == -2 || originalTabIndex != originalIndex) {
            originalTabIndex = originalIndex;
            KoParagraphStyle style(cursor.blockFormat(), cursor.blockCharFormat());
            tabList = style.tabPositions();
            if (originalTabIndex >= 0) { // modification
                currentTab = tabList[originalTabIndex];
                currentTabIndex = originalTabIndex;
            } else if (originalTabIndex == -1 && tab) { // new tab.
                currentTab = KoText::Tab();
                currentTab.type = tab->type;
                if (tab->type == QTextOption::DelimiterTab)
                    currentTab.delimiter = QLocale::system().decimalPoint()[0]; // TODO check language of text
                currentTabIndex = tabList.count();
                tabList << currentTab;
            } else {
                warnWidgets << "Unexpected input from tabChanged signal";
                Q_ASSERT(false);
                return;
            }
        }

        if (tab) {
            currentTab.position = tab->position;
            currentTab.type = tab->type;
            if (currentTabIndex == -2) { // add the new tab to the list, sorting in.
                currentTabIndex = tabList.count();
                tabList << currentTab;
            } else
                tabList.replace(currentTabIndex, currentTab);
        } else if (currentTabIndex >= 0) { // lets remove it.
            tabList.removeAt(currentTabIndex);
            currentTabIndex = -2;
        }

        QTextBlockFormat bf;
        QVector<KoText::Tab> sortedList = tabList;
        std::sort(sortedList.begin(), sortedList.end(), compareTabs);
        QList<QVariant> list;
        foreach (const KoText::Tab &tab, sortedList) {
            QVariant v;
            v.setValue(tab);
            list.append(v);
        }
        bf.setProperty(KoParagraphStyle::TabPositions, list);
        cursor.mergeBlockFormat(bf);
    }

    QTextBlock currentBlock()
    {
        QVariant docVar = resourceManager->resource(KoText::CurrentTextDocument);
        if (docVar.isNull())
            return QTextBlock();
        QTextDocument *doc = static_cast<QTextDocument *>(docVar.value<void *>());
        if (doc == nullptr)
            return QTextBlock();
        return doc->findBlock(resourceManager->intResource(KoText::CurrentTextPosition));
    }

    bool relativeTabs()
    {
        QVariant docVar = resourceManager->resource(KoText::CurrentTextDocument);
        if (docVar.isNull())
            return false;
        QTextDocument *doc = static_cast<QTextDocument *>(docVar.value<void *>());
        if (doc == nullptr)
            return false;
        return KoTextDocument(doc).relativeTabs();
    }

    void tabChangeInitiated()
    {
        tabList.clear();
        originalTabIndex = -2;
    }

private:
    KoRuler *ruler;
    KoCanvasResourceManager *resourceManager;
    int lastPosition; // the last position in the text document.
    QVector<KoText::Tab> tabList;
    KoText::Tab currentTab;
    int originalTabIndex, currentTabIndex;
};
#endif
