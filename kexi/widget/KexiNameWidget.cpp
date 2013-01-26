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

#include "KexiNameWidget.h"

#include <QLabel>
#include <QGridLayout>

#include <klineedit.h>
#include <kmessagebox.h>
#include <klocale.h>

#include <kexiutils/validator.h>
#include <kexiutils/identifier.h>
#include <core/kexi.h>

using namespace KexiUtils;

class KexiNameWidget::Private
{
public:

    Private() {}

    QLabel* lbl_message;
    QLabel* lbl_caption;
    QLabel* lbl_name;
    KLineEdit* le_caption;
    KLineEdit* le_name;
    QGridLayout* lyr;
    KexiUtils::MultiValidator *validator;
    QString nameWarning, captionWarning;
    QString originalNameText;

    bool le_name_txtchanged_disable;
    bool le_name_autofill;
    bool caption_required;
};

KexiNameWidget::KexiNameWidget(const QString& message, QWidget* parent)
        : QWidget(parent)
        , d(new Private)
{
    setObjectName("KexiNameWidget");
    init(message, QString(), QString(), QString(), QString());
}

KexiNameWidget::KexiNameWidget(const QString& message,
                               const QString& nameLabel, const QString& nameText,
                               const QString& captionLabel, const QString& captionText,
                               QWidget * parent)
        : QWidget(parent)
        , d(new Private)
{
    setObjectName("KexiNameWidget");
    init(message, nameLabel, nameText, captionLabel, captionText);
}

void KexiNameWidget::init(
    const QString& message,
    const QString& nameLabel, const QString& nameText,
    const QString& captionLabel, const QString& captionText)
{
    Q_UNUSED(captionText);

    d->le_name_txtchanged_disable = false;
    d->le_name_autofill = true;
    d->caption_required = false;

    d->lyr = new QGridLayout(this);
    d->lyr->setObjectName("lyr");

    d->lbl_message = new QLabel(this);
    d->lbl_message->setObjectName("message");
    setMessageText(message);
    d->lbl_message->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    d->lbl_message->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    d->lbl_message->setWordWrap(true);
    d->lbl_message->setTextInteractionFlags(Qt::TextBrowserInteraction);
    d->lyr->addWidget(d->lbl_message, 0, 0, 1, 2);

    d->lbl_caption = new QLabel(captionLabel.isEmpty() ? i18n("Caption:") : captionLabel,
                             this);
    d->lbl_caption->setObjectName("lbl_caption");
    d->lyr->addWidget(d->lbl_caption, 1, 0);

    d->lbl_name = new QLabel(nameLabel.isEmpty() ? i18n("Name:") : nameLabel,
                          this);
    d->lbl_name->setObjectName("lbl_name");
    d->lyr->addWidget(d->lbl_name, 2, 0);

    d->le_caption = new KLineEdit(this);
    d->le_caption->setObjectName("le_caption");
    setCaptionText(nameText);
    QSizePolicy le_captionSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    le_captionSizePolicy.setHorizontalStretch(1);
    d->le_caption->setSizePolicy(le_captionSizePolicy);
    d->lyr->addWidget(d->le_caption, 1, 1);

    d->le_name = new KLineEdit(this);
    d->le_name->setObjectName("le_name");
    setNameText(nameText);
    QSizePolicy le_nameSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    le_captionSizePolicy.setHorizontalStretch(1);
    d->le_name->setSizePolicy(le_captionSizePolicy);
    IdentifierValidator *idValidator = new IdentifierValidator(0);
    idValidator->setLowerCaseForced(true);
    d->le_name->setValidator(d->validator = new MultiValidator(idValidator, this));
    d->lyr->addWidget(d->le_name, 2, 1);

    setFocusProxy(d->le_caption);
    resize(QSize(342, 123).expandedTo(minimumSizeHint()));

    d->nameWarning = i18n("Please enter the name.");
    d->captionWarning = i18n("Please enter the caption.");

    connect(d->le_caption, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotCaptionTxtChanged(const QString&)));
    connect(d->le_name, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotNameTxtChanged(const QString&)));
    connect(d->le_caption, SIGNAL(returnPressed()),
            this, SIGNAL(returnPressed()));
    connect(d->le_name, SIGNAL(returnPressed()),
            this, SIGNAL(returnPressed()));
}

KexiNameWidget::~KexiNameWidget()
{
    delete d;
}

QLabel* KexiNameWidget::captionLabel() const
{
    return d->lbl_caption;
}

QLabel* KexiNameWidget::nameLabel() const
{
    return d->lbl_name;
}

KLineEdit* KexiNameWidget::captionLineEdit() const
{
    return d->le_caption;
}

KLineEdit* KexiNameWidget::nameLineEdit() const
{
    return d->le_name;
}

QLabel* KexiNameWidget::messageLabel() const
{
    return d->lbl_message;
}

QString KexiNameWidget::messageText() const
{
    return d->lbl_message->text();
}



void KexiNameWidget::slotCaptionTxtChanged(const QString &capt)
{
    emit textChanged();
    if (d->le_name->text().isEmpty())
        d->le_name_autofill = true;
    if (d->le_name_autofill) {
        d->le_name_txtchanged_disable = true;
        d->le_name->setText(string2Identifier(capt).toLower());
        d->le_name_txtchanged_disable = false;
    }
}

void KexiNameWidget::slotNameTxtChanged(const QString &)
{
    emit textChanged();
    if (d->le_name_txtchanged_disable)
        return;
    d->le_name_autofill = false;
}

void KexiNameWidget::clear()
{
    d->le_name->clear();
    d->le_caption->clear();
}

bool KexiNameWidget::empty() const
{
    return d->le_name->text().isEmpty() || d->le_caption->text().trimmed().isEmpty();
}

void KexiNameWidget::setNameRequired(bool set)
{
    d->validator->setAcceptsEmptyValue(!set);
}

bool KexiNameWidget::isCaptionRequired() const {
    return d->caption_required;
}

void KexiNameWidget::setCaptionRequired(bool set) {
    d->caption_required = set;
}


bool KexiNameWidget::isNameRequired() const
{
    return !d->validator->acceptsEmptyValue();
}

void KexiNameWidget::setCaptionText(const QString& capt)
{
    d->le_caption->setText(capt);
    d->le_name_autofill = true;
}

void KexiNameWidget::setNameText(const QString& name)
{
    d->le_name->setText(name);
    d->originalNameText = name;
    d->le_name_autofill = true;
}

void KexiNameWidget::setWarningForName(const QString& txt)
{
    d->nameWarning = txt;
}

void KexiNameWidget::setWarningForCaption(const QString& txt)
{
    d->captionWarning = txt;
}


void KexiNameWidget::setMessageText(const QString& msg)
{
    if (msg.trimmed().isEmpty()) {
        d->lbl_message->setText(QString());
        d->lbl_message->hide();
    } else {
        d->lbl_message->setText(msg.trimmed() + "<br>");
        d->lbl_message->show();
    }
    messageChanged();
}

QString KexiNameWidget::captionText() const
{
    return d->le_caption->text().trimmed();
}

QString KexiNameWidget::nameText() const
{
    return d->le_name->text().trimmed();
}

QString KexiNameWidget::originalNameText() const
{
    return d->originalNameText;
}


bool KexiNameWidget::checkValidity()
{
    if (isNameRequired() && d->le_name->text().trimmed().isEmpty()) {
        KMessageBox::sorry(0, d->nameWarning);
        d->le_name->setFocus();
        return false;
    }
    if (isCaptionRequired() && d->le_caption->text().trimmed().isEmpty()) {
        KMessageBox::sorry(0, d->captionWarning);
        d->le_caption->setFocus();
        return false;
    }
    QString dummy, message, details;
    if (d->validator->check(dummy, d->le_name->text(), message, details)
            == Validator::Error) {
        KMessageBox::detailedSorry(0, message, details);
        d->le_name->setFocus();
        return false;
    }
    return true;
}

Validator *KexiNameWidget::nameValidator() const
{
    return d->validator;
}

void KexiNameWidget::addNameSubvalidator(Validator* validator, bool owned)
{
    d->validator->addSubvalidator(validator, owned);
}

#include "KexiNameWidget.moc"
