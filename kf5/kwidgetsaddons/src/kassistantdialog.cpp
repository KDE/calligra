/* This file is part of the KDE libraries
    Copyright (C) 2006 Olivier Goffart <ogoffart at kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kassistantdialog.h"

#include <QDialogButtonBox>
#include <QIcon>
#include <QPushButton>
#include <QApplication>

#include <QHash>

class KAssistantDialog::Private
{
    public:
        Private(KAssistantDialog *q)
            : q(q)
        {
        }

        KAssistantDialog *q;
        QHash<KPageWidgetItem*, bool> valid;
        QHash<KPageWidgetItem*, bool> appropriate;
        KPageWidgetModel *pageModel;
        QPushButton *backButton;
        QPushButton *nextButton;
        QPushButton *finishButton;

        void init();
        void _k_slotUpdateButtons();

        QModelIndex getNext(QModelIndex nextIndex)
        {
            QModelIndex currentIndex;
            do {
                currentIndex=nextIndex;
                nextIndex=currentIndex.child(0, 0);
                if (!nextIndex.isValid())
                    nextIndex=currentIndex.sibling(currentIndex.row() + 1, 0);
            } while (nextIndex.isValid() && !appropriate.value(pageModel->item(nextIndex), true));
            return nextIndex;
        }

        QModelIndex getPrevious(QModelIndex nextIndex)
        {
            QModelIndex currentIndex;
            do {
                currentIndex=nextIndex;
                nextIndex=currentIndex.sibling(currentIndex.row() - 1, 0);
                if (!nextIndex.isValid())
                    nextIndex=currentIndex.parent();
            } while (nextIndex.isValid() && !appropriate.value(pageModel->item(nextIndex), true));
            return nextIndex;
        }
};

KAssistantDialog::KAssistantDialog(QWidget * parent, Qt::WindowFlags flags)
    : KPageDialog(parent, flags), d(new Private(this))
{
    d->init();
    //workaround to get the page model
    KPageWidget *pagewidget=findChild<KPageWidget*>();
    Q_ASSERT(pagewidget);
    d->pageModel=static_cast<KPageWidgetModel*>(pagewidget->model());
}

KAssistantDialog::KAssistantDialog(KPageWidget *widget, QWidget *parent, Qt::WindowFlags flags)
    : KPageDialog(widget, parent, flags), d(new Private(this))
{
    d->init();
    d->pageModel=static_cast<KPageWidgetModel*>(widget->model());
}

KAssistantDialog::~KAssistantDialog()
{
    delete d;
}

void KAssistantDialog::Private::init()
{
    QDialogButtonBox *buttonBox = q->buttonBox();

    backButton = new QPushButton;

    const QString iconBack = QApplication::isRightToLeft() ? QStringLiteral("go-next") : QStringLiteral("go-previous");
    const QString iconNext = QApplication::isRightToLeft() ? QStringLiteral("go-previous") : QStringLiteral("go-next");
    backButton->setText(tr("&Back", "go back"));
    backButton->setIcon(QIcon::fromTheme(iconBack));
    backButton->setToolTip(tr("Go back one step" ));
    q->connect(backButton, SIGNAL(clicked()), q, SLOT(back()));
    buttonBox->addButton(backButton, QDialogButtonBox::ActionRole);

    nextButton = new QPushButton;
    nextButton->setText(tr("Next", "Opposite to Back"));
    nextButton->setIcon(QIcon::fromTheme(iconNext));
    nextButton->setDefault(true);
    q->connect(nextButton, SIGNAL(clicked()), q, SLOT(next()));
    buttonBox->addButton(nextButton, QDialogButtonBox::ActionRole);

    finishButton = new QPushButton;
    finishButton->setText(tr("Finish"));
    finishButton->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply")));
    buttonBox->addButton(finishButton, QDialogButtonBox::AcceptRole);

    buttonBox->addButton(QDialogButtonBox::Cancel);

    q->setFaceType(KPageDialog::Plain);

    q->connect(q, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)), q, SLOT(_k_slotUpdateButtons()));
}


void KAssistantDialog::back()
{
    QModelIndex nextIndex=d->getPrevious(d->pageModel->index(currentPage()));
    if (nextIndex.isValid())
        setCurrentPage(d->pageModel->item(nextIndex));
}

void KAssistantDialog::next()
{
    QModelIndex nextIndex=d->getNext(d->pageModel->index(currentPage()));
    if (nextIndex.isValid())
        setCurrentPage(d->pageModel->item(nextIndex));
    else if (isValid(currentPage()))
        accept();
}

void KAssistantDialog::setValid(KPageWidgetItem * page, bool enable)
{
    d->valid[page]=enable;
    if (page == currentPage())
        d->_k_slotUpdateButtons();
}

bool KAssistantDialog::isValid(KPageWidgetItem * page) const
{
    return d->valid.value(page, true);
}

void KAssistantDialog::Private::_k_slotUpdateButtons()
{
    QModelIndex currentIndex=pageModel->index(q->currentPage());
    //change the caption of the next/finish button
    QModelIndex nextIndex=getNext(currentIndex);
    finishButton->setEnabled(!nextIndex.isValid() && q->isValid(q->currentPage()));
    nextButton->setEnabled(nextIndex.isValid() && q->isValid(q->currentPage()));
    finishButton->setDefault(!nextIndex.isValid());
    nextButton->setDefault(nextIndex.isValid());
    //enable or disable the back button;
    nextIndex=getPrevious(currentIndex);
    backButton->setEnabled(nextIndex.isValid());
}

void KAssistantDialog::showEvent(QShowEvent * event)
{
    d->_k_slotUpdateButtons(); //called because last time that function was called is when the first page was added, so the next button show "finish"
    KPageDialog::showEvent(event);
}

void KAssistantDialog::setAppropriate(KPageWidgetItem * page, bool appropriate)
{
    d->appropriate[page]=appropriate;
    d->_k_slotUpdateButtons();
}

bool KAssistantDialog::isAppropriate(KPageWidgetItem * page) const
{
    return d->appropriate.value(page, true);
}

#include "moc_kassistantdialog.cpp"
