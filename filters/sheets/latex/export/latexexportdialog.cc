/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002, 2003 Robert JACOLIN <rjacolin@ifrance.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
   This file use code from koTemplateOpenDia for the method chooseSlot.
*/

#include "latexexportdialog.h"

#include <QDir>
#include <QApplication>
#include <QDBusConnection>
#include <QButtonGroup>

#include <KLocalizedString>
#include <KConfig>
#include <KoFilterManager.h>

#include "LatexDebug.h"
#include "latexexportAdaptor.h"

#include "document.h"

using namespace Qt::StringLiterals;

/*
 *  Constructs a LatexExportDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
LatexExportDialog::LatexExportDialog(KoStore* inputStore, QWidget* parent)
  : QDialog(parent)
  , m_inputStore(inputStore)
  , m_config(std::make_unique<KConfig>("kspreadlatexexportdialog"))
{
    m_ui.setupUi(this);

    int i = 0;

    QApplication::restoreOverrideCursor();

    /* Recent files */
    //m_config->setGroup( "KSpread latex export filter" );
    //QString value;
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

    connect(m_ui.addLanguageBtn, &QAbstractButton::clicked, this, &LatexExportDialog::addLanguage);
    connect(m_ui.rmLanguageBtn, &QAbstractButton::clicked, this, &LatexExportDialog::removeLanguage);

    m_ui._tab->tabBar()->setExpanding(true);
    m_ui.buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    m_ui.buttonBox->setProperty("_breeze_force_frame", true);
    m_ui.buttonBox->setContentsMargins(style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                                             style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                                             style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                                             style()->pixelMetric(QStyle::PM_LayoutBottomMargin));

    auto styleGroup = new QButtonGroup(this);
    styleGroup->addButton(m_ui.latexStyleButton);
    styleGroup->addButton(m_ui.wordsStyleButton);

    auto typeGroup = new QButtonGroup(this);
    typeGroup->addButton(m_ui.fullDocButton);
    typeGroup->addButton(m_ui.embededButton);

    m_ui.documentLayout->insertStretch(0);
    m_ui.documentLayout->insertStretch(2);

    auto okButton = m_ui.buttonBox->button(QDialogButtonBox::Ok);
    okButton->setText(i18nc("@action:button", "Export"));
    okButton->setIcon(QIcon::fromTheme(u"document-export-symbolic"_s));
    okButton->setDefault(true);
    setWindowTitle(i18n("Latex Export Filter Configuration"));

    connect(m_ui.buttonBox, &QDialogButtonBox::accepted, this, &LatexExportDialog::accept);
    connect(m_ui.buttonBox, &QDialogButtonBox::rejected, this, &LatexExportDialog::reject);
}

LatexExportDialog::~LatexExportDialog() = default;

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
