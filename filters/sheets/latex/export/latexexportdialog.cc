/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Robert JACOLIN <rjacolin@ifrance.com>

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
 * Boston, MA 02110-1301, USA.
   This file use code from koTemplateOpenDia for the method chooseSlot.
*/

#include "latexexportdialog.h"

#include <QDir>
#include <QApplication>
#include <QDBusConnection>

#include <KLocalizedString>
#include <kconfig.h>
#include <KoFilterManager.h>

#include "LatexDebug.h"
#include "latexexportAdaptor.h"

#include "document.h"

/*#ifdef __FreeBSD__
#include <unistd.h>
#endif*/

/*
 *  Constructs a LatexExportDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
LatexExportDialog::LatexExportDialog(KoStore* inputStore, QWidget* parent)
  : KoDialog(parent)
  , m_inputStore(inputStore)
{
    QWidget *mainWidget = new QWidget();
    m_ui.setupUi(mainWidget);

    int i = 0;

    QApplication::restoreOverrideCursor();

    /* Recent files */
    m_config = new KConfig("kspreadlatexexportdialog");
    //m_config->setGroup( "KSpread latex export filter" );
    QString value;
    while (i < 10) {
        /*value = m_config->readPathEntry( QString("Recent%1").arg(i), QString() );
        debugLatex <<"recent :" << value;
        if(!value.isEmpty())
        {
         _recentList.append( value );
         recentBox->addItem(value);
        }
        else
         i = 10;*/
        i = i + 1;
    }

    new LatexExportAdaptor(this);
    QDBusConnection::sessionBus().registerObject("/filter/latex", this);


    /* All these items inserted must not be translated so they are inserted here
     * without i18n() method. */
    /*qualityComboBox->addItem("final");
     qualityComboBox->addItem("draft");*/

    KComboBox* classComboBox = m_ui.classComboBox;
    classComboBox->addItem("article");
    classComboBox->addItem("book");
    classComboBox->addItem("letter");
    classComboBox->addItem("report");
    classComboBox->addItem("slides");

    KComboBox* encodingComboBox = m_ui.encodingComboBox;
    encodingComboBox->addItem("unicode");
    encodingComboBox->addItem("ansinew");
    encodingComboBox->addItem("applemac");
    encodingComboBox->addItem("ascii");
    encodingComboBox->addItem("latin1");
    encodingComboBox->addItem("latin2");
    encodingComboBox->addItem("latin3");
    encodingComboBox->addItem("latin5");
    encodingComboBox->addItem("cp437");
    encodingComboBox->addItem("cp437de");
    encodingComboBox->addItem("cp850");
    encodingComboBox->addItem("cp852");
    encodingComboBox->addItem("cp865");
    encodingComboBox->addItem("cp1250");
    encodingComboBox->addItem("cp1252");
    encodingComboBox->addItem("decmulti");
    encodingComboBox->addItem("next");

    QListWidget* languagesList = m_ui.languagesList;
    languagesList->addItem("american");
    languagesList->addItem("austrian");
    languagesList->addItem("bahasa");
    languagesList->addItem("brazil");
    languagesList->addItem("breton");
    languagesList->addItem("catalan");
    languagesList->addItem("croatian");
    languagesList->addItem("czech");
    languagesList->addItem("danish");
    languagesList->addItem("dutch");
    languagesList->addItem("english");
    languagesList->addItem("esperanto");
    languagesList->addItem("finnish");
    languagesList->addItem("francais");
    languagesList->addItem("french");
    languagesList->addItem("galician");
    languagesList->addItem("german");
    languagesList->addItem("germanb");
    languagesList->addItem("hungarian");
    languagesList->addItem("magyar");
    languagesList->addItem("italian");
    languagesList->addItem("norsk");
    languagesList->addItem("nynorsk");
    languagesList->addItem("polish");
    languagesList->addItem("portuges");
    languagesList->addItem("romanian");
    languagesList->addItem("russian");
    languagesList->addItem("spanish");
    languagesList->addItem("slovak");
    languagesList->addItem("slovene");
    languagesList->addItem("swedish");
    languagesList->addItem("turkish");

    connect(m_ui.addLanguageBtn, SIGNAL(clicked(bool)), SLOT(addLanguage()));
    connect(m_ui.rmLanguageBtn, SIGNAL(clicked(bool)), SLOT(removeLanguage()));

    setMainWidget(mainWidget);

    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    setModal(true);
    setCaption(i18n("Latex Export Filter Configuration"));
}

/*
 *  Destroys the object and frees any allocated resources
 */
LatexExportDialog::~LatexExportDialog()
{
    delete m_config;
}

/**
 * Called when the cancel button is clicked.
 * Close the dialog box.
 */
void LatexExportDialog::reject()
{
    debugLatex << "Export cancelled";
    QDialog::reject();
}

/**
 * Called when the user clicks on the ok button. The xslt sheet is put on the recent list which is
 * saved, then export the document.
 */
void LatexExportDialog::accept()
{
    hide();
    debugLatex << "KSPREAD LATEX EXPORT FILTER --> BEGIN";
    Config* config = Config::instance();

    /* Document tab */
    config->setEmbeded(m_ui.embededButton->isChecked());
    if (m_ui.wordsStyleButton->isChecked())
        config->useWordsStyle();
    else
        config-> useLatexStyle();
    /* class names are not translated */
    config->setClass(m_ui.classComboBox->currentText());

    if (m_ui.qualityComboBox->currentIndex() == 0)
        config->setQuality("final");
    else
        config->setQuality("draft");
    config->setDefaultFontSize(m_ui.defaultFontSize->value());

    /* Pictures tab */
    if (m_ui.pictureCheckBox->isChecked())
        config->convertPictures();
    config->setPicturesDir(m_ui.pathPictures->url().path());

    /* Language tab */
    config->setEncoding(m_ui.encodingComboBox->currentText());
    QListWidget* langUsedList = m_ui.langUsedList;
    for (int index = 0; index < langUsedList->count(); ++index) {
        debugLatex << "lang. :" << langUsedList->item(index)->text();
        config->addLanguage(langUsedList->item(index)->text());
    }

    /* The default language is the first language in the list */
    if (langUsedList->item(0) != nullptr)
        config->setDefaultLanguage(langUsedList->item(0)->text());
    if (langUsedList->currentItem() != nullptr) {
        const QString currentLanguage = langUsedList->currentItem()->text();
        debugLatex << "default lang. :" << currentLanguage;
        config->setDefaultLanguage(currentLanguage);
    }

    Document doc(m_inputStore, m_fileOut);
//     debugLatex << "---------- analyze file -------------";
    doc.analyze();
//     debugLatex << "---------- generate file -------------";
    doc.generate();
//     debugLatex << "KSPREAD LATEX EXPORT FILTER --> END";
}

void LatexExportDialog::addLanguage()
{
    QListWidgetItem* currentItem = m_ui.languagesList->takeItem(m_ui.languagesList->currentRow());
    if (! currentItem) {
        return;
    }
    const QString text = currentItem->text();
    debugLatex << "add a new supported language" << text;
    m_ui.langUsedList->addItem(text);
    delete currentItem;
}

void LatexExportDialog::removeLanguage()
{
    QListWidgetItem* currentItem = m_ui.langUsedList->takeItem(m_ui.langUsedList->currentRow());
    if (! currentItem) {
        return;
    }
    const QString text = currentItem->text();
    debugLatex << "remove a language" << text;
    m_ui.languagesList->addItem(text);
    delete currentItem;
}
