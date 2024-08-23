// This file is part of the KDE project
// SPDX-FileCopyrightText: 2013 Aman Madaan <madaan.amanmadaan@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "LinkInsertionDialog.h"
#include "SimpleTableOfContentsWidget.h"

#include <KoTextEditor.h>

#include <QComboBox>
#include <QCompleter>
#include <QDialogButtonBox>
#include <QLabel>
#include <QString>
#include <QTimer>

LinkInsertionDialog::LinkInsertionDialog(KoTextEditor *editor, QWidget *parent)
    : QDialog(parent)
    , m_editor(editor)
    , m_bookmarkManager(nullptr)
    , m_bookmarkList(0)
    , m_reply(nullptr)
    , m_networkAccessManager(nullptr)
    , m_linkURL(nullptr)
    , m_timeoutTimer(nullptr)
{
    dlg.setupUi(this);
    setUpdatesEnabled(false);
    // set up the tabs with selected text
    QString suggestedLinkText;
    if (m_editor->hasSelection()) {
        suggestedLinkText = m_editor->selectedText();
        dlg.hyperlinkText->setText(suggestedLinkText);
        dlg.bookmarkLinkText->setText(suggestedLinkText);
    }
    connect(dlg.buttonBox, &QDialogButtonBox::accepted, this, &LinkInsertionDialog::insertLink);
    connect(dlg.buttonBox, &QDialogButtonBox::rejected, this, &QWidget::close);
    dlg.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    /// setting up the web link insertion tab
    m_networkAccessManager = new QNetworkAccessManager(this);
    connect(dlg.fetchTitleButton, &QAbstractButton::clicked, this, &LinkInsertionDialog::fetchTitleFromURL);
    dlg.fetchTitleButton->setEnabled(false);
    setUpdatesEnabled(true);

    /// setting up the bookmark link insertion tab
    // connect(dlg.bookmarkListWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
    m_bookmarkManager = KoTextDocument(editor->document()).textRangeManager()->bookmarkManager();
    m_bookmarkList = m_bookmarkManager->bookmarkNameList();
    QCompleter *bookmarkAutoCompleter = new QCompleter(m_bookmarkList, this);
    dlg.bookmarkLinkURL->setCompleter(bookmarkAutoCompleter);
    dlg.bookmarkLinkURL->addItems(m_bookmarkList);
    dlg.bookmarkLinkURL->clearEditText();
    connect(dlg.hyperlinkURL, &QLineEdit::textChanged, this, &LinkInsertionDialog::enableDisableButtons);
    connect(dlg.hyperlinkText, &QLineEdit::textChanged, this, &LinkInsertionDialog::enableDisableButtons);
    connect(dlg.bookmarkLinkURL, &QComboBox::editTextChanged, this, &LinkInsertionDialog::enableDisableButtons);
    connect(dlg.bookmarkLinkText, &QLineEdit::textChanged, this, &LinkInsertionDialog::enableDisableButtons);

    connect(dlg.linkTypesTab, &QTabWidget::currentChanged, this, &LinkInsertionDialog::checkInsertEnableValidity);
    show();
}
void LinkInsertionDialog::enableDisableButtons(QString text)
{
    text = text.trimmed();
    QObject *signalSender = (sender());
    if (qobject_cast<QLineEdit *>(signalSender) == dlg.hyperlinkURL) { // deal with fetch button
        if (!text.isEmpty()) { // is empty?
            if (!QUrl(text).isValid()) { // not empty, is it valid?
                dlg.fetchTitleButton->setEnabled(false);
                dlg.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false); // not valid too, time to get out
                displayInlineWarning(i18n("The URL is invalid"), dlg.weblinkStatusLabel);
                return;
            } else { // valid but non empty, can fetch but not sure about the others so can't OK
                displayInlineWarning("", dlg.weblinkStatusLabel);
                dlg.fetchTitleButton->setEnabled(true);
            }
        } else { // field is empty, no other choice
            dlg.fetchTitleButton->setEnabled(false);
            dlg.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            return;
        }
    } else if (qobject_cast<QComboBox *>(signalSender) == dlg.bookmarkLinkURL) { // need to check existence
        if (dlg.bookmarkLinkURL->currentText().isEmpty()) {
            displayInlineWarning("", dlg.bookmarkLinkStatusLabel);
            dlg.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            return;
        } else if (!exists(dlg.bookmarkLinkURL->currentText())) { // definitely can't go in
            displayInlineWarning(i18n("Bookmark does not exist"), dlg.bookmarkLinkStatusLabel);
            dlg.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            return;
        } else { // non empty and exits
            displayInlineWarning("", dlg.bookmarkLinkStatusLabel); // can clear the label but cannot be sure about OK
        }
    } else if (text.isEmpty()) { // for others, empty is definitely incorrect
        dlg.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return;
    }
    switch (dlg.linkTypesTab->currentIndex()) { // handle cases that reach here, only doubt is completeness
    case 0:
        if (!dlg.hyperlinkText->text().isEmpty() && QUrl(dlg.hyperlinkURL->text()).isValid() && !dlg.hyperlinkURL->text().isEmpty()) {
            dlg.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        }
        break;
    case 1:
        if (!dlg.bookmarkLinkText->text().isEmpty() && !dlg.bookmarkLinkURL->currentText().isEmpty() && exists(dlg.bookmarkLinkURL->currentText())) {
            dlg.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        }
        break;
    }
}

void LinkInsertionDialog::checkInsertEnableValidity(int currentTab)
{
    dlg.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    switch (currentTab) {
    case 0:
        if (!dlg.hyperlinkText->text().isEmpty() && QUrl(dlg.hyperlinkURL->text()).isValid() && !dlg.hyperlinkURL->text().isEmpty()) {
            dlg.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        }
        break;
    case 1:
        if (!dlg.bookmarkLinkText->text().isEmpty() && !dlg.bookmarkLinkURL->currentText().isEmpty() && exists(dlg.bookmarkLinkURL->currentText())) {
            dlg.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        }
        break;
    }
}

void LinkInsertionDialog::insertLink()
{
    if (dlg.linkTypesTab->currentIndex() == 0) {
        QString linkText = dlg.hyperlinkText->text();
        QString linkURL = dlg.hyperlinkURL->text();
        insertHyperlink(linkURL, linkText);
    } else {
        QString linkName = dlg.bookmarkLinkURL->currentText();
        QString linkText = dlg.bookmarkLinkText->text();
        insertBookmarkLink(linkName, linkText);
    }
}

void LinkInsertionDialog::displayInlineWarning(const QString &warning, QLabel *label) const
{
    label->setText(warning);
}

void LinkInsertionDialog::insertHyperlink(QString &linkURLString, const QString &linkText)
{
    QUrl linkURL = QUrl(linkURLString);
    dlg.weblinkStatusLabel->setText("");
    if (!linkURL.isValid()) {
        displayInlineWarning(i18n("The URL is invalid"), dlg.weblinkStatusLabel);
    } else {
        if ((linkURL.scheme()).isEmpty()) { // prepend a scheme if not present
            linkURLString.prepend("http://");
        }
        m_editor->insertText(linkText, linkURLString);
        this->close();
    }
}

void LinkInsertionDialog::insertBookmarkLink(const QString &linkURL, const QString &linkText)
{
    dlg.bookmarkLinkStatusLabel->setText("");
    m_editor->insertText(linkText, linkURL);
    this->close();
}

bool LinkInsertionDialog::exists(const QString &bookmarkName) const
{
    return m_bookmarkList.contains(bookmarkName);
}

void LinkInsertionDialog::fetchTitleFromURL()
{
    QString linkURLString = dlg.hyperlinkURL->text();
    m_linkURL = QUrl(linkURLString);
    if (m_linkURL.isValid()) {
        if ((m_linkURL.scheme()).isEmpty()) { // prepend a scheme if not present
            linkURLString.prepend("http://");
            dlg.hyperlinkURL->setText(linkURLString);
            m_linkURL.setUrl(linkURLString);
        }
        sendRequest();
    } else {
        displayInlineWarning(i18n("The URL is invalid"), dlg.weblinkStatusLabel);
        return;
    }
    // xgettext: no-c-format
    dlg.weblinkStatusLabel->setText(i18n("Fetching the title: 0% complete"));
}
void LinkInsertionDialog::sendRequest()
{
    QNetworkRequest request;
    request.setUrl(m_linkURL);
    m_reply = m_networkAccessManager->get(request);
    // start a timer to notify user when it takes too long to get the title
    if (m_timeoutTimer.isActive()) { // a timer for every redirection
        m_timeoutTimer.stop();
    }
    m_timeoutTimer.setInterval(FETCH_TIMEOUT);
    m_timeoutTimer.setSingleShot(true);
    m_timeoutTimer.start();
    connect(&m_timeoutTimer, &QTimer::timeout, this, &LinkInsertionDialog::fetchTitleTimeout);
    connect(m_reply, &QNetworkReply::finished, this, &LinkInsertionDialog::replyFinished);
    connect(m_reply, &QNetworkReply::errorOccurred, this, &LinkInsertionDialog::fetchTitleError);
    connect(m_reply, &QNetworkReply::downloadProgress, this, &LinkInsertionDialog::updateTitleDownloadProgress);
}

void LinkInsertionDialog::fetchTitleTimeout()
{
    if (!m_reply->isFinished()) {
        displayInlineWarning(i18n("Fetch timed out"), dlg.weblinkStatusLabel);
        m_reply->abort();
    }
}

void LinkInsertionDialog::replyFinished()
{
    // check for redirections
    QVariant possibleRedirectVariant = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QUrl possibleRedirectUrl = possibleRedirectVariant.toUrl();
    if (!possibleRedirectUrl.isEmpty() && m_linkURL != possibleRedirectUrl) { // redirect
        if (possibleRedirectUrl.toString().at(0) == '/') { // redirection to a relative url
            if (m_linkURL.toString().at(m_linkURL.toString().length() - 1) == '/') { // initially of the form http:xyz.com/
                possibleRedirectUrl.setUrl(m_linkURL.toString() + possibleRedirectUrl.toString().remove(0, 1));
            } else {
                possibleRedirectUrl.setUrl(m_linkURL.toString() + possibleRedirectUrl.toString());
            }
        }
        m_linkURL = possibleRedirectUrl;
        sendRequest();
        return;
    }
    const QString res = m_reply->readAll();
    static QRegularExpression titleStart("<title"); // title tag can have attributes, so can't search for <title>
    static QRegularExpression titleEnd("</title>");
    int start = res.indexOf(titleStart);
    if (start == -1) { // perhaps TITLE?, rare but possible
        start = res.indexOf(QRegularExpression("<TITLE"));
        if (start == -1) {
            displayInlineWarning("Error fetching title", dlg.weblinkStatusLabel);
            return;
        }
    }
    // now move to the end of the title
    while (res.at(start) != QChar('>')) {
        start++;
    }
    start++; // eat the '>'
    int end = res.indexOf(titleEnd);
    if (end == -1) {
        end = res.indexOf(QRegularExpression("</TITLE>"));
        if (end == -1) {
            displayInlineWarning("Error fetching title", dlg.weblinkStatusLabel);
            return;
        }
    }
    dlg.hyperlinkText->setText(QStringView(res).mid(start, end - start).toString());
    dlg.weblinkStatusLabel->setText("");
}

void LinkInsertionDialog::updateTitleDownloadProgress(qint64 received, qint64 total)
{
    float percentComplete = (static_cast<float>(received) / total) * 100;
    // xgettext: no-c-format
    dlg.weblinkStatusLabel->setText(i18n("Fetching the title: %1% complete", QString::number(percentComplete)));
}

LinkInsertionDialog::~LinkInsertionDialog()
{
    m_networkAccessManager->deleteLater();
}

void LinkInsertionDialog::fetchTitleError(QNetworkReply::NetworkError)
{
    m_timeoutTimer.stop();
    displayInlineWarning(i18n("The URL is invalid"), dlg.weblinkStatusLabel);
}

void LinkInsertionDialog::accept()
{
    // Overloaded to prevent the dialog from closing
}
