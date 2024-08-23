/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Aman Madaan <madaan.amanmadaan@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LINKINSERTDIALOG
#define LINKINSERTDIALOG
#include <KoBookmarkManager.h>
#include <KoTextDocument.h>
#include <KoTextRangeManager.h>
#include <QDialog>
#include <QListWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QWidget>
#include <ui_LinkInsertionDialog.h>

#define FETCH_TIMEOUT 5000

class LinkInsertionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LinkInsertionDialog(KoTextEditor *editor, QWidget *parent = nullptr);
    ~LinkInsertionDialog() override;

private Q_SLOTS:
    void insertLink();

public Q_SLOTS:

    void fetchTitleFromURL();
    void replyFinished();
    void fetchTitleError(QNetworkReply::NetworkError);
    void updateTitleDownloadProgress(qint64, qint64);
    void fetchTitleTimeout();
    /**
     * Verifies the text entered in the four line edits : Weblink URL, Weblink text,
     * Bookmark name and Bookmark text. The "Ok" button is enabled only if the input
     * is valid.
     * @param text is the text to be verified.
     */
    void enableDisableButtons(QString text);

    /**
     * Once all the line edits for a tab have been verified, the OK button is enabled.
     * If the tab is switched, the validity of OK should be recalculated for the new tab.
     * @param text is the current active tab.
     */
    void checkInsertEnableValidity(int);

private:
    Ui::LinkInsertionDialog dlg;
    KoTextEditor *m_editor;
    const KoBookmarkManager *m_bookmarkManager;
    QStringList m_bookmarkList;
    QNetworkReply *m_reply;
    QNetworkAccessManager *m_networkAccessManager;
    QUrl m_linkURL;
    QTimer m_timeoutTimer;
    void accept() override;
    void sendRequest();
    void insertBookmarkLink(const QString &URL, const QString &text);
    void insertHyperlink(QString &linkURL, const QString &linkText);
    void displayInlineWarning(const QString &title, QLabel *label) const;
    bool exists(const QString &) const;
};
#endif
