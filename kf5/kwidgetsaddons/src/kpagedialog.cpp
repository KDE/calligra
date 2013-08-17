/*
 *  This file is part of the KDE Libraries
 *  Copyright (C) 1999-2001 Mirko Boehm (mirko@kde.org) and
 *  Espen Sand (espen@kde.org)
 *  Holger Freyther <freyther@kde.org>
 *  2005-2006 Olivier Goffart <ogoffart at kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "kpagedialog.h"
#include "kpagedialog_p.h"

#include <QTimer>
#include <QLayout>

KPageDialog::KPageDialog( QWidget *parent, Qt::WindowFlags flags )
    : QDialog(parent, flags),
      d_ptr(new KPageDialogPrivate(this))
{
    Q_D(KPageDialog);
  d->mPageWidget = new KPageWidget( this );
  d->mButtonBox = new QDialogButtonBox(this);
  d->mButtonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  d->init();
}

KPageDialog::KPageDialog( KPageWidget *widget, QWidget *parent, Qt::WindowFlags flags )
    : QDialog(parent, flags),
      d_ptr(new KPageDialogPrivate(this))
{
    Q_D(KPageDialog);
    Q_ASSERT(widget);
    widget->setParent(this);
  d->mPageWidget = widget;
  d->mButtonBox = new QDialogButtonBox(this);
  d->mButtonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  d->init();
}

KPageDialog::KPageDialog(KPageDialogPrivate &dd, KPageWidget *widget, QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags),
      d_ptr(&dd)
{
    Q_D(KPageDialog);
    if (widget) {
        widget->setParent(this);
        d->mPageWidget = widget;
    } else {
        d->mPageWidget = new KPageWidget(this);
    }
    d->mButtonBox = new QDialogButtonBox(this);
    d->mButtonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    d->init();
}

KPageDialog::~KPageDialog()
{
}

void KPageDialog::setFaceType( FaceType faceType )
{
    d_func()->mPageWidget->setFaceType(static_cast<KPageWidget::FaceType>(faceType));
}

KPageWidgetItem* KPageDialog::addPage( QWidget *widget, const QString &name )
{
    return d_func()->mPageWidget->addPage(widget, name);
}

void KPageDialog::addPage( KPageWidgetItem *item )
{
    d_func()->mPageWidget->addPage(item);
}

KPageWidgetItem* KPageDialog::insertPage( KPageWidgetItem *before, QWidget *widget, const QString &name )
{
    return d_func()->mPageWidget->insertPage(before, widget, name);
}

void KPageDialog::insertPage( KPageWidgetItem *before, KPageWidgetItem *item )
{
    d_func()->mPageWidget->insertPage(before, item);
}

KPageWidgetItem* KPageDialog::addSubPage( KPageWidgetItem *parent, QWidget *widget, const QString &name )
{
    return d_func()->mPageWidget->addSubPage(parent, widget, name);
}

void KPageDialog::addSubPage( KPageWidgetItem *parent, KPageWidgetItem *item )
{
    d_func()->mPageWidget->addSubPage(parent, item);
}

void KPageDialog::removePage( KPageWidgetItem *item )
{
    d_func()->mPageWidget->removePage(item);
}

void KPageDialog::setCurrentPage( KPageWidgetItem *item )
{
    d_func()->mPageWidget->setCurrentPage(item);
}

KPageWidgetItem* KPageDialog::currentPage() const
{
    return d_func()->mPageWidget->currentPage();
}

void KPageDialog::setStandardButtons(QDialogButtonBox::StandardButtons buttons)
{
    d_func()->mButtonBox->setStandardButtons(buttons);
}

QPushButton *KPageDialog::button(QDialogButtonBox::StandardButton which) const
{
    return d_func()->mButtonBox->button(which);
}

KPageWidget* KPageDialog::pageWidget()
{
    return d_func()->mPageWidget;
}

void KPageDialog::setPageWidget(KPageWidget *widget)
{
    delete d_func()->mPageWidget;
    d_func()->mPageWidget = widget;
    d_func()->init();
}

const KPageWidget* KPageDialog::pageWidget() const
{
    return d_func()->mPageWidget;
}

QDialogButtonBox *KPageDialog::buttonBox()
{
    return d_func()->mButtonBox;
}

const QDialogButtonBox *KPageDialog::buttonBox() const
{
    return d_func()->mButtonBox;
}

void KPageDialog::setButtonBox(QDialogButtonBox *box)
{
    delete d_func()->mButtonBox;
    d_func()->mButtonBox = box;
    d_func()->init();
}

