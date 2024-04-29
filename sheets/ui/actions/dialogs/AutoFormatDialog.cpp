/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "AutoFormatDialog.h"

#include <KLocalizedString>

#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>

using namespace Calligra::Sheets;

class AutoFormatDialog::Private
{
public:
    QComboBox *combo;
    QLabel *label;

    QList<QString> names;
    QMap<QString, QPixmap> pixmaps;
};

AutoFormatDialog::AutoFormatDialog(QWidget *parent)
    : ActionDialog(parent)
    , d(new Private())
{
    setCaption(i18n("Automatic Format"));
    setObjectName(QLatin1String("AutoFormatDialog"));

    QWidget *page = mainWidget();

    QVBoxLayout *vbox = new QVBoxLayout(page);

    QLabel *toplabel = new QLabel(i18n("Select the sheet style to apply:"), page);
    d->combo = new QComboBox(page);
    d->label = new QLabel(page);

    vbox->addWidget(toplabel);
    vbox->addWidget(d->combo);
    vbox->addWidget(d->label, 1);

    connect(d->combo, QOverload<int>::of(&QComboBox::activated), this, &AutoFormatDialog::slotActivated);
}

AutoFormatDialog::~AutoFormatDialog()
{
    delete d;
}

void AutoFormatDialog::setList(const QMap<QString, QPixmap> &list)
{
    d->pixmaps = list;
    d->names = list.keys(); // storing this so that indexes retain the same order
    d->names.sort(Qt::CaseInsensitive);

    d->combo->clear();

    for (QString &name : d->names)
        d->combo->addItem(name);

    slotActivated(0);
}

QString AutoFormatDialog::selectedOption()
{
    int idx = d->combo->currentIndex();
    if (idx < 0)
        return QString();
    return d->names.at(idx);
}

void AutoFormatDialog::slotActivated(int index)
{
    if ((index < 0) || (index >= d->names.length()))
        return;

    enableButtonOk(true);

    QString name = d->names[index];
    d->label->setPixmap(d->pixmaps[name]);
}

void AutoFormatDialog::onApply()
{
    int idx = d->combo->currentIndex();
    if (idx < 0)
        return;
    Q_EMIT applyFormat(selectedOption());
}
