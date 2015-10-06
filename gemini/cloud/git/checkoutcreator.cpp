/* This file is part of the KDE project
 * Copyright 2014  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "checkoutcreator.h"

#include <kfiledialog.h>
#include <kpassworddialog.h>
#include <kmessagebox.h>
#include <kuser.h>
#include <kemailsettings.h>

#include <QInputDialog>
#include <QDir>
#include <QFileDialog>
#include <QDebug>

#include <qgit2.h>
#include <qgit2/qgitglobal.h>

class CheckoutCreator::Private
{
public:
    Private()
        : needsPrivateKeyPassphrase(false)
        , signature(0)
    {}
    ~Private()
    {
        git_signature_free(signature);
    }
    QString privateKey;
    QString publicKey;
    QString userForRemote;
    bool needsPrivateKeyPassphrase;

    QString userName;
    QString userEmail;
    git_signature* signature;

    QString getPassword()
    {
        if(!needsPrivateKeyPassphrase)
            return QString();
        KPasswordDialog dlg;
        dlg.setCaption("Private Key Passphrase");
        dlg.setPrompt("Your private key file requires a password. Please enter it here. You will be asked again each time it is accessed, and the password is not stored.");
        dlg.exec();
        return dlg.password();
    }

    bool checkUserDetails()
    {
        git_config* config;
        git_config_open_default(&config);
        const char* name;
        git_config_get_string(&name, config, "user.name");
        const char* email;
        git_config_get_string(&email, config, "user.email");

        userName = QString::fromLocal8Bit(name);
        userEmail = QString::fromLocal8Bit(email);

        if(userName.isEmpty()) {
            bool ok;
            KUser user(KUser::UseRealUserID);
            QString systemName = user.property(KUser::FullName).toString();
            QString newName = QInputDialog::getText(0,
                                                    i18n("Enter Name"),
                                                    i18n("There is no name set for Git on this system (this is used when committing). Please enter one below and press OK."),
                                                    QLineEdit::Normal,
                                                    systemName,
                                                    &ok);
            if(!ok) {
                return false;
            }
            userName = newName;
            git_config_set_string(config, "user.name", newName.toLocal8Bit());
        }
        if(userEmail.isEmpty()) {
            bool ok;
            KEMailSettings eMailSettings;
            QString emailAddress = eMailSettings.getSetting(KEMailSettings::EmailAddress);
            QString newEmail = QInputDialog::getText(0,
                                                     i18n("Enter Email"),
                                                     i18n("There is no email address set for Git on this system (this is used when committing). Please enter one below and press OK."),
                                                     QLineEdit::Normal,
                                                     emailAddress,
                                                     &ok);
            if(!ok) {
                return false;
            }
            userEmail = newEmail;
            git_config_set_string(config, "user.email", newEmail.toLocal8Bit());
        }

        git_config_free(config);

        if(userName.isEmpty() || userEmail.isEmpty()) {
            return false;
        }
        git_signature_now(&signature, userName.toLocal8Bit(), userEmail.toLocal8Bit());
        return true;
    }
};

CheckoutCreator::CheckoutCreator(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
    LibQGit2::initLibQGit2();
}

CheckoutCreator::~CheckoutCreator()
{
    LibQGit2::shutdownLibQGit2();
    delete d;
}

QString CheckoutCreator::getFile(QString caption, QString filter, QString extraSubDir) const
{
    QUrl searchDir;
    if(QDir::home().exists(extraSubDir))
        searchDir = QUrl(QDir::homePath().append(QDir::separator()).append(extraSubDir));
    QString url = KFileDialog::getOpenFileName(searchDir, filter, 0, caption);
    return url;
}

QString CheckoutCreator::getDir() const
{
    QString url = QFileDialog::getExistingDirectory();
    return url;
}

bool CheckoutCreator::isGitDir(QString directory) const
{
    QDir dir(directory);
    if(dir.exists(".git/config"))
        return true;
    return false;
}

QString CheckoutCreator::createClone(QString userVisibleName, QString url, QString localPath, QObject* credentials) const
{
    if(!d->checkUserDetails()) {
        KMessageBox::sorry(0, "I'm sorry, we cannot perform git actions without a name and email set, and the git setup on this machine lacks this information. As a result, we are aborting this clone. Please try again, and enter your name and email next time.");
        LibQGit2::shutdownLibQGit2();
        return QString();
    }

    if(userVisibleName.isEmpty()) {
        // this should normally not be hit, as the form which calls this checks for this
        // anyway, but let's just be sure
        KMessageBox::sorry(0, "You forgot to name your account. Please do that and try again.");
        LibQGit2::shutdownLibQGit2();
        return QString();
    }

    bool credentialsOk = true;
    d->privateKey = credentials->property("privateKeyFile").toString();
    d->publicKey = credentials->property("publicKeyFile").toString();
    d->userForRemote = credentials->property("userForRemote").toString();
    d->needsPrivateKeyPassphrase = credentials->property("needsPrivateKeyPassphrase").toBool();
    if(!QFile::exists(d->privateKey) && credentialsOk) { credentialsOk = false; }
    if(!QFile::exists(d->publicKey) && credentialsOk) { credentialsOk = false; }
    if(!credentialsOk) {
        KMessageBox::sorry(0, "Something is wrong with your security credentials. Please check them and try again. This is likely due to one or another keyfile not existing, or there being no username entered.");
        LibQGit2::shutdownLibQGit2();
        return QString();
    }

    // a general Repository url is server.name:somedir/reponame.git - sometimes different, but very common, so try that first
    QString repoName = url.split('/').last().split('.').first();
    if(repoName.isEmpty()) {
        repoName = userVisibleName;
        repoName = repoName.remove('/');
        repoName = repoName.remove('.');
        repoName = repoName.replace(' ', "-");
    }

    QString checkoutLocation = QString("%1%2%3").arg(localPath).arg(QDir::separator()).arg(repoName);
    LibQGit2::Repository repo;
    connect(&repo, SIGNAL(cloneProgress(int)), this, SIGNAL(cloneProgress(int)));
    try {
        repo.setRemoteCredentials("origin", LibQGit2::Credentials::ssh(d->privateKey, d->publicKey, d->userForRemote.toUtf8(), d->getPassword().toUtf8()));
        repo.clone(url, checkoutLocation);
    }
    catch (const LibQGit2::Exception& ex) {
        qDebug() << ex.what() << ex.category();
        return QString();
    }

    return checkoutLocation;
}
