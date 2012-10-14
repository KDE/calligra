/* This file is part of the KDE project
   Copyright (C) 2004-2012 Jarosław Staniek <staniek@kde.org>

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

#include "KexiNameDialog.h"
#include "KexiNameWidget.h"
#include <core/kexipartinfo.h>
#include <db/connection.h>
#include <kexi_global.h>

#include <KIconLoader>
#include <KMessageBox>
#include <KDebug>

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
    m_checkIfObjectExists = false;
    m_allowOverwriting = false;
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

bool KexiNameDialog::canOverwrite()
{
    KexiDB::SchemaData tmp_sdata;
    tristate result = m_project->dbConnection()->loadObjectSchemaData(
                          m_project->idForClass(m_part->info()->partClass()),
                          widget()->nameText(), tmp_sdata);
    kDebug() << (result == cancelled) << (result == 2) << (result == false) << (result == true) << ~result;
    if (result == cancelled) {
        return true;
    }
    if (result == false) {
        kWarning() << "Cannot load object schema data for" << widget()->nameText();
        return false;
    }
    if (!m_allowOverwriting) {
        KMessageBox::information(this,
                                 "<p>" + m_part->i18nMessage("Object \"%1\" already exists.", 0)
                                             .subs(widget()->nameText()).toString()
                                 + "</p><p>" + i18n("Please choose other name.") + "</p>");
        return false;
    }

    QString msg =
        "<p>" + m_part->i18nMessage("Object \"%1\" already exists.", 0)
                    .subs(widget()->nameText()).toString()
        + "</p><p>" + i18n("Do you want to replace it?") + "</p>";
    KGuiItem yesItem(KStandardGuiItem::yes());
    yesItem.setText(i18n("&Replace"));
    yesItem.setToolTip(i18n("Replace object"));
    int res = KMessageBox::warningYesNo(
                  this, msg, QString(),
                  yesItem, KGuiItem(i18n("&Choose Other Name...")),
                  QString(),
                  KMessageBox::Notify | KMessageBox::Dangerous);
    if (m_overwriteNeeded && res == KMessageBox::Yes) {
        *m_overwriteNeeded = true;
    }
    return res == KMessageBox::Yes;
}

void KexiNameDialog::accept()
{
    if (!m_widget->checkValidity())
        return;

    if (m_checkIfObjectExists && m_project) {
        if (!canOverwrite()) {
            return;
        }
    }

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

KexiNameWidget* KexiNameDialog::widget() const
{
    return m_widget;
}

int KexiNameDialog::execAndCheckIfObjectExists(const KexiProject &project,
                                               const KexiPart::Part &part,
                                               bool *overwriteNeeded)
{
    m_project = &project;
    m_part = &part;
    m_checkIfObjectExists = true;
    m_overwriteNeeded = overwriteNeeded;
    if (m_overwriteNeeded) {
        *m_overwriteNeeded = false;
    }
    int res = exec();
    m_project = 0;
    m_part = 0;
    m_checkIfObjectExists = false;
    m_overwriteNeeded = 0;
    return res;
}

void KexiNameDialog::setAllowOverwriting(bool set)
{
    m_allowOverwriting = set;
}

#include "KexiNameDialog.moc"
