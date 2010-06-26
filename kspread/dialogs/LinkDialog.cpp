/* This file is part of the KDE project
   Copyright (C) 2005 Ariya Hidayat <ariya@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 1999 Stephan Kulow <coolo@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "LinkDialog.h"

#include "Cell.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Selection.h"
#include "Sheet.h"

#include <QLabel>
#include <QVBoxLayout>

#include <kcombobox.h>
#include <kdesktopfile.h>
#include <kpagedialog.h>
#include <kicon.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <krecentdocument.h>
#include <kurlrequester.h>
#include <kurlcompletion.h>

using namespace KSpread;

class LinkDialog::Private
{
public:
    QString text;
    QWidget* internetPage;
    KLineEdit* internetText;
    KLineEdit* internetLink;
    QWidget* mailPage;
    KLineEdit* mailText;
    KLineEdit* mailLink;
    KLineEdit* mailSubject;
    QWidget* filePage;
    KLineEdit* fileText;
    KUrlRequester* fileLink;
    QWidget* cellPage;
    KLineEdit* cellText;
    KComboBox* cellLink;
    KPageWidgetItem* p1, *p2, *p3, *p4;
};

LinkDialog::LinkDialog(QWidget* parent, Selection* selection)
        : KPageDialog(parent)
        , d(new Private)
{
    setCaption(i18n("Insert Link"));
    setButtons(Ok | Cancel);
    setFaceType(List);

    // link for web or ftp
    d->internetPage = new QWidget();
    d->p1 = addPage(d->internetPage, i18n("Internet"));
    d->p1->setHeader(i18n("Link to Internet Address"));
    d->p1->setIcon(KIcon("internet-web-browser"));
    QVBoxLayout* iLayout = new QVBoxLayout(d->internetPage);
    iLayout->addWidget(new QLabel(i18n("Text to display:"), d->internetPage));
    d->internetText = new KLineEdit(d->internetPage);
    iLayout->addWidget(d->internetText);
    iLayout->addWidget(new QLabel(i18n("Internet address:"), d->internetPage));
    d->internetLink = new KLineEdit(d->internetPage);
    iLayout->addWidget(d->internetLink);
    iLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    connect(d->internetText, SIGNAL(textChanged(const QString&)), this,
            SLOT(setText(const QString&)));

    // link for e-mail
    d->mailPage = new QWidget();
    d->p2 = addPage(d->mailPage, i18n("Mail"));
    d->p2->setHeader(i18n("Link to Mail Address"));
    d->p2->setIcon(KIcon("mail-message"));
    QVBoxLayout* mLayout = new QVBoxLayout(d->mailPage);
    mLayout->addWidget(new QLabel(i18n("Text to display:"), d->mailPage));
    d->mailText = new KLineEdit(d->mailPage);
    mLayout->addWidget(d->mailText);
    mLayout->addWidget(new QLabel(i18n("Email:"), d->mailPage));
    d->mailLink = new KLineEdit(d->mailPage);
    mLayout->addWidget(d->mailLink);
    connect(d->mailText, SIGNAL(textChanged(const QString&)), this,
            SLOT(setText(const QString&)));
    mLayout->addWidget(new QLabel(i18n("Subject:"), d->mailPage));
    d->mailSubject = new KLineEdit(d->mailPage);
    mLayout->addWidget(d->mailSubject);
    mLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // link for external file
    d->filePage = new QWidget();
    d->p3 = addPage(d->filePage, i18n("File"));
    d->p3->setHeader(i18n("Link to File"));
    d->p3->setIcon(KIcon("system-file-manager"));
    QVBoxLayout* fLayout = new QVBoxLayout(d->filePage);
    fLayout->addWidget(new QLabel(i18n("Text to display:"), d->filePage));
    d->fileText = new KLineEdit(d->filePage);
    fLayout->addWidget(d->fileText);
    fLayout->addWidget(new QLabel(i18n("File location:"), d->filePage));
    d->fileLink = new KUrlRequester(d->filePage);
    d->fileLink->completionObject()->setReplaceHome(true);
    d->fileLink->completionObject()->setReplaceEnv(true);
    fLayout->addWidget(d->fileLink);
    fLayout->addWidget(new QLabel(i18n("Recent file:"), d->filePage));
    KComboBox* recentFile = new KComboBox(d->filePage);
    recentFile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    fLayout->addWidget(recentFile);
    fLayout->addItem(new QSpacerItem(0, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
    connect(d->fileText, SIGNAL(textChanged(const QString&)), this,
            SLOT(setText(const QString&)));
    QObject::connect(recentFile, SIGNAL(highlighted(const QString &)),
                     d->fileLink->lineEdit(), SLOT(setText(const QString &)));

    // populate recent files
    int index = 0;
    const QStringList fileList = KRecentDocument::recentDocuments();
    for (QStringList::ConstIterator it = fileList.constBegin(); it != fileList.constEnd(); ++it) {
        KDesktopFile f(*it);
        if (!f.readUrl().isEmpty())
            recentFile->insertItem(index++, f.readUrl());
    }
    if (recentFile->count() == 0) {
        recentFile->insertItem(0, i18n("No Entries"));
        recentFile->setEnabled(false);
    }

    // link to another cell
    d->cellPage = new QWidget();
    d->p4 = addPage(d->cellPage, i18n("Cell"));
    d->p4->setHeader(i18n("Link to Cell"));
    d->p4->setIcon(KIcon("table"));
    QVBoxLayout* cLayout = new QVBoxLayout(d->cellPage);
    cLayout->addWidget(new QLabel(i18n("Text to display:"), d->cellPage));
    d->cellText = new KLineEdit(d->cellPage);
    cLayout->addWidget(d->cellText);
    cLayout->addWidget(new QLabel(i18n("Cell or Named Area:"), d->cellPage));
    d->cellLink = new KComboBox(d->cellPage);
    d->cellLink->setEditable(true);

    const Sheet *sheet = selection->activeSheet();
    if (sheet && selection) {
        Cell cell(sheet, selection->cursor());
        d->cellLink->addItem(cell.fullName());
    }

    const NamedAreaManager *manager = selection->activeSheet()->map()->namedAreaManager();
    d->cellLink->addItems(manager->areaNames());

    d->cellLink->setItemText(d->cellLink->currentIndex(), "");
    cLayout->addWidget(d->cellLink);
    cLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    connect(d->cellText, SIGNAL(textChanged(const QString&)), this,
            SLOT(setText(const QString&)));

    showButtonSeparator(true);
    d->internetText->setFocus();
    resize(400, 300);
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
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
    if (currentPage() == d->p1) {
        str = d->internetLink->text();
        if (!str.isEmpty())
            if (str.indexOf("http://") == -1)
                if (str.indexOf("https://") == -1)
                    if (str.indexOf("ftp://") == -1)
                        str.prepend("http://");
    } else if (currentPage() == d->p2) {
        str = d->mailLink->text();
        if (!str.isEmpty())
            if (str.indexOf("mailto:") == -1)
                str.prepend("mailto:");
        const QString subject = d->mailSubject->text().trimmed();
        if (! subject.isEmpty())
            str.append(QString("?subject=%1").arg(QString(QUrl::toPercentEncoding(subject))));
    } else if (currentPage() == d->p3) {
        KUrl url = d->fileLink->url();
        if (url.isValid()) {
            str = url.url();
        } else {
            str = d->fileText->text();
            if (!str.isEmpty())
                if (! str.contains(QRegExp("^(file|mailto|http|https|ftp):")))
                    str.prepend("file://");
        }
    } else if (currentPage() == d->p4) {
        str = d->cellLink->currentText();
    }

    return str;
}

void LinkDialog::setText(const QString& text)
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

// link must be complete, e.g. "http://www.koffice.org" instead of
// "www.koffice.org" only, since protocol is used to decide which page to show
void LinkDialog::setLink(const QString& link)
{
    if (link.startsWith("https://")) {
        d->internetLink->setText(link.mid(QString("https://").length()));
        setCurrentPage(d->p1);
        return;
    }

    if (link.startsWith("http://")) {
        d->internetLink->setText(link.mid(QString("http://").length()));
        setCurrentPage(d->p1);
        return;
    }

    if (link.startsWith("ftp://")) {
        d->internetLink->setText(link.mid(QString("ftp://").length()));
        setCurrentPage(d->p1);
        return;
    }

    if (link.startsWith("mailto:")) {
        QUrl url(link);
        if (url.isValid()) {
            d->mailLink->setText(url.toString(QUrl::RemoveScheme | QUrl::RemoveQuery));
            d->mailSubject->setText(url.queryItemValue("subject"));
        } else {
            d->mailLink->setText(link.mid(QString("mailto:").length()));
        }
        setCurrentPage(d->p2);
        return;
    }

    if (link.startsWith("file:/")) {
        QString s = link.mid(QString("file:/").length());
        while (s.startsWith("//")) s.remove(0, 1);
        d->fileLink->lineEdit()->setText(s);
        setCurrentPage(d->p3);
        return;
    }

    // assume cell reference
    d->cellLink->setItemText(d->cellLink->currentIndex(), link);
    setCurrentPage(d->p4);
}

void LinkDialog::slotOk()
{
    QString str;

    if (currentPage() == d->p1)
        str = i18n("Internet address is empty");
    else if (currentPage() == d->p2)
        str = i18n("Mail address is empty");
    else if (currentPage() == d->p3)
        str = i18n("File name is empty");
    else if (currentPage() == d->p4)
        i18n("Destination cell is empty");

    if (link().isEmpty()) {
        KMessageBox::error(this, str);
        return;
    }

    if (d->text.isEmpty())
        d->text = link();

    accept();
}

#include "LinkDialog.moc"
