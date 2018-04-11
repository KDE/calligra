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

#include <KoIcon.h>
#include <KLocalizedString>

#include <kpassworddialog.h>
#include <kmessagebox.h>
#include <kuser.h>
#include <kemailsettings.h>

#include <QInputDialog>
#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QTextCodec>
#include <QThreadPool>

#include <git2.h>
#include <git2/branch.h>
#include <git2/refs.h>
#include <git2/merge.h>
#include <git2/cred_helpers.h>
#include <git2/repository.h>
#include <git2/tree.h>

class GitOpsThread::Private {
public:
    Private(GitOpsThread* qq, QString privateKey, QString publicKey, QString userForRemote, bool needsPrivateKeyPassphrase, git_signature* signature, QString gitDir, GitOperation operation, QString currentFile, QString message) 
        : q(qq)
        , privateKey(privateKey)
        , publicKey(publicKey)
        , userForRemote(userForRemote)
        , needsPrivateKeyPassphrase(needsPrivateKeyPassphrase)
        , currentFile(currentFile)
        , message(message)
        , abort(false)
        , signature(signature)
        , gitDir(gitDir)
        , gitOp(operation)
        , progress(0)
    {}

    GitOpsThread* q;
    QString privateKey;
    QString publicKey;
    QString userForRemote;
    bool needsPrivateKeyPassphrase;

    QString currentFile;
    QString message;

    bool abort;
    git_signature* signature;
    QString gitDir;
    GitOperation gitOp;

    int progress;

    QString getPassword()
    {
        if(!needsPrivateKeyPassphrase)
            return QString();
        KPasswordDialog dlg;
        dlg.setWindowTitle("Private Key Passphrase");
        dlg.setPrompt("Your private key file requires a password. Please enter it here. You will be asked again each time it is accessed, and the password is not stored.");
        dlg.exec();
        return dlg.password();
    }

    // returns true if errorCode is 0 (in which case there was no error!)
    bool check_error(int errorCode, const char* description)
    {
        if(errorCode) {
            qDebug() << "Operation failed:"<< description << errorCode;
            return false;
        }
        return true;
    }

    static int transferProgressCallback(const git_transfer_progress* stats, void* data)
    {
        if (!data) {
            return 1;
        }

        Private *payload = static_cast<Private*>(data);
        int percent = (int)(0.5 + 100.0 * ((double)stats->received_objects) / ((double)stats->total_objects));
        if (percent != payload->progress) {
            emit payload->q->transferProgress(percent);
            payload->progress = percent;
        }
        return 0;
    }

    static int acquireCredentialsCallback(git_cred **cred, const char */*url*/, const char *username_from_url, unsigned int /*allowed_types*/, void *data)
    {
        int result = -1;
        if (data) {
            Private* payload = static_cast<Private*>(data);
            if(payload->needsPrivateKeyPassphrase) {
                result = git_cred_ssh_key_new(cred, username_from_url, payload->publicKey.toLatin1(), payload->privateKey.toLatin1(), payload->getPassword().toLatin1());
            }
            else {
                result = git_cred_ssh_key_new(cred, username_from_url, payload->publicKey.toLatin1(), payload->privateKey.toLatin1(), "");
            }
        }

        return result;
    }
};

GitOpsThread::GitOpsThread(QString privateKey, QString publicKey, QString userForRemote, bool needsPrivateKeyPassphrase, git_signature* signature, QString gitDir, GitOperation operation, QString currentFile, QString message, QObject *parent)
    : QObject(parent)
    , d(new Private(this, privateKey, publicKey, userForRemote, needsPrivateKeyPassphrase, signature, gitDir, operation, currentFile, message))
{
}

GitOpsThread::~GitOpsThread()
{
    delete d;
}

void GitOpsThread::run()
{
    switch(d->gitOp)
    {
        case PushOperation:
            performPush();
            emit pushCompleted();
            break;
        case PullOperation:
            performPull();
            emit pullCompleted();
            break;
        default:
            break;
    }
}

void GitOpsThread::abort()
{
    d->abort = true;
}

void GitOpsThread::performPush()
{
    git_repository* repository;
    int error = git_repository_open(&repository, QString("%1/.git").arg(d->gitDir).toLatin1());
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 1, error code from git2 was" << error << "which is described as" << err->message; return; }

    // Get the current index
    git_index* index;
    error = git_repository_index(&index, repository);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 2, error code from git2 was" << error << "which is described as" << err->message; return; }

    // refresh it, and add the file
    error = git_index_read(index, true);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 3, error code from git2 was" << error << "which is described as" << err->message; return; }
    QString relative = d->currentFile.mid(d->gitDir.length() + 1); // That is, 1 for the leading slash
    error = git_index_add_bypath(index, relative.toLocal8Bit());
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 4, error code from git2 was" << error << "which is described as" << err->message; return; }
    error = git_index_write(index);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 5, error code from git2 was" << error << "which is described as" << err->message; return; }

    // convert the index to a tree, so we can use that to create the commit
    git_tree* tree;
    git_oid tree_id;
    error = git_index_write_tree(&tree_id, index);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 6, error code from git2 was" << error << "which is described as" << err->message; return; }
    error = git_tree_lookup(&tree, repository, &tree_id);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 7, error code from git2 was" << error << "which is described as" << err->message; return; }

    // get where we want to parent things to
    git_oid obj;
    error = git_reference_name_to_id(&obj, repository, "HEAD");
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 8, error code from git2 was" << error << "which is described as" << err->message; return; }
    git_commit *parent = NULL;
    error = git_commit_lookup(&parent, repository, &obj);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 9, error code from git2 was" << error << "which is described as" << err->message; return; }

    // create the commit
    git_oid oid;
    error = git_commit_create_v(&oid, repository, "HEAD", d->signature, d->signature, "UTF-8", d->message.toLatin1(), tree, 1, parent);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 10, error code from git2 was" << error << "which is described as" << err->message; return; }

    error = git_repository_state_cleanup(repository);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 11, error code from git2 was" << error << "which is described as" << err->message; return; }


    // Find the current branch's upstream remote
    git_reference *current_branch;
    error = git_repository_head(&current_branch, repository);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 12, error code from git2 was" << error << "which is described as" << err->message; return; }

    git_reference *upstream;
    error = git_branch_upstream(&upstream, current_branch);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 13, error code from git2 was" << error << "which is described as" << err->message; return; }

    // Now find the name of the remote
    git_buf remote_name = {0,0,0};
    error = git_branch_remote_name(&remote_name, repository, git_reference_name(upstream));
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 14, error code from git2 was" << error << "which is described as" << err->message; return; }
    QString remoteName = QString::fromUtf8(remote_name.ptr);
    git_buf_free(&remote_name);

    // And the upstream and local branch names...
    const char *branch_name;
    error = git_branch_name(&branch_name, upstream);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 15, error code from git2 was" << error << "which is described as" << err->message; return; }
    QString upstreamBranchName = QString::fromUtf8(branch_name);
    upstreamBranchName.remove(0, remoteName.length() + 1);
    error = git_branch_name(&branch_name, current_branch);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 16, error code from git2 was" << error << "which is described as" << err->message; return; }
    QString branchName = QString::fromUtf8(branch_name);

    git_remote_callbacks remoteCallbacks = GIT_REMOTE_CALLBACKS_INIT;
    remoteCallbacks.payload = (void*)this->d;
    remoteCallbacks.transfer_progress = &Private::transferProgressCallback;
    remoteCallbacks.credentials = &Private::acquireCredentialsCallback;
    git_remote* remote;
    error = git_remote_lookup(&remote, repository, "origin");
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 17, error code from git2 was" << error << "which is described as" << err->message; return; }

    char tempPath[512] = "refs/heads/";
    char tempPath2[512] = "refs/heads/";
    strcat(tempPath, branch_name);
    strcat(tempPath2, upstreamBranchName.toLocal8Bit());
    char *refs[2] = { tempPath, tempPath2 };
    git_strarray uploadrefs;
    uploadrefs.strings = refs;
    uploadrefs.count = 2;

    git_push_options pushOptions;
    error = git_push_init_options(&pushOptions, GIT_PUSH_OPTIONS_VERSION);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 18, error code from git2 was" << error << "which is described as" << err->message; return; }
    pushOptions.callbacks = remoteCallbacks;

    error = git_remote_push(remote, &uploadrefs, &pushOptions);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Push 19, error code from git2 was" << error << "which is described as" << err->message; return; }

    emit pushCompleted();
}

void GitOpsThread::performPull()
{
    git_repository* repository;
    int error = git_repository_open(&repository, QString("%1/.git").arg(d->gitDir).toLatin1());
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return; }

    // Find the current branch's upstream remote
    git_reference *current_branch;
    error = git_repository_head(&current_branch, repository);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return; }

    git_reference *upstream;
    error = git_branch_upstream(&upstream, current_branch);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return; }

    // Now find the name of the remote
    git_buf remote_name = {0,0,0};
    error = git_branch_remote_name(&remote_name, repository, git_reference_name(upstream));
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return; }

    // Finally set the credentials on it that we're given, and fetch it
    git_remote_callbacks remoteCallbacks = GIT_REMOTE_CALLBACKS_INIT;
    remoteCallbacks.payload = (void*)this->d;
    remoteCallbacks.transfer_progress = &Private::transferProgressCallback;
    remoteCallbacks.credentials = &Private::acquireCredentialsCallback;
    git_remote* remote;
    error = git_remote_lookup(&remote, repository, remote_name.ptr);
    git_buf_free(&remote_name);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return; }
    git_fetch_options fetch_options = GIT_FETCH_OPTIONS_INIT;
    fetch_options.callbacks = remoteCallbacks;
    error = git_remote_fetch(remote, NULL, &fetch_options, NULL);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return; }

    error = git_branch_upstream(&upstream, current_branch);
    if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return; }

    // Let's check and see what sort of merge we should be doing...
    git_merge_analysis_t analysis;
    git_merge_preference_t preference;
    git_annotated_commit *merge_heads[1];

    git_annotated_commit_from_ref(&merge_heads[0], repository, upstream);
    error = git_merge_analysis(&analysis, &preference, repository, (const git_annotated_commit **)merge_heads, 1);
    if(error == GIT_OK) {
        if(GIT_MERGE_ANALYSIS_UP_TO_DATE == (analysis & GIT_MERGE_ANALYSIS_UP_TO_DATE)) {
            // If we're already up to date, yay, no need to do anything!
            qDebug() << "all up to date, yeah!";
            git_annotated_commit_free(merge_heads[0]);
        } else if(GIT_MERGE_ANALYSIS_UNBORN == (analysis & GIT_MERGE_ANALYSIS_UNBORN)) {
            // this is silly, don't give me an unborn repository you silly person
            qDebug() << "huh, we have an unborn repo here...";
            git_annotated_commit_free(merge_heads[0]);
        } else if(GIT_MERGE_ANALYSIS_FASTFORWARD == (analysis & GIT_MERGE_ANALYSIS_FASTFORWARD) && (GIT_MERGE_PREFERENCE_NO_FASTFORWARD != (preference & GIT_MERGE_PREFERENCE_NO_FASTFORWARD))) {
            // If the analysis says we can fast forward, then let's fast forward!
            // ...unless preferences say to never fast forward, of course
            qDebug() << "fast forwarding all up in that thang";
            git_merge_options mergeopts = GIT_MERGE_OPTIONS_INIT;
            git_checkout_options checkoutopts = GIT_CHECKOUT_OPTIONS_INIT;
            checkoutopts.checkout_strategy = GIT_CHECKOUT_SAFE;
            git_merge(repository, (const git_annotated_commit **)merge_heads, 1, &mergeopts, &checkoutopts);


            // the code below was modified from an original (GPL2) version by the git2r community
            const git_oid *oid;
            git_buf log_message = {0,0,0};
            git_commit *commit = NULL;
            git_tree *tree = NULL;
            git_reference *reference = NULL;
            git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;

            git_repository_message(&log_message, repository);

            oid = git_annotated_commit_id(merge_heads[0]);
            error = git_commit_lookup(&commit, repository, oid);
            if (error == GIT_OK) {
                error = git_commit_tree(&tree, commit);
                if (error == GIT_OK) {
                    opts.checkout_strategy = GIT_CHECKOUT_SAFE;
                    error = git_checkout_tree(repository, (git_object*)tree, &opts);
                    if (error == GIT_OK) {
                        error = git_repository_head(&reference, repository);
                        if (error == GIT_OK && error != GIT_ENOTFOUND) {
                            if (error == GIT_OK) {
                                if (error == GIT_ENOTFOUND) {
                                    error = git_reference_create(
                                        &reference,
                                        repository,
                                        "HEAD",
                                        git_commit_id(commit),
                                        0, // force
                                        log_message.ptr);
                                } else {
                                    git_reference *target_ref = NULL;

                                    error = git_reference_set_target(
                                        &target_ref,
                                        reference,
                                        git_commit_id(commit),
                                        log_message.ptr);

                                    if (target_ref) {
                                        git_reference_free(target_ref);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (commit) {
                git_commit_free(commit);
            }
            if (reference) {
                git_reference_free(reference);
            }
            if (tree) {
                git_tree_free(tree);
            }
            git_annotated_commit_free(merge_heads[0]);
            git_repository_state_cleanup(repository);
        }
        else if(GIT_MERGE_ANALYSIS_NORMAL == (analysis & GIT_MERGE_ANALYSIS_NORMAL)) {
            // If the analysis says we are able to do a normal merge, let's attempt one of those...
            if(GIT_MERGE_PREFERENCE_FASTFORWARD_ONLY == (preference & GIT_MERGE_PREFERENCE_FASTFORWARD_ONLY)) {
            // but only if we're not told to not try and not do fast forwards!
            KMessageBox::sorry(0, "Fast Forward Only", "We're attempting to merge, but the repository is set to only do fast forwarding - sorry, we don't support this scenario and you'll need to handle things yourself...");
            } else {
                git_merge(repository, (const git_annotated_commit **) merge_heads, 1, NULL, NULL);
                git_annotated_commit_free(merge_heads[0]);
                git_index* index;
                git_repository_index(&index, repository);
                if (git_index_has_conflicts(index)) {
                    qDebug() << "There were conflicts merging. Please resolve them and commit";
                } else {
                    git_buf message = {0, 0, 0};
                    git_oid commit_id, tree_id;
                    git_commit *parents[2];
                    git_tree *tree;

                    git_index_write_tree(&tree_id, index);

                    git_repository_message(&message, repository);

                    git_tree_lookup(&tree, repository, &tree_id);

                    error = git_commit_lookup(&parents[0], repository, git_reference_target(current_branch));
                    d->check_error(error, "looking up current branch");
                    error = git_commit_lookup(&parents[1], repository, git_reference_target(upstream));
                    d->check_error(error, "looking up remote branch");

                    git_commit_create(&commit_id, repository, "HEAD", d->signature, d->signature,
                                                NULL, message.ptr,
                                                tree, 2, (const git_commit **) parents);
                    git_tree_free(tree);
                }
                git_index_free(index);
            }
        } else {
            // how did i get here, i am not good with undefined entries in enums
            qDebug() << "wait, what?";
            git_annotated_commit_free(merge_heads[0]);
        }
    }
    git_repository_state_cleanup(repository);
    git_repository_free(repository);
    emit pullCompleted();
}

class GitController::Private {
public:
    Private(GitController* q)
        : needsPrivateKeyPassphrase(false)
        , documents(new DocumentListModel(q))
        , commitAndPushAction(0)
        , signature(0)
        , opThread(0)
    {
    }
    ~Private()
    {
        git_signature_free(signature);
    }
    QString privateKey;
    QString publicKey;
    QString userForRemote;
    bool needsPrivateKeyPassphrase;

    QString cloneDir;
    DocumentListModel* documents;
    QAction* commitAndPushAction;
    QString currentFile;
    QString userName;
    QString userEmail;
    git_signature* signature;

    GitOpsThread* opThread;

    QString getPassword()
    {
        if(!needsPrivateKeyPassphrase)
            return QString();
        KPasswordDialog dlg;
        dlg.setWindowTitle("Private Key Passphrase");
        dlg.setPrompt("Your private key file requires a password. Please enter it here. You will be asked again each time it is accessed, and the password is not stored.");
        dlg.exec();
        return dlg.password();
    }

    bool checkUserDetails()
    {
        git_config* configActual;
        int error = git_config_open_default(&configActual);
        if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return false; }

        git_config* config;
        error = git_config_snapshot(&config, configActual);
        if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return false; }

        const char* name;
        error = git_config_get_string(&name, config, "user.name");
        if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return false; }

        const char* email;
        error = git_config_get_string(&email, config, "user.email");
        if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return false; }

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
            error = git_config_set_string(config, "user.name", newName.toLocal8Bit());
            if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return false; }
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
            error = git_config_set_string(config, "user.email", newEmail.toLocal8Bit());
            if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return false; }
        }

        git_config_free(config);

        if(userName.isEmpty() || userEmail.isEmpty()) {
            return false;
        }
        error = git_signature_now(&signature, userName.toLocal8Bit(), userEmail.toLocal8Bit());
        if(error != 0) { const git_error* err = giterr_last(); qDebug() << "Kapow, error code from git2 was" << error << "which is described as" << err->message; return false; }
        return true;
    }

    // returns true if errorCode is 0 (in which case there was no error!)
    bool check_error(int errorCode, const char* description)
    {
        if(errorCode) {
            qDebug() << "Operation failed:"<< description << errorCode;
            return false;
        }
        return true;
    }
};

GitController::GitController(QObject* parent)
    : QObject(parent)
{
    git_libgit2_init();
    d = new Private(this);
}

GitController::~GitController()
{
    delete d;
    git_libgit2_shutdown();
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
    // OK, so some silliness here. This ensures a bit of sanity later, because otherwise we
    // end up comparing a localised checkout dir to a non-localised current file (since that
    // is created from a URL, and the checkout dir isn't)
    // if("\\" == QDir::separator() &&  newFile.contains("/")) {
        // d->currentFile = d->currentFile.replace("/", QDir::separator());
    // }
    emit currentFileChanged();
}

QAbstractListModel* GitController::documents() const
{
    return d->documents;
}

QString GitController::privateKeyFile() const
{
    return d->privateKey;
}

void GitController::setPrivateKeyFile(QString newFile)
{
    d->privateKey = newFile;
    emit privateKeyFileChanged();
}

QString GitController::publicKeyFile() const
{
    return d->publicKey;
}

void GitController::setPublicKeyFile(QString newFile)
{
    d->publicKey = newFile;
    emit publicKeyFileChanged();
}

bool GitController::needsPrivateKeyPassphrase() const
{
    return d->needsPrivateKeyPassphrase;
}

void GitController::setNeedsPrivateKeyPassphrase(bool needsPassphrase)
{
    d->needsPrivateKeyPassphrase = needsPassphrase;
    emit needsPrivateKeyPassphrase();
}

QString GitController::userForRemote() const
{
    return d->userForRemote;
}

void GitController::setUserForRemote(QString newUser)
{
    d->userForRemote = newUser;
    emit userForRemoteChanged();
}

QAction* GitController::commitAndPushCurrentFileAction()
{
    if(!d->commitAndPushAction) {
        d->commitAndPushAction = new QAction(koIcon("folder-remote"), "Update Git Copy", this);
        connect(d->commitAndPushAction, SIGNAL(triggered(bool)), SLOT(commitAndPushCurrentFile()));
    }
    return d->commitAndPushAction;
}

void GitController::commitAndPushCurrentFile()
{
    if(d->opThread) {
        // if so, then we're already performing an operation of some kind, let's not confuse the point
        return;
    }

    // Don't allow committing unless the user details are sensible
    if(!d->checkUserDetails()) {
        KMessageBox::sorry(0, "I'm sorry, we cannot create commits without a username and email set. Please try again, and enter your name and email next time.");
        return;
    }

    if(d->currentFile.startsWith(d->cloneDir)) {
        // ask commit message and checkbox for push (default on, remember?)
        bool ok = false;
        QString message = QInputDialog::getMultiLineText(0,
                                                         i18n("Describe changes"),
                                                         i18n("Please enter a description of your changes (also known as a commit message)."),
                                                         i18n("Commit message"),
                                                         &ok);
        // if user pressed cancel, cancel out now...
        // we explicitly leave the action enabled here because we want the user to be able to
        // regret their cancellation and commit anyway
        if(ok) {
            qDebug() << "Attempting to push" << d->currentFile << "in the clone dir" << d->cloneDir;
            emit operationBegun(QString("Pushing local changes to remote storage"));
            d->opThread = new GitOpsThread(d->privateKey, d->publicKey, d->userForRemote, d->needsPrivateKeyPassphrase, d->signature, d->cloneDir, GitOpsThread::PushOperation, d->currentFile, message, this);
            connect(d->opThread, SIGNAL(destroyed()), this, SLOT(clearOpThread()));
            connect(d->opThread, SIGNAL(transferProgress(int)), this, SIGNAL(transferProgress(int)));
            connect(d->opThread, SIGNAL(pushCompleted()), this, SIGNAL(pushCompleted()));
            connect(d->opThread, SIGNAL(pushCompleted()), this, SLOT(disableCommitAndPushAction()));
            d->opThread->setAutoDelete(true);
            QThreadPool::globalInstance()->start(d->opThread);
        }
    } else {
        KMessageBox::sorry(0, QString("The file %1 is not located within the current clone directory of %2. Before you can commit the file, please save it there and try again.").arg(d->currentFile).arg(d->cloneDir));
    }
}

void GitController::clearOpThread()
{
    d->opThread = 0;
}

void GitController::disableCommitAndPushAction()
{
    d->commitAndPushAction->setEnabled(false);
}

void GitController::pull()
{
    if(d->opThread) {
        // if so, then we're already performing an operation of some kind, let's not confuse the point
        return;
    }

    // Don't allow committing unless the user details are sensible
    if(!d->checkUserDetails()) {
        KMessageBox::sorry(0, "I'm sorry, we cannot create commits without a name and email set, and we might need to do a merge later, so we are aborting this pull. Please try again, and enter your name and email next time.");
        return;
    }

    emit operationBegun(QString("Pulling any changes on the remote storage to your local clone"));
    d->opThread = new GitOpsThread(d->privateKey, d->publicKey, d->userForRemote, d->needsPrivateKeyPassphrase, d->signature, d->cloneDir, GitOpsThread::PullOperation, d->currentFile, QString(), this);
    connect(d->opThread, SIGNAL(destroyed()), this, SLOT(clearOpThread()));
    connect(d->opThread, SIGNAL(transferProgress(int)), this, SIGNAL(transferProgress(int)));
    connect(d->opThread, SIGNAL(pullCompleted()), this, SIGNAL(pullCompleted()));
    connect(d->opThread, SIGNAL(pullCompleted()), d->documents, SLOT(rescan()));
    d->opThread->setAutoDelete(true);
    QThreadPool::globalInstance()->start(d->opThread);
}
