// vi: ts=8 sts=4 sw=4
/* This file is part of the KDE libraries
   Copyright (C) 1998 Pietro Iglio <iglio@fub.it>
   Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
   Copyright (C) 2004,2005 Andrew Coles <andrew_coles@yahoo.co.uk>
   Copyright (C) 2007 Michaël Larouche <larouche@kde.org>
   Copyright (C) 2009 Christoph Feck <christoph@maxiom.de>

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
#include "knewpassworddialog.h"

#include <QApplication>
#include <QProgressBar>
#include <QPushButton>
#include <QRegExp>
#include <QSize>
#include <QString>
#include <QLineEdit>
#include <QMessageBox>

#include <ktitlewidget.h>

#include "ui_knewpassworddialog.h"

class KNewPasswordDialog::KNewPasswordDialogPrivate
{
public:
    KNewPasswordDialogPrivate( KNewPasswordDialog *parent )
        : q( parent ),
         minimumPasswordLength(0), passwordStrengthWarningLevel(1),reasonablePasswordLength(8)
    {}

    void init();
    void _k_textChanged();

    KNewPasswordDialog *q;

    int minimumPasswordLength;
    int passwordStrengthWarningLevel;
    int reasonablePasswordLength;

    int effectivePasswordLength(const QString &password);

    QString pass;

    Ui::KNewPasswordDialog ui;
};


void KNewPasswordDialog::KNewPasswordDialogPrivate::init()
{
    ui.setupUi( q );

    ui.labelIcon->setPixmap( QIcon::fromTheme(QStringLiteral("dialog-password")).pixmap(96, 96) );
    ui.labelMatch->setHidden(true);

    const QString strengthBarWhatsThis(tr("The password strength meter gives an indication of the security "
            "of the password you have entered.  To improve the strength of "
            "the password, try:\n"
            " - using a longer password;\n"
            " - using a mixture of upper- and lower-case letters;\n"
            " - using numbers or symbols, such as #, as well as letters."));
    ui.labelStrengthMeter->setWhatsThis(strengthBarWhatsThis);
    ui.strengthBar->setWhatsThis(strengthBarWhatsThis);

    connect( ui.linePassword, SIGNAL(textChanged(QString)), q, SLOT(_k_textChanged()) );
    connect( ui.lineVerifyPassword, SIGNAL(textChanged(QString)), q, SLOT(_k_textChanged()) );

    _k_textChanged();
}


int KNewPasswordDialog::KNewPasswordDialogPrivate::effectivePasswordLength(const QString &password)
{
    enum Category {
        Digit,
        Upper,
        Vowel,
        Consonant,
        Special
    };

    Category previousCategory = Vowel;
    QString vowels(QStringLiteral("aeiou"));
    int count = 0;

    for (int i = 0; i < password.length(); ++i) {
        QChar currentChar = password.at(i);
        if (!password.left(i).contains(currentChar)) {
            Category currentCategory;
            switch (currentChar.category()) {
                case QChar::Letter_Uppercase:
                    currentCategory = Upper;
                    break;
                case QChar::Letter_Lowercase:
                    if (vowels.contains(currentChar)) {
                        currentCategory = Vowel;
                    } else {
                        currentCategory = Consonant;
                    }
                    break;
                case QChar::Number_DecimalDigit:
                    currentCategory = Digit;
                    break;
                default:
                    currentCategory = Special;
                    break;
            }
            switch (currentCategory) {
                case Vowel:
                    if (previousCategory != Consonant) {
                        ++count;
                    }
                    break;
                case Consonant:
                    if (previousCategory != Vowel) {
                        ++count;
                    }
                    break;
                default:
                    if (previousCategory != currentCategory) {
                        ++count;
                    }
                    break;
            }
            previousCategory = currentCategory;
        }
    }
    return count;
}


void KNewPasswordDialog::KNewPasswordDialogPrivate::_k_textChanged()
{
    const bool match = ui.linePassword->text() == ui.lineVerifyPassword->text();

    const int minPasswordLength = q->minimumPasswordLength();

    if ( ui.linePassword->text().length() < minPasswordLength) {
        ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    } else {
        ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(match);
    }

    if ( match && !q->allowEmptyPasswords() && ui.linePassword->text().isEmpty()) {
        ui.labelMatch->setPixmap( QIcon::fromTheme(QStringLiteral("dialog-error")) );
        ui.labelMatch->setText( tr("Password is empty") );
    }
    else {
        if ( ui.linePassword->text().length() < minPasswordLength ) {
            ui.labelMatch->setPixmap( QIcon::fromTheme(QStringLiteral("dialog-error")) );
            ui.labelMatch->setText(tr("Password must be at least %1 character(s) long").arg(minimumPasswordLength));
        } else {
            ui.labelMatch->setPixmap( match ? QIcon::fromTheme(QStringLiteral("dialog-ok")) : QIcon::fromTheme(QStringLiteral("dialog-error")) );
            ui.labelMatch->setText( match? tr("Passwords match")
                :tr("Passwords do not match") );
        }
    }

    // Password strength calculator
    int pwstrength = (20 * ui.linePassword->text().length() + 80 * effectivePasswordLength(ui.linePassword->text())) / qMax(reasonablePasswordLength, 2);
    if (pwstrength < 0) {
        pwstrength = 0;
    } else if (pwstrength > 100) {
        pwstrength = 100;
    }
    ui.strengthBar->setValue(pwstrength);
}

/*
 * Password dialog.
 */

KNewPasswordDialog::KNewPasswordDialog( QWidget *parent)
    : QDialog(parent), d(new KNewPasswordDialogPrivate(this))
{
    d->init();
}


KNewPasswordDialog::~KNewPasswordDialog()
{
    delete d;
}


void KNewPasswordDialog::setPrompt(const QString &prompt)
{
    d->ui.labelPrompt->setText(prompt);
}


QString KNewPasswordDialog::prompt() const
{
    return d->ui.labelPrompt->text();
}


void KNewPasswordDialog::setPixmap(const QPixmap &pixmap)
{
    d->ui.labelIcon->setPixmap(pixmap);
    d->ui.labelIcon->setFixedSize( d->ui.labelIcon->sizeHint() );
}


QPixmap KNewPasswordDialog::pixmap() const
{
    return *d->ui.labelIcon->pixmap();
}

bool KNewPasswordDialog::checkAndGetPassword(QString *pwd)
{
    pwd->clear();
    if ( d->ui.linePassword->text() != d->ui.lineVerifyPassword->text() ) {
        d->ui.labelMatch->setPixmap( KTitleWidget::ErrorMessage );
        d->ui.labelMatch->setText( tr("You entered two different "
                "passwords. Please try again.") );

        d->ui.linePassword->clear();
        d->ui.lineVerifyPassword->clear();
        return false;
    }

    if (d->ui.strengthBar && d->ui.strengthBar->value() < d->passwordStrengthWarningLevel) {
        QMessageBox::StandardButton selectedButton = QMessageBox::warning(this,
                                            tr("Low Password Strength"),
                                            tr("The password you have entered has a low strength. "
                                                "To improve the strength of "
                                                "the password, try:\n"
                                                " - using a longer password;\n"
                                                " - using a mixture of upper- and lower-case letters;\n"
                                                " - using numbers or symbols as well as letters.\n"
                                                "\n"
                                                "Would you like to use this password anyway?"),
                                            QMessageBox::Yes | QMessageBox::No);

        if (selectedButton == QMessageBox::No) {
            return false;
        }
    }
    if ( !checkPassword(d->ui.linePassword->text()) ) {
        return false;
    }

    *pwd = d->ui.linePassword->text();
    return true;
}

void KNewPasswordDialog::accept()
{
    QString pwd;
    if (!checkAndGetPassword(&pwd)) {
        return;
    }
    d->pass = pwd;
    emit newPassword( d->pass );
    QDialog::accept();
}


void KNewPasswordDialog::setAllowEmptyPasswords(bool allowed)
{
    setMinimumPasswordLength( allowed ? 0 : 1 );
    d->_k_textChanged();
}


bool KNewPasswordDialog::allowEmptyPasswords() const
{
    return d->minimumPasswordLength == 0;
}

void KNewPasswordDialog::setMinimumPasswordLength(int minLength)
{
    d->minimumPasswordLength = minLength;
    d->_k_textChanged();
}

int KNewPasswordDialog::minimumPasswordLength() const
{
    return d->minimumPasswordLength;
}

void KNewPasswordDialog::setMaximumPasswordLength(int maxLength)
{
    d->ui.linePassword->setMaxLength(maxLength);
    d->ui.lineVerifyPassword->setMaxLength(maxLength);
}

int KNewPasswordDialog::maximumPasswordLength() const
{
    return d->ui.linePassword->maxLength();
}

// reasonable password length code contributed by Steffen Mthing

void KNewPasswordDialog::setReasonablePasswordLength(int reasonableLength)
{

    if (reasonableLength < 1) {
        reasonableLength = 1;
    }
    if (reasonableLength >= maximumPasswordLength()) {
        reasonableLength = maximumPasswordLength();
    }

    d->reasonablePasswordLength = reasonableLength;

}

int KNewPasswordDialog::reasonablePasswordLength() const
{
    return d->reasonablePasswordLength;
}


void KNewPasswordDialog::setPasswordStrengthWarningLevel(int warningLevel)
{
    if (warningLevel < 0) {
        warningLevel = 0;
    }
    if (warningLevel > 99) {
        warningLevel = 99;
    }
    d->passwordStrengthWarningLevel = warningLevel;
}

int KNewPasswordDialog::passwordStrengthWarningLevel() const
{
    return d->passwordStrengthWarningLevel;
}

QString KNewPasswordDialog::password() const
{
    return d->pass;
}

bool KNewPasswordDialog::checkPassword(const QString &)
{
    return true;
}

#include "moc_knewpassworddialog.cpp"

// kate: space-indent on; indent-width 4; encoding utf-8; replace-tabs on;
