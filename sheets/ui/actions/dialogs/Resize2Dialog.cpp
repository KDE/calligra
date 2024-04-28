/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Resize2Dialog.h"

// Qt
#include <QGridLayout>
#include <QLabel>

// KF5
#include <KLocalizedString>

// Calligra
#include <KoUnitDoubleSpinBox.h>

using namespace Calligra::Sheets;

ResizeRowDialog::ResizeRowDialog(QWidget *parent, KoUnit unit)
    : KoDialog(parent)
{
    setCaption(i18n("Resize Row"));
    setModal(true);
    setButtons(Ok | Cancel | Default);

    QWidget *page = new QWidget();
    setMainWidget(page);

    QGridLayout *gridLayout = new QGridLayout(page);
    gridLayout->setColumnStretch(1, 1);

    QLabel *label1 = new QLabel(page);
    label1->setText(i18n("Height:"));
    gridLayout->addWidget(label1, 0, 0);

    m_pHeight = new KoUnitDoubleSpinBox(page);
    m_pHeight->setUnit(unit);
    gridLayout->addWidget(m_pHeight, 0, 1);

    m_pHeight->setFocus();

    connect(this, &KoDialog::okClicked, this, &ResizeRowDialog::slotOk);
    connect(this, &KoDialog::defaultClicked, this, &ResizeRowDialog::slotDefault);
}

void ResizeRowDialog::slotOk()
{
    accept();
}

void ResizeRowDialog::setRowHeight(double height)
{
    m_pHeight->changeValue(height);
}

double ResizeRowDialog::rowHeight()
{
    return m_pHeight->value();
}

void ResizeRowDialog::slotDefault()
{
    m_pHeight->changeValue(defaultHeight);
}

ResizeColumnDialog::ResizeColumnDialog(QWidget *parent, KoUnit unit)
    : KoDialog(parent)
{
    setCaption(i18n("Resize Column"));
    setModal(true);
    setButtons(Ok | Cancel | Default);

    QWidget *page = new QWidget();
    setMainWidget(page);

    QGridLayout *gridLayout = new QGridLayout(page);
    gridLayout->setColumnStretch(1, 1);

    QLabel *label1 = new QLabel(page);
    label1->setText(i18n("Width:"));
    gridLayout->addWidget(label1, 0, 0);

    m_pWidth = new KoUnitDoubleSpinBox(page);
    m_pWidth->setUnit(unit);
    gridLayout->addWidget(m_pWidth, 0, 1);

    m_pWidth->setFocus();

    connect(this, &KoDialog::okClicked, this, &ResizeColumnDialog::slotOk);
    connect(this, &KoDialog::defaultClicked, this, &ResizeColumnDialog::slotDefault);
}

void ResizeColumnDialog::setColWidth(double width)
{
    m_pWidth->changeValue(width);
}

double ResizeColumnDialog::colWidth()
{
    return m_pWidth->value();
}

void ResizeColumnDialog::slotDefault()
{
    m_pWidth->changeValue(defaultWidth);
}

void ResizeColumnDialog::slotOk()
{
    accept();
}
