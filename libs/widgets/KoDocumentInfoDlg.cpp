/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
                 2006 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoDocumentInfoDlg.h"

#include "ui_koDocumentInfoAboutWidget.h"
#include "ui_koDocumentInfoAuthorWidget.h"

#include "KoDocumentBase.h"
#include "KoDocumentInfo.h"
#include "KoGlobal.h"
#include "KoPageWidgetItem.h"
#include <KoDocumentRdfBase.h>
#include <KoIcon.h>

#include <KIconLoader>
#include <KLocalizedString>

#include <KMessageBox>
#include <KoDialog.h>
#include <kmainwindow.h>

#include <QDateTime>
#include <QLineEdit>
#include <QMimeDatabase>
#include <QMimeType>
#include <QUrl>

// see KoIcon.h
#define koSmallIcon(name) (QIcon::fromTheme(name).pixmap(KIconLoader::StdSizes::SizeSmall))

class KoPageWidgetItemAdapter : public KPageWidgetItem
{
    Q_OBJECT
public:
    KoPageWidgetItemAdapter(KoPageWidgetItem *item)
        : KPageWidgetItem(item->widget(), item->name())
        , m_item(item)
    {
        setHeader(item->name());
        setIcon(QIcon::fromTheme(item->iconName()));
    }
    ~KoPageWidgetItemAdapter() override
    {
        delete m_item;
    }

    bool shouldDialogCloseBeVetoed()
    {
        return m_item->shouldDialogCloseBeVetoed();
    }
    void apply()
    {
        m_item->apply();
    }

private:
    KoPageWidgetItem *const m_item;
};

class KoDocumentInfoDlg::KoDocumentInfoDlgPrivate
{
public:
    KoDocumentInfoDlgPrivate() = default;
    ~KoDocumentInfoDlgPrivate() = default;

    KoDocumentInfo *info;
    QList<KPageWidgetItem *> pages;
    std::unique_ptr<Ui::KoDocumentInfoAboutWidget> aboutUi;
    std::unique_ptr<Ui::KoDocumentInfoAuthorWidget> authorUi;

    bool toggleEncryption = false;
    bool applyToggleEncryption = false;
    bool documentSaved = false;
};

KoDocumentInfoDlg::KoDocumentInfoDlg(QWidget *parent, KoDocumentInfo *docInfo)
    : KPageDialog(parent)
    , d(std::make_unique<KoDocumentInfoDlgPrivate>())
{
    d->info = docInfo;

    setWindowTitle(i18n("Document Information"));
    setFaceType(KPageDialog::Tabbed);
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    button(QDialogButtonBox::Ok)->setDefault(true);

    d->aboutUi = std::make_unique<Ui::KoDocumentInfoAboutWidget>();
    auto infodlg = new QWidget();
    d->aboutUi->setupUi(infodlg);
    d->aboutUi->lblEncryptedDesc->setVisible(false);
    d->aboutUi->lblEncrypted->setVisible(false);
    d->aboutUi->pbEncrypt->setVisible(false);
    d->aboutUi->lblEncryptedPic->setVisible(false);
    d->aboutUi->cbLanguage->addItems(KoGlobal::listOfLanguages());
    d->aboutUi->cbLanguage->setCurrentIndex(-1);

    auto page = new KPageWidgetItem(infodlg, i18n("General"));
    page->setHeader(i18n("General"));

    // Ugly hack, the mimetype should be a parameter, instead
    auto doc = dynamic_cast<KoDocumentBase *>(d->info->parent());
    if (!doc) {
        // hide all entries not used in pages for KoDocumentInfoPropsPage
        d->aboutUi->filePathInfoLabel->setVisible(false);
        d->aboutUi->filePathLabel->setVisible(false);
        d->aboutUi->filePathSeparatorLine->setVisible(false);
        d->aboutUi->lblTypeDesc->setVisible(false);
        d->aboutUi->lblType->setVisible(false);
    }
    addPage(page);
    d->pages.append(page);

    initAboutTab();

    d->authorUi = std::make_unique<Ui::KoDocumentInfoAuthorWidget>();
    auto authordlg = new QWidget();
    d->authorUi->setupUi(authordlg);
    d->authorUi->mainLayout->insertStretch(0);
    d->authorUi->mainLayout->insertStretch(2);
    page = new KPageWidgetItem(authordlg, i18n("Author"));
    page->setHeader(i18n("Last saved by"));
    addPage(page);
    d->pages.append(page);

    initAuthorTab();
}

KoDocumentInfoDlg::~KoDocumentInfoDlg() = default;

void KoDocumentInfoDlg::accept()
{
    // check if any pages veto the close
    for (KPageWidgetItem *item : std::as_const(d->pages)) {
        auto page = dynamic_cast<KoPageWidgetItemAdapter *>(item);
        if (page && page->shouldDialogCloseBeVetoed()) {
            return;
        }
    }

    // all fine, go and apply
    saveAboutData();
    for (KPageWidgetItem *item : std::as_const(d->pages)) {
        auto page = dynamic_cast<KoPageWidgetItemAdapter *>(item);
        if (page) {
            page->apply();
        }
    }

    KPageDialog::accept();
}

bool KoDocumentInfoDlg::isDocumentSaved()
{
    return d->documentSaved;
}

void KoDocumentInfoDlg::initAboutTab()
{
    auto doc = dynamic_cast<KoDocumentBase *>(d->info->parent());

    if (doc) {
        d->aboutUi->filePathLabel->setText(doc->localFilePath());
    }

    d->aboutUi->leTitle->setText(d->info->aboutInfo("title"));
    d->aboutUi->leSubject->setText(d->info->aboutInfo("subject"));
    QString language = KoGlobal::languageFromTag(d->info->aboutInfo("language"));
    d->aboutUi->cbLanguage->setCurrentIndex(d->aboutUi->cbLanguage->findText(language));

    d->aboutUi->leKeywords->setToolTip(i18n("Use ';' (Example: Office;KDE;Calligra)"));
    if (!d->info->aboutInfo("keyword").isEmpty()) {
        d->aboutUi->leKeywords->setText(d->info->aboutInfo("keyword"));
    }

    d->aboutUi->meComments->setPlainText(d->info->aboutInfo("description"));
    if (doc && !doc->mimeType().isEmpty()) {
        QMimeDatabase db;
        QMimeType docmime = db.mimeTypeForName(doc->mimeType());
        if (docmime.isValid()) {
            d->aboutUi->lblType->setText(docmime.comment());
        }
    }
    if (!d->info->aboutInfo("creation-date").isEmpty()) {
        QDateTime t = QDateTime::fromString(d->info->aboutInfo("creation-date"), Qt::ISODate);
        QString s = QLocale().toString(t);
        d->aboutUi->lblCreated->setText(s + ", " + d->info->aboutInfo("initial-creator"));
    }

    if (!d->info->aboutInfo("date").isEmpty()) {
        QDateTime t = QDateTime::fromString(d->info->aboutInfo("date"), Qt::ISODate);
        QString s = QLocale().toString(t);
        d->aboutUi->lblModified->setText(s + ", " + d->info->authorInfo("creator"));
    }

    d->aboutUi->lblRevision->setText(d->info->aboutInfo("editing-cycles"));

    if (doc && (doc->supportedSpecialFormats() & KoDocumentBase::SaveEncrypted)) {
        if (doc->specialOutputFlag() == KoDocumentBase::SaveEncrypted) {
            if (d->toggleEncryption) {
                d->aboutUi->lblEncrypted->setText(i18n("This document will be decrypted"));
                d->aboutUi->lblEncryptedPic->setPixmap(koSmallIcon("object-unlocked"));
                d->aboutUi->pbEncrypt->setText(i18n("Do not decrypt"));
            } else {
                d->aboutUi->lblEncrypted->setText(i18n("This document is encrypted"));
                d->aboutUi->lblEncryptedPic->setPixmap(koSmallIcon("object-locked"));
                d->aboutUi->pbEncrypt->setText(i18n("D&ecrypt"));
            }
        } else {
            if (d->toggleEncryption) {
                d->aboutUi->lblEncrypted->setText(i18n("This document will be encrypted."));
                d->aboutUi->lblEncryptedPic->setPixmap(koSmallIcon("object-locked"));
                d->aboutUi->pbEncrypt->setText(i18n("Do not encrypt"));
            } else {
                d->aboutUi->lblEncrypted->setText(i18n("This document is not encrypted"));
                d->aboutUi->lblEncryptedPic->setPixmap(koSmallIcon("object-unlocked"));
                d->aboutUi->pbEncrypt->setText(i18n("&Encrypt"));
            }
        }
    } else {
        d->aboutUi->lblEncrypted->setText(i18n("This document does not support encryption"));
        d->aboutUi->pbEncrypt->setEnabled(false);
    }
    connect(d->aboutUi->pbReset, &QAbstractButton::clicked, this, &KoDocumentInfoDlg::slotResetMetaData);
    connect(d->aboutUi->pbEncrypt, &QAbstractButton::clicked, this, &KoDocumentInfoDlg::slotToggleEncryption);
}

void KoDocumentInfoDlg::initAuthorTab()
{
    auto setText = [](QLabel *label, const QString &value) {
        label->setText(!value.isEmpty() ? value : i18nc("@info:placeholder", "Not set"));
    };
    setText(d->authorUi->fullName, d->info->authorInfo("creator"));
    setText(d->authorUi->initials, d->info->authorInfo("initial"));
    setText(d->authorUi->title, d->info->authorInfo("author-title"));
    setText(d->authorUi->company, d->info->authorInfo("company"));
    setText(d->authorUi->email, d->info->authorInfo("email"));
    setText(d->authorUi->phoneWork, d->info->authorInfo("telephone-work"));
    setText(d->authorUi->phoneHome, d->info->authorInfo("telephone"));
    setText(d->authorUi->fax, d->info->authorInfo("fax"));
    setText(d->authorUi->country, d->info->authorInfo("country"));
    setText(d->authorUi->postal, d->info->authorInfo("postal-code"));
    setText(d->authorUi->city, d->info->authorInfo("city"));
    setText(d->authorUi->street, d->info->authorInfo("street"));
    setText(d->authorUi->position, d->info->authorInfo("position"));
}

void KoDocumentInfoDlg::saveAboutData()
{
    d->info->setAboutInfo("keyword", d->aboutUi->leKeywords->text());
    d->info->setAboutInfo("title", d->aboutUi->leTitle->text());
    d->info->setAboutInfo("subject", d->aboutUi->leSubject->text());
    d->info->setAboutInfo("description", d->aboutUi->meComments->toPlainText());
    d->info->setAboutInfo("language", KoGlobal::tagOfLanguage(d->aboutUi->cbLanguage->currentText()));
    d->applyToggleEncryption = d->toggleEncryption;
}

void KoDocumentInfoDlg::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);

    // Saving encryption implies saving the document, this is done after closing the dialog
    // TODO: shouldn't this be skipped if cancel is pressed?
    saveEncryption();
}

void KoDocumentInfoDlg::slotResetMetaData()
{
    d->info->resetMetaData();

    if (!d->info->aboutInfo("creation-date").isEmpty()) {
        QDateTime t = QDateTime::fromString(d->info->aboutInfo("creation-date"), Qt::ISODate);
        QString s = QLocale().toString(t);
        d->aboutUi->lblCreated->setText(s + i18nc("list separator", ", ") + d->info->aboutInfo("initial-creator"));
    }

    if (!d->info->aboutInfo("date").isEmpty()) {
        QDateTime t = QDateTime::fromString(d->info->aboutInfo("date"), Qt::ISODate);
        QString s = QLocale().toString(t);
        d->aboutUi->lblModified->setText(s + i18nc("list separator", ", ") + d->info->authorInfo("creator"));
    }

    d->aboutUi->lblRevision->setText(d->info->aboutInfo("editing-cycles"));
}

void KoDocumentInfoDlg::slotToggleEncryption()
{
    auto doc = dynamic_cast<KoDocumentBase *>(d->info->parent());
    if (!doc) {
        return;
    }

    d->toggleEncryption = !d->toggleEncryption;

    if (doc->specialOutputFlag() == KoDocumentBase::SaveEncrypted) {
        if (d->toggleEncryption) {
            d->aboutUi->lblEncrypted->setText(i18n("This document will be decrypted"));
            d->aboutUi->lblEncryptedPic->setPixmap(koSmallIcon("object-unlocked"));
            d->aboutUi->pbEncrypt->setText(i18n("Do not decrypt"));
        } else {
            d->aboutUi->lblEncrypted->setText(i18n("This document is encrypted"));
            d->aboutUi->lblEncryptedPic->setPixmap(koSmallIcon("object-locked"));
            d->aboutUi->pbEncrypt->setText(i18n("D&ecrypt"));
        }
    } else if (d->toggleEncryption) {
        d->aboutUi->lblEncrypted->setText(i18n("This document will be encrypted."));
        d->aboutUi->lblEncryptedPic->setPixmap(koSmallIcon("object-locked"));
        d->aboutUi->pbEncrypt->setText(i18n("Do not encrypt"));
    } else {
        d->aboutUi->lblEncrypted->setText(i18n("This document is not encrypted"));
        d->aboutUi->lblEncryptedPic->setPixmap(koSmallIcon("object-unlocked"));
        d->aboutUi->pbEncrypt->setText(i18n("&Encrypt"));
    }
}

void KoDocumentInfoDlg::saveEncryption()
{
    if (!d->applyToggleEncryption) {
        return;
    }

    auto doc = dynamic_cast<KoDocumentBase *>(d->info->parent());
    if (!doc) {
        return;
    }

    auto mainWindow = dynamic_cast<KMainWindow *>(parent());

    bool saveas = doc->url().isEmpty();
    if (doc->specialOutputFlag() == KoDocumentBase::SaveEncrypted) {
        // Decrypt
        if (KMessageBox::warningContinueCancel(this,
                                               i18n("<qt>Decrypting the document will remove the password protection from it."
                                                    "<p>Do you still want to decrypt the file?</qt>"),
                                               i18n("Confirm Decrypt"),
                                               KGuiItem(i18n("Decrypt")),
                                               KStandardGuiItem::cancel(),
                                               "DecryptConfirmation")
            != KMessageBox::Continue) {
            return;
        }
        doc->setOutputMimeType(doc->outputMimeType(), doc->specialOutputFlag() & ~KoDocumentBase::SaveEncrypted);
        if (!mainWindow) {
            KMessageBox::information(this,
                                     i18n("<qt>Your document could not be saved automatically."
                                          "<p>To complete the decryption, please save the document.</qt>"),
                                     i18n("Save Document"),
                                     "DecryptSaveMessage");
            return;
        }
        if (KMessageBox::questionTwoActions(this,
                                            i18n("<qt>To complete the decryption the document needs to be saved."
                                                 "<p>Do you want to save the document now?</qt>"),
                                            i18n("Save Document"),
                                            KStandardGuiItem::save(),
                                            KStandardGuiItem::dontSave(),
                                            "DecryptSaveConfirmation")
            != KMessageBox::PrimaryAction) {
            return;
        }
        // Force saveas so user can decide if overwrite the encrypted file or not
        saveas = true;
    } else {
        // Encrypt
        if (!doc->url().isEmpty() && (!doc->mimeType().startsWith("application/vnd.oasis.opendocument.") || doc->specialOutputFlag() != 0)) {
            QMimeDatabase db;
            QMimeType mime = db.mimeTypeForName(doc->mimeType());
            QString comment = mime.isValid() ? mime.comment() : i18n("%1 (unknown file type)", QString::fromLatin1(doc->mimeType()));
            if (KMessageBox::warningContinueCancel(
                    this,
                    i18n("<qt>The document is currently saved as %1. The document needs to be changed to <b>OASIS OpenDocument</b> to be encrypted."
                         "<p>Do you want to change the file to OASIS OpenDocument?</qt>",
                         QString("<b>%1</b>").arg(comment)),
                    i18n("Change Filetype"),
                    KGuiItem(i18n("Change")),
                    KStandardGuiItem::cancel(),
                    "EncryptChangeFiletypeConfirmation")
                != KMessageBox::Continue) {
                return;
            }
            doc->resetURL();
        }
        doc->setMimeType(doc->nativeOasisMimeType());
        doc->setOutputMimeType(doc->nativeOasisMimeType(), KoDocumentBase::SaveEncrypted);
        if (!mainWindow) {
            KMessageBox::information(this,
                                     i18n("<qt>Your document could not be saved automatically."
                                          "<p>To complete the encryption, please save the document.</qt>"),
                                     i18n("Save Document"),
                                     "EncryptSaveMessage");
            return;
        }
        if (KMessageBox::questionTwoActions(
                this,
                i18n("<qt>The document has been changed since it was opened. To complete the encryption the document needs to be saved."
                     "<p>Do you want to save the document now?</qt>"),
                i18n("Save Document"),
                KStandardGuiItem::save(),
                KStandardGuiItem::dontSave(),
                "EncryptSaveConfirmation")
            != KMessageBox::PrimaryAction) {
            return;
        }
        saveas = true; // Force saveas because save will silently fail when doc is not modified
    }
    // Why do the dirty work ourselves?
    Q_EMIT saveRequested(saveas, false, doc->specialOutputFlag());
    d->toggleEncryption = false;
    d->applyToggleEncryption = false;
    // Detects when the user cancelled saving
    d->documentSaved = !doc->url().isEmpty();
}

QList<KPageWidgetItem *> KoDocumentInfoDlg::pages() const
{
    return d->pages;
}

void KoDocumentInfoDlg::setReadOnly(bool ro)
{
    d->aboutUi->meComments->setReadOnly(ro);

    for (KPageWidgetItem *page : std::as_const(d->pages)) {
        const auto lineEdits = page->widget()->findChildren<QLineEdit *>();
        for (auto lineEdit : lineEdits) {
            lineEdit->setReadOnly(ro);
        }
        const auto pushButtons = page->widget()->findChildren<QPushButton *>();
        for (auto pushButton : pushButtons) {
            pushButton->setDisabled(ro);
        }
    }
}

void KoDocumentInfoDlg::addPageItem(KoPageWidgetItem *item)
{
    KPageWidgetItem *page = new KoPageWidgetItemAdapter(item);

    addPage(page);
    d->pages.append(page);
}

#include "KoDocumentInfoDlg.moc"
