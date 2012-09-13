/* This file is part of the KDE project
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexinamedialog.h"
#include <KIconLoader>
#include <QGridLayout>
#include <QLabel>

KexiNameDialog::KexiNameDialog(
    const QString& message, QWidget * parent)
        : KDialog(parent)
{
    setMainWidget(new QWidget(this));
    m_widget = new KexiNameWidget(message, mainWidget());
    init();
}

KexiNameDialog::KexiNameDialog(const QString& message,
                               const QString& nameLabel, const QString& nameText,
                               const QString& captionLabel, const QString& captionText,
                               QWidget * parent)
        : KDialog(parent)
{
    setMainWidget(new QWidget(this));
    m_widget = new KexiNameWidget(message, nameLabel, nameText,
                                  captionLabel, captionText, mainWidget());
    init();
}

KexiNameDialog::~KexiNameDialog()
{
}

void KexiNameDialog::init()
{
    setButtons(Ok | Cancel | Help);
    QGridLayout *lyr = new QGridLayout(mainWidget());
    m_icon = new QLabel(mainWidget());
    m_icon->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    QSizePolicy sp(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sp.setHorizontalStretch(1);
    m_icon->setSizePolicy(sp);
    m_icon->setFixedWidth(50);
    lyr->addWidget(m_icon, 0, 0);

    sp = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sp.setHorizontalStretch(1);
    m_widget->setSizePolicy(sp);
    lyr->addWidget(m_widget, 0, 1);
    lyr->addItem(new QSpacerItem(25, 10, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2);
    lyr->addItem(new QSpacerItem(5, 10, QSizePolicy::Minimum, QSizePolicy::Expanding), 1, 1);
// m_widget->captionLineEdit()->selectAll();
// m_widget->captionLineEdit()->setFocus();
    connect(m_widget, SIGNAL(messageChanged()), this, SLOT(updateSize()));
    updateSize();
    enableButtonOk(true);
    slotTextChanged();
    connect(m_widget, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
}

void KexiNameDialog::updateSize()
{
// resize( QSize(400, 140 + (m_widget->lbl_message->isVisible()?m_widget->lbl_message->height():0) )
    resize(QSize(400, 140 + (!m_widget->lbl_message->text().isEmpty() ? m_widget->lbl_message->height() : 0))
           .expandedTo(minimumSizeHint()));
// updateGeometry();
}

void KexiNameDialog::slotTextChanged()
{
    bool enable = true;
    if (   (m_widget->isNameRequired() && m_widget->nameText().isEmpty())
        || (m_widget->isCaptionRequired() && m_widget->captionText().isEmpty()) )
    {
        enable = false;
    }
    enableButtonOk(enable);
}

void KexiNameDialog::accept()
{
    if (!m_widget->checkValidity())
        return;
    KDialog::accept();
}

void KexiNameDialog::setDialogIcon(const QString &iconName)
{
    m_icon->setPixmap(DesktopIcon(iconName, KIconLoader::SizeMedium));
}

void KexiNameDialog::showEvent(QShowEvent * event)
{
    m_widget->captionLineEdit()->selectAll();
    m_widget->captionLineEdit()->setFocus();
    KDialog::showEvent(event);
}

#include "kexinamedialog.moc"
