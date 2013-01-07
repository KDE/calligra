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

class KexiNameDialog::Private
{

public:
    Private() {}

    QLabel *icon;
    KexiNameWidget* widget;
    const KexiProject *project;
    const KexiPart::Part *part;
    bool checkIfObjectExists;
    bool allowOverwriting;
    bool *overwriteNeeded;
};

KexiNameDialog::KexiNameDialog(
    const QString& message, QWidget * parent)
        : KDialog(parent)
        , d(new Private)
{
    setMainWidget(new QWidget(this));
    d->widget = new KexiNameWidget(message, mainWidget());
    init();
}

KexiNameDialog::KexiNameDialog(const QString& message,
                               const QString& nameLabel, const QString& nameText,
                               const QString& captionLabel, const QString& captionText,
                               QWidget * parent)
        : KDialog(parent)
        , d(new Private)
{
    setMainWidget(new QWidget(this));
    d->widget = new KexiNameWidget(message, nameLabel, nameText,
                                  captionLabel, captionText, mainWidget());
    init();
}

KexiNameDialog::~KexiNameDialog()
{
    delete d;
}

void KexiNameDialog::init()
{
    d->checkIfObjectExists = false;
    d->allowOverwriting = false;
    setButtons(Ok | Cancel | Help);
    QGridLayout *lyr = new QGridLayout(mainWidget());
    d->icon = new QLabel(mainWidget());
    d->icon->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    QSizePolicy sp(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sp.setHorizontalStretch(1);
    d->icon->setSizePolicy(sp);
    d->icon->setFixedWidth(50);
    lyr->addWidget(d->icon, 0, 0);

    sp = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sp.setHorizontalStretch(1);
    d->widget->setSizePolicy(sp);
    lyr->addWidget(d->widget, 0, 1);
    lyr->addItem(new QSpacerItem(25, 10, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2);
    lyr->addItem(new QSpacerItem(5, 10, QSizePolicy::Minimum, QSizePolicy::Expanding), 1, 1);
// d->widget->captionLineEdit()->selectAll();
// d->widget->captionLineEdit()->setFocus();
    connect(d->widget, SIGNAL(messageChanged()), this, SLOT(updateSize()));
    updateSize();
    enableButtonOk(true);
    slotTextChanged();
    connect(d->widget, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
}

void KexiNameDialog::updateSize()
{
// resize( QSize(400, 140 + (d->widget->lbl_message->isVisible()?d->widget->lbl_message->height():0) )
  resize(QSize(400, 140 + (!d->widget->messageLabel()->text().isEmpty() ?
                           d->widget->messageLabel()->height() : 0))
           .expandedTo(minimumSizeHint()));
// updateGeometry();
}

void KexiNameDialog::slotTextChanged()
{
    bool enable = true;
    if (   (d->widget->isNameRequired() && d->widget->nameText().isEmpty())
        || (d->widget->isCaptionRequired() && d->widget->captionText().isEmpty()) )
    {
        enable = false;
    }
    enableButtonOk(enable);
}

bool KexiNameDialog::canOverwrite()
{
    KexiDB::SchemaData tmp_sdata;
    tristate result = d->project->dbConnection()->loadObjectSchemaData(
                          d->project->idForClass(d->part->info()->partClass()),
                          widget()->nameText(), tmp_sdata);
    if (result == cancelled) {
        return true;
    }
    if (result == false) {
        kWarning() << "Cannot load object schema data for" << widget()->nameText();
        return false;
    }
    if (widget()->originalNameText() == tmp_sdata.name()) {
        return true;
    }
    if (!d->allowOverwriting) {
        KMessageBox::information(this,
                                 "<p>" + d->part->i18nMessage("Object \"%1\" already exists.", 0)
                                             .subs(widget()->nameText()).toString()
                                 + "</p><p>" + i18n("Please choose other name.") + "</p>");
        return false;
    }

    QString msg =
        "<p>" + d->part->i18nMessage("Object \"%1\" already exists.", 0)
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
    if (d->overwriteNeeded && res == KMessageBox::Yes) {
        *d->overwriteNeeded = true;
    }
    return res == KMessageBox::Yes;
}

void KexiNameDialog::accept()
{
    if (!d->widget->checkValidity())
        return;

    if (d->checkIfObjectExists && d->project) {
        if (!canOverwrite()) {
            return;
        }
    }

    KDialog::accept();
}

void KexiNameDialog::setDialogIcon(const QString &iconName)
{
    d->icon->setPixmap(DesktopIcon(iconName, KIconLoader::SizeMedium));
}

void KexiNameDialog::showEvent(QShowEvent * event)
{
    d->widget->captionLineEdit()->selectAll();
    d->widget->captionLineEdit()->setFocus();
    KDialog::showEvent(event);
}

KexiNameWidget* KexiNameDialog::widget() const
{
    return d->widget;
}

int KexiNameDialog::execAndCheckIfObjectExists(const KexiProject &project,
                                               const KexiPart::Part &part,
                                               bool *overwriteNeeded)
{
    d->project = &project;
    d->part = &part;
    d->checkIfObjectExists = true;
    d->overwriteNeeded = overwriteNeeded;
    if (d->overwriteNeeded) {
        *d->overwriteNeeded = false;
    }
    int res = exec();
    d->project = 0;
    d->part = 0;
    d->checkIfObjectExists = false;
    d->overwriteNeeded = 0;
    return res;
}

void KexiNameDialog::setAllowOverwriting(bool set)
{
    d->allowOverwriting = set;
}

#include "KexiNameDialog.moc"
