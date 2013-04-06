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

#include "KexiAssistantWidget.h"
#include "KexiAssistantPage.h"
#include "KexiAnimatedLayout.h"

#include <kdialog.h>
#include <kdebug.h>

#include <QStyle>

class KexiAssistantWidget::Private
{
public:
    Private(KexiAssistantWidget *qq)
        : q(qq)
    {
    }
    
    ~Private()
    {
    }
    
    KexiAnimatedLayout *lyr;

    void addPage(KexiAssistantPage* page) {
        lyr->addWidget(page);
        connect(page, SIGNAL(back(KexiAssistantPage*)), q, SLOT(previousPageRequested(KexiAssistantPage*)));
        connect(page, SIGNAL(next(KexiAssistantPage*)), q, SLOT(nextPageRequested(KexiAssistantPage*)));
        connect(page, SIGNAL(cancelled(KexiAssistantPage*)), q, SLOT(cancelRequested(KexiAssistantPage*)));
    }

private:
    KexiAssistantWidget* q;
};

// ----

KexiAssistantWidget::KexiAssistantWidget(QWidget* parent)
 : QWidget(parent)
 , d(new Private(this))
{
    QVBoxLayout *mainLyr = new QVBoxLayout(this);
    d->lyr = new KexiAnimatedLayout;
    mainLyr->addLayout(d->lyr);
    int margin = style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, 0)
        + KDialog::marginHint();
    mainLyr->setContentsMargins(margin, margin, margin, margin);
}

KexiAssistantWidget::~KexiAssistantWidget()
{
    delete d;
}

void KexiAssistantWidget::addPage(KexiAssistantPage* page)
{
    d->addPage(page);
}

void KexiAssistantWidget::previousPageRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
}

void KexiAssistantWidget::nextPageRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
}
    
void KexiAssistantWidget::cancelRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
}
    
KexiAssistantPage* KexiAssistantWidget::currentPage() const
{
    return dynamic_cast<KexiAssistantPage*>(d->lyr->currentWidget());
}

void KexiAssistantWidget::setCurrentPage(KexiAssistantPage* page)
{
    d->lyr->setCurrentWidget(page);
    if (page->focusWidget())
        page->focusWidget()->setFocus();
}

#include "KexiAssistantWidget.moc"
