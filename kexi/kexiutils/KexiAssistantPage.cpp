/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#include "KexiAssistantPage.h"

#include "utils.h"
#include "KexiTitleLabel.h"
#include "KexiLinkWidget.h"
#include "KexiLinkButton.h"

#include <KAcceleratorManager>
#include <KStandardGuiItem>
#include <KLocale>

#include <QGridLayout>
#include <QPointer>
#include <QEvent>

#include <KDebug>

class KexiAssistantPage::Private {
public:    
    Private(KexiAssistantPage* q_) : q(q_), backButton(0), nextButton(0)
    {
    }
    void setButtonVisible(KexiLinkWidget** button, bool back, bool set,
                          int x, int y);
    QColor linkColor() const;
    KexiAssistantPage * const q;
    QGridLayout* mainLyr;
    QLabel* descriptionLabel;
    KexiLinkWidget* backButton;
    KexiLinkWidget* nextButton;
    KexiLinkButton* cancelButton;
    QPointer<QWidget> focusWidget;
};

void KexiAssistantPage::Private::setButtonVisible(KexiLinkWidget** button,
                                                  bool back, /* or next */
                                                  bool set, int x, int y)
{
    if (set) {
        if (*button) {
            (*button)->show();
        }
        else {
            QString text;
            if (back) {
                *button = new KexiLinkWidget(
                    QLatin1String("KexiAssistantPage:back"),
                    KStandardGuiItem::back().plainText(), q);
                (*button)->setFormat(
                    i18nc("Back button arrow: back button in assistant (wizard)", "‹ %L"));
            }
            else {
                *button = new KexiLinkWidget(
                    QLatin1String("KexiAssistantPage:next"),
                    i18nc("Button text: Next page in assistant (wizard)", "Next"), q);
                (*button)->setFormat(
                    i18nc("Next button arrow: next button in assistant (wizard)", "%L ›"));
            }
            int space = (*button)->fontMetrics().height() / 2;
            Qt::Alignment align;
            if (back) {
                (*button)->setContentsMargins(0, 0, space, 0);
                align = Qt::AlignTop | Qt::AlignLeft;
            }
            else {
                (*button)->setContentsMargins(space, 0, 0, 0);
                align = Qt::AlignTop | Qt::AlignRight;
            }
            KAcceleratorManager::setNoAccel(*button);
            mainLyr->addWidget(*button, x, y, align);
            connect(*button, SIGNAL(linkActivated(QString)),
                    q, SLOT(slotLinkActivated(QString)));
        }
    }
    else {
        if (*button)
            (*button)->hide();
    }
}

// ----

KexiAssistantPage::KexiAssistantPage(const QString& title, const QString& description, QWidget* parent)
 : QWidget(parent)
 , d(new Private(this))
{
/*0         [titleLabel]       [cancel]
  1  [back] [descriptionLabel]   [next]
  2         [contents]                 */
    d->mainLyr = new QGridLayout(this);
    d->mainLyr->setContentsMargins(0, 0, 0, 0);
    d->mainLyr->setColumnStretch(1, 1);
    d->mainLyr->setRowStretch(2, 1);
    KexiTitleLabel* titleLabel = new KexiTitleLabel(title);
    d->mainLyr->addWidget(titleLabel, 0, 1, Qt::AlignTop);
    d->descriptionLabel = new QLabel(description);
    int space = d->descriptionLabel->fontMetrics().height();
    d->descriptionLabel->setContentsMargins(2, 0, 0, space);
    d->descriptionLabel->setWordWrap(true);
    d->mainLyr->addWidget(d->descriptionLabel, 1, 1, Qt::AlignTop);
    
    d->cancelButton = new KexiLinkButton(KIcon("close"));
    d->cancelButton->setToolTip(KStandardGuiItem::cancel().plainText());
    d->cancelButton->setUsesForegroundColor(true);
    connect(d->cancelButton, SIGNAL(clicked()), this, SLOT(slotCancel()));
    d->mainLyr->addWidget(d->cancelButton, 0, 2, Qt::AlignTop|Qt::AlignRight);
}

KexiAssistantPage::~KexiAssistantPage()
{
    delete d;
}

void KexiAssistantPage::setDescription(const QString& text)
{
    d->descriptionLabel->setText(text);
}

void KexiAssistantPage::setBackButtonVisible(bool set)
{
    d->setButtonVisible(&d->backButton, true/*back*/, set, 1, 0);
}

void KexiAssistantPage::setNextButtonVisible(bool set)
{
    d->setButtonVisible(&d->nextButton, false/*next*/, set, 1, 2);
}

void KexiAssistantPage::setContents(QWidget* widget)
{
    widget->setContentsMargins(0, 0, 0, 0);
    d->mainLyr->addWidget(widget, 2, 1);
}

void KexiAssistantPage::setContents(QLayout* layout)
{
    layout->setContentsMargins(0, 0, 0, 0);
    d->mainLyr->addLayout(layout, 2, 1);
}

void KexiAssistantPage::slotLinkActivated(const QString& link)
{
    if (d->backButton && link == d->backButton->link()) {
        back();
    }
    else if (d->nextButton && link == d->nextButton->link()) {
        next();
    }
}

void KexiAssistantPage::slotCancel()
{
    emit cancelled(this);
    if (parentWidget()) {
        parentWidget()->deleteLater();
    }
}

KexiLinkWidget* KexiAssistantPage::backButton()
{
    if (!d->backButton) {
        setBackButtonVisible(true);
        d->backButton->hide();
    }
    return d->backButton;
}

KexiLinkWidget* KexiAssistantPage::nextButton()
{
    if (!d->nextButton) {
        setNextButtonVisible(true);
        d->nextButton->hide();
    }
    return d->nextButton;
}

void KexiAssistantPage::back()
{
    emit back(this);
}

void KexiAssistantPage::next()
{
    emit next(this);
}

QWidget* KexiAssistantPage::focusWidget() const
{
    return d->focusWidget;
}

void KexiAssistantPage::setFocusWidget(QWidget* widget)
{
    d->focusWidget = widget;
}

#include "KexiAssistantPage.moc"
