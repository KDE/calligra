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

#include "gitcontroller.h"
#include "documentlistmodel.h"

#include <kmessagebox.h>
#include <kinputdialog.h>
#include <kuser.h>
#include <kemailsettings.h>

#include <QDebug>
#include <QTimer>
#include <QTextCodec>

#include <git2.h>

class GitController::Private {
public:
    Private(GitController* q)
        : documents(new DocumentListModel(q))
        , commitAndPushAction(0)
    {}
    QString cloneDir;
    DocumentListModel* documents;
    QAction* commitAndPushAction;
    QString currentFile;
    QString userName;
    QString userEmail;

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
            QString newName = KInputDialog::getText("Enter Name",
                                                    "There is no name set for Git on this system (this is used when committing). Please enter one below and press OK.",
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
            QString newEmail = KInputDialog::getText("Enter Email",
                                                    "There is no email address set for Git on this system (this is used when committing). Please enter one below and press OK.",
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
        return true;
    }

    void check_error(int errorCode, const char* description)
    {
        if(errorCode) {
            qDebug() << "Operation failed:"<< description;
        }
    }
};

GitController::GitController(QObject* parent)
    : QObject(parent)
    , d(new Private(this))
{
}

GitController::~GitController()
{
    delete d;
}

QString GitController::cloneDir() const
{
    return d->cloneDir;
}

void GitController::setCloneDir(const QString& newDir)
{
    d->cloneDir = newDir;
    d->documents->setDocumentsFolder(newDir);
    QTimer::singleShot(100, d->documents, SLOT(startSearch()));
    emit cloneDirChanged();
}

QString GitController::currentFile() const
{
    return d->currentFile;
}

void GitController::setCurrentFile(QString& newFile)
{
    d->currentFile = newFile;
    emit currentFileChanged();
}

QAbstractListModel* GitController::documents() const
{
    return d->documents;
}

QAction* GitController::commitAndPushCurrentFileAction()
{
    if(!d->commitAndPushAction)
    {
        d->commitAndPushAction = new QAction(QIcon::fromTheme("folder-remote"), "Update Git Copy", this);
        connect(d->commitAndPushAction, SIGNAL(triggered(bool)), SLOT(commitAndPushCurrentFile()));
    }
    return d->commitAndPushAction;
}

// /* Credential callback */
// int credential_cb(git_cred **out,
//                   const char *url,
//                   const char *username_from_url,
//                   unsigned int allowed_types,
//                   void *payload)
// {
//     git_cred_userpass_payload *payloadActual = (git_cred_userpass_payload*)payload;
// 
//     KPasswordDialog dlg(0, KPasswordDialog::ShowUsernameLine);
//     dlg.setPrompt("Enter the username and password for your remote git repository to allow us to push. Cancelling aborts the push.");
//     if( !dlg.exec() )
//         return; //the user canceled
//     //use( dlg.username() , dlg.password() );
//     payloadActual.username = dlg.username().toLocal8Bit();
//     payloadActual.password = dlg.password().toLocal8Bit();
// }

void GitController::commitAndPushCurrentFile()
{
    qDebug() << "commit and push" << d->currentFile;

    // Don't allow committing unless the user details are sensible
    if(!d->checkUserDetails()) {
        KMessageBox::sorry(0, "I'm sorry, we cannot create commits without a username and email set.");
        return;
    }

    // ensure file is in current repository
    git_repository* repo;
    int error = git_repository_open(&repo, d->cloneDir.toLocal8Bit());
    if(error == 0)
    {
        if(d->currentFile.startsWith(d->cloneDir))
        {
            QString relative = d->currentFile.mid(d->cloneDir.length() + 1);
            bool shouldPush = false;
            int status = 0;
            unsigned int statusFlags = 0;
            status = git_status_file(&statusFlags, repo, relative.toLocal8Bit());
            // ensure file is added already
            if(status == 0)
            {
                // if so, ask commit message and checkbox for push (default on, remember?)
                bool ok = false;
                QString message = KInputDialog::getMultiLineText("Describe changes",
                                                                 "Please enter a description of your changes (also known as a commit message).",
                                                                 "Commit message",
                                                                 &ok, 0);
                // if user pressed cancel, cancel out now...
                // we explicitly leave the action enabled here because we want the user to be able to
                // regret their cancellation and commit anyway
                if(!ok)
                    return;

                // Get the current index
                git_index* index;
                git_repository_index(&index, repo);

                // refresh it, and add the file
                git_index_read(index);
                git_index_add_bypath(index, relative.toLocal8Bit());
                git_index_write(index);

                // convert the index to a tree, so we can use that to create the commit
                git_oid newTree;
                git_index_write_tree(&newTree, index);
                git_index_free(index);
                git_tree* tree;
                git_tree_lookup(&tree, repo, &newTree);

                // create the commit
                git_signature *me = NULL;
                int error = git_signature_now(&me, d->userName.toLocal8Bit(), d->userEmail.toLocal8Bit());

                git_oid head;
                git_reference_name_to_id(&head, repo, "HEAD");
                git_commit* headCommit;
                git_commit_lookup(&headCommit, repo, &head);

                const git_commit *parents[] = {headCommit};

                QTextCodec* codec = QTextCodec::codecForLocale();
                git_oid new_commit_id;
                error = git_commit_create(&new_commit_id,
                                          repo,
                                          "HEAD",                      /* name of ref to update */
                                          me,                          /* author */
                                          me,                          /* committer */
                                          codec->name(),               /* message encoding */
                                          message.toLocal8Bit(),       /* message */
                                          tree,                        /* root tree */
                                          1,                           /* parent count */
                                          parents);                    /* parents */

                git_tree_free(tree);
                git_signature_free(me);
                git_commit_free(headCommit);
                if(error) {
                    qDebug() << "something went wrong, nuh! :O" << error;
                }
                shouldPush = true;
            }
            else if(status == GIT_ENOTFOUND)
            {
                KMessageBox::sorry(0, "Sorry, this file is not yet added to the Git repository. Please add it and try again! (yes, this will be done for you later, but right now you'll have to do it yourself)");
                // if not, ask if it should be added
            }
            else
            {
                // Oh dear, some weirdness...
            }
            if(shouldPush) {
                git_remote* remote;
                git_push* push;
//                 bool cred_acquire_called;

                d->check_error(git_remote_load(&remote,repo,"origin"),"load a remote");
//                 git_remote_set_cred_acquire_cb(remote, (git_cred_acquire_cb)credential_cb, &cred_acquire_called);
                d->check_error(git_remote_connect(remote, GIT_DIRECTION_PUSH),"connect remote");
                d->check_error(git_push_new(&push, remote),"new push object");
                d->check_error(git_push_add_refspec(push,"refs/heads/master:refs/heads/master"),"add push refspec");

                d->check_error(git_push_finish(push),"finish pushing");
                d->check_error(git_push_unpack_ok(push),"unpacked OK? ");


                git_push_free(push);
                git_remote_free(remote);
            }
        }
        // if not, nothing we can do, derp - it can be fixed later, but for now, nopenopenope
        d->commitAndPushAction->setEnabled(false);
    }
    else {
        // kapow! error thing
    }
    git_repository_free(repo);
}


#include "gitcontroller.moc"
