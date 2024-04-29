/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "LinkDialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QUrlQuery>
#include <QVBoxLayout>

#include <KoIcon.h>

#include <KComboBox>
#include <KLocalizedString>
#include <KMessageBox>
#include <KPageWidget>
#include <kdesktopfile.h>
#include <klineedit.h>
#include <krecentdocument.h>
#include <kurlcompletion.h>
#include <kurlrequester.h>

using namespace Calligra::Sheets;

class LinkDialog::Private
{
public:
    QString text;
    KPageWidget *pages;
    QWidget *internetPage;
    QLineEdit *internetText;
    QLineEdit *internetLink;
    QWidget *mailPage;
    QLineEdit *mailText;
    QLineEdit *mailLink;
    QLineEdit *mailSubject;
    QWidget *filePage;
    QLineEdit *fileText;
    KUrlRequester *fileLink;
    QWidget *cellPage;
    QLineEdit *cellText;
    KComboBox *cellLink;
    KPageWidgetItem *p1, *p2, *p3, *p4;
};

LinkDialog::LinkDialog(QWidget *parent, const QList<QString> &links)
    : ActionDialog(parent)
    , d(new Private)
{
    setWindowTitle(i18n("Insert Link"));
    setButtonText(Apply, i18n("Insert Link"));

    d->pages = new KPageWidget();
    setMainWidget(d->pages);
    d->pages->setFaceType(KPageWidget::List);

    // link for web or ftp
    d->internetPage = new QWidget();
    d->p1 = d->pages->addPage(d->internetPage, i18n("Internet"));
    d->p1->setHeader(i18n("Link to Internet Address"));
    d->p1->setIcon(koIcon("internet-web-browser"));
    QVBoxLayout *iLayout = new QVBoxLayout(d->internetPage);
    iLayout->addWidget(new QLabel(i18n("Text to display:"), d->internetPage));
    d->internetText = new QLineEdit(d->internetPage);
    iLayout->addWidget(d->internetText);
    iLayout->addWidget(new QLabel(i18n("Internet address:"), d->internetPage));
    d->internetLink = new QLineEdit(d->internetPage);
    iLayout->addWidget(d->internetLink);
    iLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    connect(d->internetText, &QLineEdit::textChanged, this, &LinkDialog::setText);

    // link for e-mail
    d->mailPage = new QWidget();
    d->p2 = d->pages->addPage(d->mailPage, i18n("Email"));
    d->p2->setHeader(i18n("Link to Email Address"));
    d->p2->setIcon(koIcon("internet-mail"));
    QVBoxLayout *mLayout = new QVBoxLayout(d->mailPage);
    mLayout->addWidget(new QLabel(i18n("Text to display:"), d->mailPage));
    d->mailText = new QLineEdit(d->mailPage);
    mLayout->addWidget(d->mailText);
    mLayout->addWidget(new QLabel(i18n("Email address:"), d->mailPage));
    d->mailLink = new QLineEdit(d->mailPage);
    mLayout->addWidget(d->mailLink);
    connect(d->mailText, &QLineEdit::textChanged, this, &LinkDialog::setText);
    mLayout->addWidget(new QLabel(i18n("Subject:"), d->mailPage));
    d->mailSubject = new QLineEdit(d->mailPage);
    mLayout->addWidget(d->mailSubject);
    mLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // link for external file
    d->filePage = new QWidget();
    d->p3 = d->pages->addPage(d->filePage, i18n("File"));
    d->p3->setHeader(i18n("Link to File"));
    d->p3->setIcon(koIcon("system-file-manager"));
    QVBoxLayout *fLayout = new QVBoxLayout(d->filePage);
    fLayout->addWidget(new QLabel(i18n("Text to display:"), d->filePage));
    d->fileText = new QLineEdit(d->filePage);
    fLayout->addWidget(d->fileText);
    fLayout->addWidget(new QLabel(i18n("File location:"), d->filePage));
    d->fileLink = new KUrlRequester(d->filePage);
    d->fileLink->completionObject()->setReplaceHome(true);
    d->fileLink->completionObject()->setReplaceEnv(true);
    fLayout->addWidget(d->fileLink);
    fLayout->addWidget(new QLabel(i18n("Recent file:"), d->filePage));
    KComboBox *recentFile = new KComboBox(d->filePage);
    recentFile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    recentFile->setMinimumContentsLength(40);
    fLayout->addWidget(recentFile);
    fLayout->addItem(new QSpacerItem(0, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
    connect(d->fileText, &QLineEdit::textChanged, this, &LinkDialog::setText);
    connect(recentFile, &KComboBox::textHighlighted, d->fileLink->lineEdit(), &QLineEdit::setText);

    // populate recent files
    int index = 0;
    const auto fileList = KRecentDocument::recentUrls();
    for (const auto &url : fileList) {
        if (url.isLocalFile()) {
            KDesktopFile f(url.toLocalFile());
            if (!f.readUrl().isEmpty())
                recentFile->insertItem(index++, f.readUrl());
        }
    }
    if (recentFile->count() == 0) {
        recentFile->insertItem(0, i18n("No Entries"));
        recentFile->setEnabled(false);
    }

    // link to another cell
    d->cellPage = new QWidget();
    d->p4 = d->pages->addPage(d->cellPage, i18n("Cell"));
    d->p4->setHeader(i18n("Link to Cell"));
    d->p4->setIcon(koIcon("table"));
    QVBoxLayout *cLayout = new QVBoxLayout(d->cellPage);
    cLayout->addWidget(new QLabel(i18n("Text to display:"), d->cellPage));
    d->cellText = new QLineEdit(d->cellPage);
    cLayout->addWidget(d->cellText);
    cLayout->addWidget(new QLabel(i18n("Cell or Named Area:"), d->cellPage));
    d->cellLink = new KComboBox(d->cellPage);
    d->cellLink->setEditable(true);
    d->cellLink->addItems(links);

    cLayout->addWidget(d->cellLink);
    cLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    connect(d->cellText, &QLineEdit::textChanged, this, &LinkDialog::setText);

    d->internetText->setFocus();
    resize(400, 300);
}

LinkDialog::~LinkDialog()
{
    delete d;
}

QString LinkDialog::text() const
{
    return d->text;
}

QString LinkDialog::link() const
{
    QString str;
    if (d->pages->currentPage() == d->p1) {
        str = d->internetLink->text();
        if (!str.isEmpty())
            if (str.indexOf("http://") == -1)
                if (str.indexOf("https://") == -1)
                    if (str.indexOf("ftp://") == -1)
                        str.prepend("http://");
    } else if (d->pages->currentPage() == d->p2) {
        str = d->mailLink->text();
        if (!str.isEmpty())
            if (str.indexOf("mailto:") == -1)
                str.prepend("mailto:");
        const QString subject = d->mailSubject->text().trimmed();
        if (!subject.isEmpty())
            str.append(QString("?subject=%1").arg(QString(QUrl::toPercentEncoding(subject))));
    } else if (d->pages->currentPage() == d->p3) {
        QUrl url = d->fileLink->url();
        if (url.isValid()) {
            str = url.url();
        } else {
            str = d->fileText->text();
            if (!str.isEmpty())
                if (!str.contains(QRegularExpression("^(file|mailto|http|https|ftp):")))
                    str.prepend("file://");
        }
    } else if (d->pages->currentPage() == d->p4) {
        str = d->cellLink->currentText();
    }

    return str;
}

void LinkDialog::setText(const QString &text)
{
    d->text = text;

    d->internetText->blockSignals(true);
    d->internetText->setText(text);
    d->internetText->blockSignals(false);

    d->mailText->blockSignals(true);
    d->mailText->setText(text);
    d->mailText->blockSignals(false);

    d->fileText->blockSignals(true);
    d->fileText->setText(text);
    d->fileText->blockSignals(false);

    d->cellText->blockSignals(true);
    d->cellText->setText(text);
    d->cellText->blockSignals(false);
}

// link must be complete, e.g. "http://www.calligra.org" instead of
// "www.calligra.org" only, since protocol is used to decide which page to show
void LinkDialog::setLink(const QString &link)
{
    d->internetLink->clear();
    d->mailLink->clear();
    d->mailSubject->clear();
    d->fileLink->clear();
    d->cellLink->setCurrentText(QString());

    if (link.startsWith(QLatin1String("https://"))) {
        d->internetLink->setText(link.mid(8));
        d->pages->setCurrentPage(d->p1);
        return;
    }

    if (link.startsWith(QLatin1String("http://"))) {
        d->internetLink->setText(link.mid(7));
        d->pages->setCurrentPage(d->p1);
        return;
    }

    if (link.startsWith(QLatin1String("ftp://"))) {
        d->internetLink->setText(link.mid(6));
        d->pages->setCurrentPage(d->p1);
        return;
    }

    if (link.startsWith(QLatin1String("mailto:"))) {
        QUrl url(link);
        if (url.isValid()) {
            d->mailLink->setText(url.toString(QUrl::RemoveScheme | QUrl::RemoveQuery));
            d->mailSubject->setText(QUrlQuery(url).queryItemValue("subject"));
        } else {
            d->mailLink->setText(link.mid(7));
        }
        d->pages->setCurrentPage(d->p2);
        return;
    }

    if (link.startsWith(QLatin1String("file:/"))) {
        QString s = link.mid(6);
        while (s.startsWith(QLatin1String("//")))
            s.remove(0, 1);
        d->fileLink->lineEdit()->setText(s);
        d->pages->setCurrentPage(d->p3);
        return;
    }

    // assume cell reference
    d->cellLink->setCurrentText(link);
    d->pages->setCurrentPage(d->p4);
}

void LinkDialog::onApply()
{
    Q_EMIT applyLink(text(), link());
}
