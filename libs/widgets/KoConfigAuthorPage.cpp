/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2012 C. Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoConfigAuthorPage.h"

#include "ui_KoConfigAuthorPage.h"

#include <KoGlobal.h>
#include <KoIcon.h>

#include <KConfigGroup>
#include <KEMailSettings>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KUser>

#include <QComboBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QList>
#include <QStackedWidget>
#include <QString>
#include <QStringList>
#include <QToolButton>

class Q_DECL_HIDDEN KoConfigAuthorPage::Private
{
public:
    QList<Ui::KoConfigAuthorPage *> profileUiList;
    QStackedWidget *stack;
    QComboBox *combo;
    QToolButton *deleteUser;
};

KoConfigAuthorPage::KoConfigAuthorPage()
    : d(new Private)
{
    QGridLayout *layout = new QGridLayout;

    d->combo = new QComboBox;
    layout->addWidget(d->combo, 0, 0);
    QToolButton *newUser = new QToolButton;
    newUser->setIcon(koIcon("list-add-user"));
    newUser->setToolTip(i18n("Add new author profile (starts out as a copy of current)"));
    layout->addWidget(newUser, 0, 1);
    d->deleteUser = new QToolButton;
    d->deleteUser->setIcon(koIcon("list-remove-user"));
    d->deleteUser->setToolTip(i18n("Delete the author profile"));
    layout->addWidget(d->deleteUser, 0, 2);
    QFrame *f = new QFrame;
    f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    layout->addWidget(f, 1, 0);
    d->stack = new QStackedWidget();
    layout->addWidget(d->stack, 2, 0, 1, 3);
    setLayout(layout);

    // Add a default profile
    Ui::KoConfigAuthorPage *aUi = new Ui::KoConfigAuthorPage();
    QWidget *w = new QWidget;
    w->setEnabled(false);
    aUi->setupUi(w);
    d->combo->addItem(i18n("Default Author Profile"));
    d->stack->addWidget(w);
    KUser user(KUser::UseRealUserID);
    aUi->leFullName->setText(user.property(KUser::FullName).toString());
    aUi->lePhoneWork->setText(user.property(KUser::WorkPhone).toString());
    aUi->lePhoneHome->setText(user.property(KUser::HomePhone).toString());
    KEMailSettings eMailSettings;
    aUi->leEmail->setText(eMailSettings.getSetting(KEMailSettings::EmailAddress));
    d->profileUiList.append(aUi);

    // Add all the user defined profiles
    KConfig *config = KoGlobal::calligraConfig();
    KConfigGroup authorGroup(config, "Author");
    QStringList profiles = authorGroup.readEntry("profile-names", QStringList());

    foreach (const QString &profile, profiles) {
        KConfigGroup cgs(&authorGroup, "Author-" + profile);
        aUi = new Ui::KoConfigAuthorPage();
        w = new QWidget;
        aUi->setupUi(w);
        aUi->leFullName->setText(cgs.readEntry("creator"));
        aUi->leInitials->setText(cgs.readEntry("initial"));
        aUi->leTitle->setText(cgs.readEntry("author-title"));
        aUi->leCompany->setText(cgs.readEntry("company"));
        aUi->leEmail->setText(cgs.readEntry("email"));
        aUi->lePhoneWork->setText(cgs.readEntry("telephone-work"));
        aUi->lePhoneHome->setText(cgs.readEntry("telephone"));
        aUi->leFax->setText(cgs.readEntry("fax"));
        aUi->leCountry->setText(cgs.readEntry("country"));
        aUi->lePostal->setText(cgs.readEntry("postal-code"));
        aUi->leCity->setText(cgs.readEntry("city"));
        aUi->leStreet->setText(cgs.readEntry("street"));
        aUi->lePosition->setText(cgs.readEntry("position"));

        d->combo->addItem(profile);
        d->profileUiList.append(aUi);
        d->stack->addWidget(w);
    }

    // Connect slots
    connect(d->combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KoConfigAuthorPage::profileChanged);
    connect(newUser, &QAbstractButton::clicked, this, &KoConfigAuthorPage::addUser);
    connect(d->deleteUser, &QAbstractButton::clicked, this, &KoConfigAuthorPage::deleteUser);
    profileChanged(0);
}

KoConfigAuthorPage::~KoConfigAuthorPage()
{
    delete d;
}

void KoConfigAuthorPage::profileChanged(int i)
{
    d->stack->setCurrentIndex(i);
    d->deleteUser->setEnabled(i != 0);
}

void KoConfigAuthorPage::addUser()
{
    bool ok;
    QString profileName = QInputDialog::getText(this, i18n("Name of Profile"), i18n("Name (not duplicate or blank name):"), QLineEdit::Normal, "", &ok);

    if (!ok) {
        return;
    }

    Ui::KoConfigAuthorPage *curUi = d->profileUiList[d->combo->currentIndex()];
    Ui::KoConfigAuthorPage *aUi = new Ui::KoConfigAuthorPage();
    QWidget *w = new QWidget;
    aUi->setupUi(w);

    aUi->leFullName->setText(curUi->leFullName->text());
    aUi->leInitials->setText(curUi->leInitials->text());
    aUi->leTitle->setText(curUi->leTitle->text());
    aUi->leCompany->setText(curUi->leCompany->text());
    aUi->leEmail->setText(curUi->leEmail->text());
    aUi->lePhoneWork->setText(curUi->lePhoneWork->text());
    aUi->lePhoneHome->setText(curUi->lePhoneHome->text());
    aUi->leFax->setText(curUi->leFax->text());
    aUi->leCountry->setText(curUi->leCountry->text());
    aUi->lePostal->setText(curUi->lePostal->text());
    aUi->leCity->setText(curUi->leCity->text());
    aUi->leStreet->setText(curUi->leStreet->text());
    aUi->lePosition->setText(curUi->lePosition->text());

    int index = d->combo->currentIndex() + 1;
    d->combo->insertItem(index, profileName);
    d->profileUiList.insert(index, aUi);
    d->stack->insertWidget(index, w);
    d->combo->setCurrentIndex(index);
}

void KoConfigAuthorPage::deleteUser()
{
    int index = d->combo->currentIndex();
    QWidget *w = d->stack->currentWidget();

    d->stack->removeWidget(w);
    d->profileUiList.removeAt(index);
    d->combo->removeItem(index);
    delete w;
}

void KoConfigAuthorPage::apply()
{
    KConfig *config = KoGlobal::calligraConfig();
    KConfigGroup authorGroup(config, "Author");
    QStringList profiles;

    for (int i = 1; i < d->profileUiList.size(); i++) {
        KConfigGroup cgs(&authorGroup, "Author-" + d->combo->itemText(i));
        profiles.append(d->combo->itemText(i));
        Ui::KoConfigAuthorPage *aUi = d->profileUiList[i];
        cgs.writeEntry("creator", aUi->leFullName->text());
        cgs.writeEntry("initial", aUi->leInitials->text());
        cgs.writeEntry("author-title", aUi->leTitle->text());
        cgs.writeEntry("company", aUi->leCompany->text());
        cgs.writeEntry("email", aUi->leEmail->text());
        cgs.writeEntry("telephone-work", aUi->lePhoneWork->text());
        cgs.writeEntry("telephone", aUi->lePhoneHome->text());
        cgs.writeEntry("fax", aUi->leFax->text());
        cgs.writeEntry("country", aUi->leCountry->text());
        cgs.writeEntry("postal-code", aUi->lePostal->text());
        cgs.writeEntry("city", aUi->leCity->text());
        cgs.writeEntry("street", aUi->leStreet->text());
        cgs.writeEntry("position", aUi->lePosition->text());

        cgs.sync();
    }
    authorGroup.writeEntry("profile-names", profiles);
    authorGroup.sync();
}
