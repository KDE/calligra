/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002 Montel Laurent <lmontel@mandrakesoft.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "LanguageTab.h"
#include "KoGlobal.h"

#include <KoCharacterStyle.h>
#include <KoIcon.h>

#include <QSet>
#include <QStringList>

LanguageTab::LanguageTab(/*KSpell2::Loader::Ptr loader,*/ bool uniqueFormat, QWidget *parent, Qt::WindowFlags fl)
    : QWidget(parent)
    , m_uniqueFormat(uniqueFormat)
{
    widget.setupUi(this);

    Q_UNUSED(fl);

    widget.languageListSearchLine->setListWidget(widget.languageList);

    // TODO use fl
    const QStringList langNames = KoGlobal::listOfLanguages();
    const QStringList langTags = KoGlobal::listOfLanguageTags();
    QSet<QString> spellCheckLanguages;

    widget.languageList->addItem(QString("None"));
#if 0 // Port it
    if (loader)
        spellCheckLanguages = QSet<QString>::fromList(loader->languages());
#endif
    QStringList::ConstIterator itName = langNames.begin();
    QStringList::ConstIterator itTag = langTags.begin();
    for (; itName != langNames.end() && itTag != langTags.end(); ++itName, ++itTag) {
        if (spellCheckLanguages.contains(*itTag)) {
            QListWidgetItem *item = new QListWidgetItem();
            item->setText(*itName);
            item->setIcon(koIcon("tools-check-spelling"));

            widget.languageList->addItem(item);
        } else
            widget.languageList->addItem(*itName);
    }
    connect(widget.languageList, &QListWidget::currentItemChanged, this, &LanguageTab::languageChanged);
}

LanguageTab::~LanguageTab() = default;

void LanguageTab::save(KoCharacterStyle *style) const
{
    if (!widget.languageList->currentItem() || widget.languageList->currentItem()->text() == "None") // TODO i18n
        style->setLanguage(QString());
    else
        style->setLanguage(KoGlobal::tagOfLanguage(widget.languageList->currentItem()->text()));
}

void LanguageTab::setDisplay(KoCharacterStyle *style)
{
    if (m_uniqueFormat) {
        const QString &name = KoGlobal::languageFromTag(style->language());

        QList<QListWidgetItem *> items = widget.languageList->findItems(name, Qt::MatchFixedString);
        if (!items.isEmpty()) {
            widget.languageList->setCurrentItem(items.first());
            widget.languageList->scrollToItem(items.first());
        }
    }
}
