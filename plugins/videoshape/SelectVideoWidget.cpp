/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SelectVideoWidget.h"

#include <KLocalizedString>
#include <kfilewidget.h>
#include <phonon/BackendCapabilities>

#include <QCheckBox>
#include <QUrl>
#include <QVBoxLayout>

SelectVideoWidget::SelectVideoWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_fileWidget = new KFileWidget(QUrl(/*QT5TODO:"kfiledialog:///OpenVideoDialog"*/), this);
    m_fileWidget->setOperationMode(KFileWidget::Opening);
    m_fileWidget->setFilters(KFileFilter::fromMimeTypes(Phonon::BackendCapabilities::availableMimeTypes()));
    layout->addWidget(m_fileWidget);

    m_saveEmbedded = new QCheckBox(i18n("Save as part of document"));
    m_fileWidget->setCustomWidget("", m_saveEmbedded);
    setLayout(layout);
}

SelectVideoWidget::~SelectVideoWidget() = default;

void SelectVideoWidget::accept()
{
    m_fileWidget->slotOk();
    m_fileWidget->accept();
}

void SelectVideoWidget::cancel()
{
    m_fileWidget->slotCancel();
}

QUrl SelectVideoWidget::selectedUrl() const
{
    return m_fileWidget->selectedUrl();
}

bool SelectVideoWidget::saveEmbedded()
{
    return m_saveEmbedded->isChecked();
}
