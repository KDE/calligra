/* This file is part of the KDE project
 * Copyright (C) 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "InsertCitationDialog.h"

#include <BibliographyDb.h>

#include <KAction>
#include <KDebug>
#include <KoInlineCite.h>
#include <KoInlineTextObjectManager.h>
#include <KoTextDocument.h>

#include <QMessageBox>

InsertCitationDialog::InsertCitationDialog(KoTextEditor *editor ,QWidget *parent) :
    QDialog(parent),
    m_blockSignals(false),
    m_editor(editor),
    m_table(0),
    m_biblio(new BibliographyDb(this, QDir::home().absolutePath().append(QDir::separator()).append(".calligra"), "biblio.kexi")),
    m_mode(InsertCitationDialog::Default)
{
    Q_ASSERT(m_editor);

    init();
}

InsertCitationDialog::InsertCitationDialog(BibliographyDb *db, QWidget *parent) :
    QDialog(parent),
    m_blockSignals(false),
    m_editor(0),
    m_table(db),
    m_biblio(new BibliographyDb(this, QDir::home().absolutePath().append(QDir::separator()).append(".calligra"), "biblio.kexi")),
    m_mode(InsertCitationDialog::DB)
{
    Q_ASSERT(m_table);

    init();
}

void InsertCitationDialog::init()
{
    dialog.setupUi(this);
    connect(dialog.buttonBox,SIGNAL(accepted()),this,SLOT(insert()));
    connect(dialog.existingCites,SIGNAL(currentIndexChanged(QString)),this,SLOT(selectionChangedFromExistingCites()));
    connect(dialog.databaseCites,SIGNAL(currentIndexChanged(QString)),this,SLOT(selectionChangedFromDatabaseCites()));

    dialog.fromDocument->setChecked(true);

    m_cites = citations();
    QStringList citeList(m_cites.keys());
    citeList.prepend(i18n("Select"));
    citeList.removeDuplicates();
    dialog.existingCites->addItems(citeList);

    m_records = m_biblio->citationRecords();
    citeList = QStringList(m_records.keys());
    citeList.prepend(i18n("Select"));
    citeList.removeDuplicates();
    dialog.databaseCites->addItems(citeList);

    show();
}

void InsertCitationDialog::insert()
{
    if (dialog.shortName->text().isEmpty()) {
        dialog.shortName->setText(QString(i18n("Short name%1")).arg(QString::number(this->citations().count())));

        dialog.shortName->setSelection(dialog.shortName->text().length(),0);
    }

    if (m_mode == InsertCitationDialog::Default) {
        if (m_cites.contains(dialog.shortName->text())
                && *(m_cites.value(dialog.shortName->text())) != *(toCite())) {
            //prompts if values are changed
            int ret = QMessageBox::warning(this,i18n("Warning"),i18n("The document already contains the bibliography entry with different data.\n"
                                 "Do you want to adjust existing entries?"), QMessageBox::Yes | QMessageBox::No);

            if (ret == QMessageBox::Yes) {
                foreach(KoInlineCite *existingCite, m_cites.values(dialog.shortName->text())) {     //update all cites with new values
                    existingCite->setFields(toCite()->fieldMap());
                    existingCite->setType(KoInlineCite::ClonedCitation);    //change type to ClonedCitation
                }
                KoInlineCite *cite = m_editor->insertCitation();
                cite->setFields(toCite()->fieldMap());
                emit accept();
            } else {
                return;
            }
        } else {
            KoInlineCite *cite = m_editor->insertCitation();
            cite->setFields(toCite()->fieldMap());
        }
    }
    if (m_mode == InsertCitationDialog::DB && !m_table->insertCitation(toCite())) {
        QMessageBox::critical(this, i18n("Error"), QString(i18n("Unable to insert citation record to database. "))
                              .append(m_table->lastError()), QMessageBox::Ok);

        return;
    }
    emit accept();
}

void InsertCitationDialog::selectionChangedFromExistingCites()
{
    if (dialog.existingCites->currentIndex() != 0) {
        KoInlineCite *cite = m_cites[dialog.existingCites->currentText()];
        this->fillValuesFrom(cite);
    } else if (dialog.existingCites->currentIndex() == 0) {
        KoInlineCite *blankCite = new KoInlineCite(KoInlineCite::Citation);
        blankCite->setField("bibliography-type", "Article");      //default bibliography type
        blankCite->setField("identifier", QString(i18n("Short name%1")).arg(QString::number(this->citations().count()+1)));
        fillValuesFrom(blankCite);
    }
}

void InsertCitationDialog::selectionChangedFromDatabaseCites()
{
    if (dialog.databaseCites->currentIndex() != 0) {
        KoInlineCite *cite = m_records[dialog.databaseCites->currentText()];
        this->fillValuesFrom(cite);
    } else if (dialog.databaseCites->currentIndex() == 0) {
        KoInlineCite *blankCite = new KoInlineCite(KoInlineCite::Citation);
        blankCite->setField("bibliography-type", "Article");      //default bibliography type
        blankCite->setField("identifier", QString(i18n("Short name%1")).arg(QString::number(this->citations().count()+1)));
        fillValuesFrom(blankCite);
    }
}

QMap<QString, KoInlineCite*> InsertCitationDialog::citations()
{
    if (m_editor) {
        return KoTextDocument(m_editor->document()).inlineTextObjectManager()->citations();
    } else {
        return m_table->citationRecords();
    }
}

KoInlineCite *InsertCitationDialog::toCite()
{
    KoInlineCite *cite = new KoInlineCite(KoInlineCite::Citation);
    cite->setField("address", dialog.address->text());
    cite->setField("annote", dialog.annotation->text());
    cite->setField("author", dialog.author->text());
    cite->setField("bibliography-type", dialog.sourceType->currentText().remove(' ').toLower());      //removing spaces and lowering case for exact tag attribute of bibliography-type
    cite->setField("booktitle", dialog.booktitle->text());
    cite->setField("chapter", dialog.chapter->text());
    cite->setField("custom1", dialog.ud1->text());
    cite->setField("custom2", dialog.ud2->text());
    cite->setField("custom3", dialog.ud3->text());
    cite->setField("custom4", dialog.ud4->text());
    cite->setField("custom5", dialog.ud5->text());
    cite->setField("edition", dialog.edition->text());
    cite->setField("editor", dialog.editor->text());
    cite->setField("howpublished", dialog.publication->text());
    cite->setField("identifier", dialog.shortName->text());
    cite->setField("institution", dialog.institution->text());
    cite->setField("isbn", dialog.isbn->text());
    cite->setField("issn", dialog.issn->text());
    cite->setField("journal", dialog.journal->text());
    cite->setField("month", dialog.month->text());
    cite->setField("note", dialog.note->text());
    cite->setField("number", dialog.number->text());
    cite->setField("organizations", dialog.organisation->text());
    cite->setField("pages", dialog.pages->text());
    cite->setField("publisher", dialog.publisher->text());
    cite->setField("report-type", dialog.reporttype->text());
    cite->setField("school", dialog.school->text());
    cite->setField("series", dialog.series->text());
    cite->setField("title", dialog.title->text());
    cite->setField("url", dialog.url->text());
    cite->setField("volume", dialog.volume->text());
    cite->setField("year", dialog.year->text());
    return cite;
}

void InsertCitationDialog::fillValuesFrom(KoInlineCite *cite)
{
    dialog.address->setText(cite->value("address"));
    dialog.annotation->setText(cite->value("annote"));
    dialog.author->setText(cite->value("author"));
    dialog.sourceType->setCurrentIndex(dialog.sourceType->findText(cite->value("bibliography-type"), Qt::MatchFixedString));
    dialog.booktitle->setText(cite->value("booktitle"));
    dialog.chapter->setText(cite->value("chapter"));
    dialog.ud1->setText(cite->value("custom1"));
    dialog.ud2->setText(cite->value("custom2"));
    dialog.ud3->setText(cite->value("custom3"));
    dialog.ud4->setText(cite->value("custom4"));
    dialog.ud5->setText(cite->value("custom5"));
    dialog.edition->setText(cite->value("edition"));
    dialog.editor->setText(cite->value("editor"));
    dialog.institution->setText(cite->value("institution"));
    dialog.shortName->setText(cite->value("identifier"));
    dialog.isbn->setText(cite->value("isbn"));
    dialog.issn->setText(cite->value("issn"));
    dialog.publication->setText(cite->value("howpublished"));
    dialog.journal->setText(cite->value("journal"));
    dialog.month->setText(cite->value("month"));
    dialog.note->setText(cite->value("note"));
    dialog.number->setText(cite->value("number"));
    dialog.organisation->setText(cite->value("organizations"));
    dialog.pages->setText(cite->value("pages"));
    dialog.publisher->setText(cite->value("publisher"));
    dialog.school->setText(cite->value("school"));
    dialog.series->setText(cite->value("series"));
    dialog.title->setText(cite->value("title"));
    dialog.reporttype->setText(cite->value("report-type"));
    dialog.volume->setText(cite->value("volume"));
    dialog.year->setText(cite->value("year"));
    dialog.url->setText(cite->value("url"));
}
