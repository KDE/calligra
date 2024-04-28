/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoFindToolbar.h"
#include "KoFindToolbar_p.h"

#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QTimer>
#include <QToolButton>

#include <KActionCollection>
#include <KLocalizedString>
#include <kcolorscheme.h>
#include <khistorycombobox.h>
#include <ksqueezedtextlabel.h>

#include <KoIcon.h>

#include "KoFindBase.h"
#include "KoFindOption.h"
#include "KoFindOptionSet.h"

QStringList KoFindToolbar::Private::searchCompletionItems = QStringList();
QStringList KoFindToolbar::Private::replaceCompletionItems = QStringList();

KoFindToolbar::KoFindToolbar(KoFindBase *finder, KActionCollection *ac, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , d(new Private(this))
{
    QGridLayout *layout = new QGridLayout();

    d->finder = finder;
    connect(d->finder, &KoFindBase::matchFound, this, [&]() {
        d->matchFound();
    });
    connect(d->finder, &KoFindBase::noMatchFound, this, [&]() {
        d->noMatchFound();
    });
    connect(d->finder, &KoFindBase::wrapAround, this, [&](bool direction) {
        d->searchWrapped(direction);
    });

    d->textTimeout = new QTimer(this);
    d->textTimeout->setInterval(1000);
    d->textTimeout->setSingleShot(true);
    connect(d->textTimeout, &QTimer::timeout, this, [&]() {
        d->inputTimeout();
    });

    d->closeButton = new QToolButton(this);
    d->closeButton->setAutoRaise(true);
    d->closeButton->setIcon(koIcon("dialog-close"));
    d->closeButton->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(d->closeButton, &QAbstractButton::clicked, this, &QWidget::hide);
    connect(d->closeButton, &QAbstractButton::clicked, d->finder, &KoFindBase::finished);
    connect(d->closeButton, &QAbstractButton::clicked, d->textTimeout, &QTimer::stop);
    layout->addWidget(d->closeButton, 0, 0);

    layout->addWidget(new QLabel(i18nc("Label for the Find text input box", "Find:"), this), 0, 1, Qt::AlignRight);

    d->searchLine = new KHistoryComboBox(true, this);
    d->searchLine->setCompletedItems(d->searchCompletionItems);
    d->searchLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(d->searchLine, &KHistoryComboBox::editTextChanged, this, [&]() {
        d->textTimeout->start();
    });
    connect(d->searchLine, &KHistoryComboBox::returnPressed, this, [&]() {
        d->returnPressed();
    });
    connect(d->searchLine, QOverload<const QString &>::of(&KHistoryComboBox::returnPressed), d->searchLine, &KHistoryComboBox::addToHistory);
    connect(d->searchLine, &KHistoryComboBox::cleared, finder, &KoFindBase::finished);
    layout->addWidget(d->searchLine, 0, 2);

    d->nextButton = new QToolButton(this);
    d->nextButton->setIcon(koIcon("go-down-search"));
    d->nextButton->setText(i18nc("Next search result", "Next"));
    d->nextButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d->nextButton->setEnabled(false);
    connect(d->nextButton, &QAbstractButton::clicked, d->finder, &KoFindBase::findNext);
    connect(d->nextButton, &QToolButton::clicked, this, [&](bool) {
        d->addToHistory();
    });
    connect(d->finder, &KoFindBase::hasMatchesChanged, d->nextButton, &QWidget::setEnabled);
    layout->addWidget(d->nextButton, 0, 3);

    d->previousButton = new QToolButton(this);
    d->previousButton->setIcon(koIcon("go-up-search"));
    d->previousButton->setText(i18nc("Previous search result", "Previous"));
    d->previousButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d->previousButton->setEnabled(false);
    connect(d->previousButton, &QAbstractButton::clicked, d->finder, &KoFindBase::findPrevious);
    connect(d->previousButton, &QToolButton::clicked, this, [&](bool) {
        d->addToHistory();
    });
    connect(d->finder, &KoFindBase::hasMatchesChanged, d->previousButton, &QWidget::setEnabled);
    layout->addWidget(d->previousButton, 0, 4);

    d->optionsButton = new QToolButton(this);
    d->optionsButton->setText(i18nc("Search options", "Options"));

    QMenu *menu = new QMenu(d->optionsButton);

    QList<KoFindOption *> options = finder->options()->options();
    foreach (KoFindOption *option, options) {
        if (option->value().type() == QVariant::Bool) {
            QAction *action = new QAction(option->title(), menu);
            action->setStatusTip(option->description());
            action->setToolTip(option->description());
            action->setObjectName(option->name());
            action->setCheckable(true);
            action->setChecked(option->value().toBool());
            connect(action, &QAction::triggered, this, [&](bool) {
                d->optionChanged();
            });
            menu->addAction(action);
        }
    }

    d->optionsButton->setMenu(menu);
    d->optionsButton->setPopupMode(QToolButton::InstantPopup);
    if (menu->actions().count() == 0) {
        d->optionsButton->setEnabled(false);
    }
    layout->addWidget(d->optionsButton, 0, 5);

    d->information = new KSqueezedTextLabel(this);
    layout->addWidget(d->information, 0, 6);

    d->replaceLabel = new QLabel(i18nc("Label for the Replace text input box", "Replace:"), this);
    layout->addWidget(d->replaceLabel, 1, 1, Qt::AlignRight);

    d->replaceLine = new KHistoryComboBox(true, this);
    d->replaceLine->setHistoryItems(d->replaceCompletionItems);
    d->replaceLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(d->replaceLine, &KHistoryComboBox::returnPressed, this, [&]() {
        d->replace();
    });
    layout->addWidget(d->replaceLine, 1, 2);

    d->replaceButton = new QToolButton(this);
    d->replaceButton->setText(i18nc("Replace the current match", "Replace"));
    d->replaceButton->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    connect(d->replaceButton, &QToolButton::clicked, this, [&]() {
        d->replace();
    });
    layout->addWidget(d->replaceButton, 1, 3);

    d->replaceAllButton = new QToolButton(this);
    d->replaceAllButton->setText(i18nc("Replace all found matches", "Replace All"));
    d->replaceAllButton->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    connect(d->replaceAllButton, &QToolButton::clicked, this, [&]() {
        d->replaceAll();
    });
    layout->addWidget(d->replaceAllButton, 1, 4);

    setLayout(layout);

    ac->addAction(KStandardAction::Find, "edit_find", this, SLOT(activateSearch()));
    QAction *replaceAction = new QAction(i18n("Replace"), this);
    ac->addAction("edit_replace", replaceAction);
    replaceAction->setShortcut(Qt::CTRL + Qt::Key_H);
    connect(replaceAction, &QAction::triggered, this, &KoFindToolbar::activateReplace);

    QAction *findNextAction = ac->addAction(KStandardAction::FindNext, "edit_findnext", d->nextButton, SIGNAL(clicked(bool)));
    connect(finder, &KoFindBase::hasMatchesChanged, findNextAction, &QAction::setEnabled);
    connect(findNextAction, &QAction::triggered, this, &KoFindToolbar::activateSearch);
    findNextAction->setEnabled(false);
    QAction *findPrevAction = ac->addAction(KStandardAction::FindPrev, "edit_findprevious", d->previousButton, SIGNAL(clicked(bool)));
    connect(finder, &KoFindBase::hasMatchesChanged, findPrevAction, &QAction::setEnabled);
    connect(findPrevAction, &QAction::triggered, this, &KoFindToolbar::activateSearch);
    findPrevAction->setEnabled(false);
}

KoFindToolbar::~KoFindToolbar()
{
    delete d;
}

void KoFindToolbar::activateSearch()
{
    d->replaceLabel->setVisible(false);
    d->replaceLine->setVisible(false);
    d->replaceButton->setVisible(false);
    d->replaceAllButton->setVisible(false);

    if (!isVisible()) {
        show();
    }
    d->searchLine->setFocus();

    if (d->finder->matches().size() == 0) {
        d->textTimeout->start();
    }
}

void KoFindToolbar::activateReplace()
{
    if (!isVisible()) {
        show();
    }
    d->searchLine->setFocus();

    d->replaceLabel->setVisible(true);
    d->replaceLine->setVisible(true);
    d->replaceButton->setVisible(true);
    d->replaceAllButton->setVisible(true);

    if (d->finder->matches().size() == 0) {
        d->textTimeout->start();
    }
}

void KoFindToolbar::Private::matchFound()
{
    QPalette current = searchLine->palette();
    KColorScheme::adjustBackground(current, KColorScheme::PositiveBackground);
    searchLine->setPalette(current);
    replaceLine->setPalette(current);

    information->setText(i18ncp("Total number of matches", "1 match found", "%1 matches found", finder->matches().count()));
}

void KoFindToolbar::Private::noMatchFound()
{
    QPalette current = searchLine->palette();
    KColorScheme::adjustBackground(current, KColorScheme::NegativeBackground);
    searchLine->setPalette(current);
    replaceLine->setPalette(current);

    information->setText(i18n("No matches found"));
}

void KoFindToolbar::Private::searchWrapped(bool direction)
{
    if (direction) {
        information->setText(i18n("Search hit bottom, continuing from top."));
    } else {
        information->setText(i18n("Search hit top, continuing from bottom."));
    }
}

void KoFindToolbar::Private::addToHistory()
{
    searchLine->addToHistory(searchLine->currentText());
}

void KoFindToolbar::Private::find(const QString &pattern)
{
    textTimeout->stop();

    if (pattern.length() > 0) {
        finder->find(pattern);
    } else {
        finder->finished();
        information->setText(QString());
        searchLine->setPalette(qApp->palette());
        replaceLine->setPalette(qApp->palette());
    }
}

void KoFindToolbar::Private::optionChanged()
{
    QAction *action = qobject_cast<QAction *>(q->sender());
    if (action) {
        finder->options()->setOptionValue(action->objectName(), action->isChecked());
        find(searchLine->currentText());
    }
}

void KoFindToolbar::Private::replace()
{
    finder->replaceCurrent(replaceLine->currentText());
    replaceLine->addToHistory(replaceLine->currentText());
}

void KoFindToolbar::Private::replaceAll()
{
    finder->replaceAll(replaceLine->currentText());
    replaceLine->addToHistory(replaceLine->currentText());
}

void KoFindToolbar::Private::inputTimeout()
{
    find(searchLine->currentText());
}

void KoFindToolbar::Private::returnPressed()
{
    // in case the timer is active there is a new word so search for the new word
    // otherwise go to the next found match.
    if (textTimeout->isActive()) {
        find(searchLine->currentText());
    } else {
        finder->findNext();
    }
}

// have to include this because of Q_PRIVATE_SLOT
#include "moc_KoFindToolbar.cpp"
