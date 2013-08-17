/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PAGED_KPAGEDIALOG_P_H
#define PAGED_KPAGEDIALOG_P_H

#include "kpagedialog.h"
#include <QDialogButtonBox>
#include <QLayout>

class KPageDialogPrivate
{
    Q_DECLARE_PUBLIC(KPageDialog)
    protected:
        KPageDialogPrivate(KPageDialog *parent)
            : q_ptr(parent),
              mPageWidget(0),
              mButtonBox(0)
        {
        }

        virtual ~KPageDialogPrivate()
        {
        }

        KPageDialog * const q_ptr;
        KPageWidget *mPageWidget;
        QDialogButtonBox *mButtonBox;

        void init()
        {
            Q_Q(KPageDialog);
            delete q->layout();

            QVBoxLayout *layout = new QVBoxLayout;
            q->setLayout(layout);

            if (mPageWidget) {
                q->connect(mPageWidget, SIGNAL(currentPageChanged(KPageWidgetItem *, KPageWidgetItem *)),
                        q, SIGNAL(currentPageChanged(KPageWidgetItem *, KPageWidgetItem *)));
                q->connect(mPageWidget, SIGNAL(pageRemoved(KPageWidgetItem *)), 
                        q, SIGNAL(pageRemoved(KPageWidgetItem *)));
                layout->addWidget(mPageWidget);
            } else {
                layout->addStretch();
            }

            if (mButtonBox) {
                q->connect(mButtonBox, SIGNAL(accepted()), q, SLOT(accept()));
                q->connect(mButtonBox, SIGNAL(rejected()), q, SLOT(reject()));
                layout->addWidget(mButtonBox);
            }
        }
};

#endif // PAGED_KPAGEDIALOG_P_H
// vim: sw=4 sts=4 et tw=100
