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

#include <qgit2.h>
#include <qgit2/qgitglobal.h>

#include <git2.h>
#include <git2/branch.h>
#include <git2/refs.h>
#include <git2/merge.h>
#include <git2/cred_helpers.h>

class GitOpsThread::Private {
public:
    Private(QString privateKey, QString publicKey, QString userForRemote, bool needsPrivateKeyPassphrase, git_signature* signature, QString gitDir, GitOperation operation, QString currentFile, QString message) 
        : privateKey(privateKey)
        , publicKey(publicKey)
        , userForRemote(userForRemote)
        , needsPrivateKeyPassphrase(needsPrivateKeyPassphrase)
        , currentFile(currentFile)
        , message(message)
        , abort(false)
        , signature(signature)
        , gitDir(gitDir)
        , gitOp(operation)
    {}

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

GitOpsThread::GitOpsThread(QString privateKey, QString publicKey, QString userForRemote, bool needsPrivateKeyPassphrase, git_signature* signature, QString gitDir, GitOperation operation, QString currentFile, QString message, QObject *parent)
    : QObject(parent)
    , d(new Private(privateKey, publicKey, userForRemote, needsPrivateKeyPassphrase, signature, gitDir, operation, currentFile, message))
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
    // ensure file is in current repository
    LibQGit2::Repository repo;
    connect(&repo, SIGNAL(cloneProgress(int)), SIGNAL(transferProgress(int)));
    try {
        repo.open(QString("%1/.git").arg(d->gitDir));
        // Get the current index
        LibQGit2::Index index = repo.index();
        // refresh it, and add the file
        index.read(true);
        QString relative = d->currentFile.mid(d->gitDir.length() + 1);
        index.addByPath(relative);
        index.write();
        // convert the index to a tree, so we can use that to create the commit
        LibQGit2::Tree newTree = repo.lookupTree(index.createTree());
        // create the commit
        LibQGit2::Commit headCommit = repo.lookupCommit(repo.head().target());
        QList<LibQGit2::Commit> parents;
        parents << headCommit;
        repo.createCommit(newTree, parents, LibQGit2::Signature(d->signature), LibQGit2::Signature(d->signature), d->message, "HEAD");

        // Find the current branch's upstream remote
        git_reference *current_branch;
        git_repository_head(&current_branch, repo.data());
        LibQGit2::Reference currentBranch(current_branch);

        git_reference *upstream;
        git_branch_upstream(&upstream, currentBranch.data());
        LibQGit2::Reference upstreamRef(upstream);

        // Now find the name of the remote
        git_buf remote_name = {0,0,0};
        git_branch_remote_name(&remote_name, repo.data(), git_reference_name(upstreamRef.data()));
        QString remoteName = QString::fromUtf8(remote_name.ptr);
        git_buf_free(&remote_name);

        // And the upstream and local branch names...
        const char *branch_name;
        git_branch_name(&branch_name, upstreamRef.data());
        QString upstreamBranchName = QString::fromUtf8(branch_name);
        upstreamBranchName.remove(0, remoteName.length() + 1);
        git_branch_name(&branch_name, currentBranch.data());
        QString branchName = QString::fromUtf8(branch_name);

        repo.setRemoteCredentials(remoteName, LibQGit2::Credentials::ssh(d->privateKey, d->publicKey, d->userForRemote.toUtf8(), d->getPassword().toUtf8()));
        connect(repo.remote(remoteName), SIGNAL(transferProgress(int)), SIGNAL(transferProgress(int)));
        LibQGit2::Push push = repo.push(remoteName);
        push.addRefSpec(QString("refs/heads/%1:refs/heads/%2").arg(branchName).arg(upstreamBranchName));
        push.execute();
        emit pushCompleted();
    }
    catch (const LibQGit2::Exception& ex) {
        qDebug() << ex.what() << ex.category();
    }
}

void GitOpsThread::performPull()
{
    LibQGit2::Repository qrepo;
    connect(&qrepo, SIGNAL(cloneProgress(int)), SIGNAL(transferProgress(int)));
    try {
        qrepo.open(QString("%1/.git").arg(d->gitDir));

        // Find the current branch's upstream remote
        git_reference *current_branch;
        git_repository_head(&current_branch, qrepo.data());
        LibQGit2::Reference currentBranch(current_branch);

        git_reference *upstream;
        git_branch_upstream(&upstream, currentBranch.data());
        LibQGit2::Reference upstreamRef(upstream);

        // Now find the name of the remote
        git_buf remote_name = {0,0,0};
        git_branch_remote_name(&remote_name, qrepo.data(), git_reference_name(upstreamRef.data()));
        QString remoteName = QString::fromUtf8(remote_name.ptr);
        git_buf_free(&remote_name);

        // Finally set the credentials on it that we're given, and fetch it
        qrepo.setRemoteCredentials(remoteName, LibQGit2::Credentials::ssh(d->privateKey, d->publicKey, d->userForRemote.toUtf8(), d->getPassword().toUtf8()));
        connect(qrepo.remote(remoteName), SIGNAL(transferProgress(int)), SIGNAL(transferProgress(int)));
        qrepo.fetch(remoteName);

        git_branch_upstream(&upstream, currentBranch.data());
        upstreamRef = LibQGit2::Reference(upstream);

        // Let's check and see what sort of merge we should be doing...
        git_merge_analysis_t analysis;
        git_merge_preference_t preference;
        git_merge_head *merge_heads[1];

        git_merge_head_from_ref(&merge_heads[0], qrepo.data(), upstreamRef.data());
        int error = git_merge_analysis(&analysis, &preference, qrepo.data(), (const git_merge_head **)&merge_heads, 1);
        if(error == GIT_OK) {
            if(GIT_MERGE_ANALYSIS_UP_TO_DATE == (analysis & GIT_MERGE_ANALYSIS_UP_TO_DATE)) {
                // If we're already up to date, yay, no need to do anything!
                qDebug() << "all up to date, yeah!";
                git_merge_head_free(merge_heads[0]);
            } else if(GIT_MERGE_ANALYSIS_UNBORN == (analysis & GIT_MERGE_ANALYSIS_UNBORN)) {
                // this is silly, don't give me an unborn repository you silly person
                qDebug() << "huh, we have an unborn repo here...";
                git_merge_head_free(merge_heads[0]);
            } else if(GIT_MERGE_ANALYSIS_FASTFORWARD == (analysis & GIT_MERGE_ANALYSIS_FASTFORWARD) && (GIT_MERGE_PREFERENCE_NO_FASTFORWARD != (preference & GIT_MERGE_PREFERENCE_NO_FASTFORWARD))) {
                // If the analysis says we can fast forward, then let's fast forward!
                // ...unless preferences say to never fast forward, of course
                qDebug() << "fast forwarding all up in that thang";

                // the code below was modified from an original (GPL2) version by the git2r community
                const git_oid *oid;
                git_buf log_message = {0,0,0};
                git_commit *commit = NULL;
                git_tree *tree = NULL;
                git_reference *reference = NULL;
                git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;

                git_repository_message(&log_message, qrepo.data());

                oid = git_merge_head_id(merge_heads[0]);
                error = git_commit_lookup(&commit, qrepo.data(), oid);
                if (error == GIT_OK) {
                    error = git_commit_tree(&tree, commit);
                    if (error == GIT_OK) {
                        opts.checkout_strategy = GIT_CHECKOUT_SAFE;
                        error = git_checkout_tree(qrepo.data(), (git_object*)tree, &opts);
                        if (error == GIT_OK) {
                            error = git_repository_head(&reference, qrepo.data());
                            if (error == GIT_OK && error != GIT_ENOTFOUND) {
                                if (error == GIT_OK) {
                                    if (error == GIT_ENOTFOUND) {
                                        error = git_reference_create(
                                            &reference,
                                            qrepo.data(),
                                            "HEAD",
                                            git_commit_id(commit),
                                            0, /* force */
                                            d->signature,
                                            log_message.ptr);
                                    } else {
                                        git_reference *target_ref = NULL;

                                        error = git_reference_set_target(
                                            &target_ref,
                                            reference,
                                            git_commit_id(commit),
                                            d->signature,
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
                // Leaving this code in for now - this /should/ as far as i can tell do the same
                // as the code above. However, it looks as though it doesn't. If anybody can work
                // out why, i would appreciate learning what went wrong :P
                //const git_oid* id = git_merge_head_id(merge_heads[0]);
                //LibQGit2::OId mergeId(id);
                //LibQGit2::Commit headCommit = qrepo.lookupCommit(mergeId);

                //qrepo.checkoutTree(headCommit.tree());
                //qrepo.head().setTarget(headCommit.oid());
                //qrepo.reset(headCommit);
                git_merge_head_free(merge_heads[0]);
                git_repository_state_cleanup(qrepo.data());
            }
            else if(GIT_MERGE_ANALYSIS_NORMAL == (analysis & GIT_MERGE_ANALYSIS_NORMAL)) {
                // If the analysis says we are able to do a normal merge, let's attempt one of those...
                if(GIT_MERGE_PREFERENCE_FASTFORWARD_ONLY == (preference & GIT_MERGE_PREFERENCE_FASTFORWARD_ONLY)) {
                    // but only if we're not told to not try and not do fast forwards!
                    KMessageBox::sorry(0, "Fast Forward Only", "We're attempting to merge, but the repository is set to only do fast forwarding - sorry, we don't support this scenario and you'll need to handle things yourself...");
                } else {
                    git_merge(qrepo.data(), (const git_merge_head **) merge_heads, 1, NULL, NULL);
                    git_merge_head_free(merge_heads[0]);
                    if (qrepo.index().hasConflicts()) {
                        qDebug() << "There were conflicts merging. Please resolve them and commit";
                    } else {
                        git_oid commit_id;
                        git_buf message = {0,0,0};
                        git_commit *parents[2];
                        LibQGit2::Index index = qrepo.index();
                        LibQGit2::OId tree_id = index.createTree();
                        LibQGit2::Tree tree = qrepo.lookupTree(tree_id);

                        git_repository_message(&message, qrepo.data());
                        const char *branch_name;
                        git_branch_name(&branch_name, upstreamRef.data());
                        QString upstreamBranchName = QString::fromUtf8(branch_name);
                        git_branch_name(&branch_name, currentBranch.data());
                        QString branchName = QString::fromUtf8(branch_name);

                        git_reference *upstream;
                        git_branch_upstream(&upstream, currentBranch.data());
                        LibQGit2::Reference upstreamRef(upstream);

                        error = git_commit_lookup(&parents[0], qrepo.data(), qrepo.head().target().data());
                        d->check_error(error, "looking up current branch");
                        error = git_commit_lookup(&parents[1], qrepo.data(), upstreamRef.target().data());
                        d->check_error(error, "looking up remote branch");

                        git_commit_create(&commit_id, qrepo.data(), "HEAD", d->signature, d->signature,
                                        NULL, message.ptr,
                                        tree.data(), 2, (const git_commit **) parents);
                    }
                }
            } else {
                // how did i get here, i am not good with undefined entries in enums
                qDebug() << "wait, what?";
                git_merge_head_free(merge_heads[0]);
            }
        }
        git_repository_state_cleanup(qrepo.data());
        emit pullCompleted();
    } catch (const LibQGit2::Exception& ex) {
        qDebug() << ex.what() << ex.category();
    }
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
    LibQGit2::initLibQGit2();
    d = new Private(this);
}

GitController::~GitController()
{
    delete d;
    LibQGit2::shutdownLibQGit2();
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
    if("\\" == QDir::separator() &&  newFile.contains("/")) {
        d->currentFile = d->currentFile.replace("/", QDir::separator());
    }
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
