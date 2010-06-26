/* This file is part of the KDE project
   Copyright (C) 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2000-2005 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2001-2002 David Faure <faure@kde.org>
             (C) 2001 Werner Trobin <trobin@kde.org>
             (C) 2000 Bernd Johannes Wuebben <wuebben@kde.org>

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

// Local
#include "PreferenceDialog.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollBar>

#include <kcombobox.h>
#include <kconfig.h>
#include <kicon.h>
#include <kstatusbar.h>
#include <knuminput.h>
#include <kmessagebox.h>

#include <kcolorbutton.h>
#include <KPluginInfo>
#include <KPluginSelector>
#include <KServiceTypeTrader>
#include <ksharedconfig.h>
#include <KStandardDirs>
#include <sonnet/configwidget.h>

#include "ApplicationSettings.h"
#include "CalculationSettings.h"
#include "part/Doc.h"
#include "part/Factory.h"
#include "FunctionModuleRegistry.h"
#include "Localization.h"
#include "Map.h"
#include "Sheet.h"
#include "part/View.h"

#include "ui/Editors.h"

#include "ui_FileOptionsWidget.h"
#include "ui_InterfaceOptionsWidget.h"

using namespace KSpread;

class PreferenceDialog::Private
{
public:
    View* view;
    KPageWidgetItem* page2;
    KPageWidgetItem* page3;
    KPageWidgetItem* page4;
    KPageWidgetItem* pluginPage;

    // Interface Options
    Ui::InterfaceOptionsWidget interfaceOptions;
    MoveTo oldCursorMovement;
    MethodOfCalc oldFunction;
    KoUnit oldUnit;
    double oldIndentationStep;
    bool oldCaptureAllArrowKeys;
    QColor oldGridColor;
    QColor oldPageBorderColor;

    // Open/Save Options
    Ui::FileOptionsWidget fileOptions;
    int oldRecentFilesEntries;
    int oldAutoSaveDelay;
    bool oldCreateBackupFile;

    // Plugin Options
    KPluginSelector* pluginSelector;

    // Spellchecker Options
    Sonnet::ConfigWidget* spellCheckPage;

public:
    // Interface Options
    void applyInterfaceOptions();
    void defaultInterfaceOptions();
    void resetInterfaceOptions();

    // Open/Save Options
    void applyOpenSaveOptions();
    void defaultOpenSaveOptions();
    void resetOpenSaveOptions();
};

void PreferenceDialog::Private::applyInterfaceOptions()
{
    KSharedConfigPtr config = Factory::global().config();
    KConfigGroup parameterGroup = config->group("Parameters");

    oldCursorMovement = view->doc()->map()->settings()->moveToValue();
    oldFunction = view->doc()->map()->settings()->getTypeOfCalc();
    oldUnit = view->doc()->unit();

    const int cursorMovementIndex = interfaceOptions.m_cursorMovement->currentIndex();
    const MoveTo cursorMovement = (MoveTo)interfaceOptions.m_cursorMovement->itemData(cursorMovementIndex).toInt();
    if (cursorMovement != view->doc()->map()->settings()->moveToValue()) {
        view->doc()->map()->settings()->setMoveToValue(cursorMovement);
        parameterGroup.writeEntry("Move", (int)cursorMovement);
        oldCursorMovement = cursorMovement;
    }

    const int functionIndex = interfaceOptions.m_statusBarFunction->currentIndex();
    const MethodOfCalc function = (MethodOfCalc)interfaceOptions.m_statusBarFunction->itemData(functionIndex).toInt();
    if (function != view->doc()->map()->settings()->getTypeOfCalc()) {
        view->doc()->map()->settings()->setTypeOfCalc(function);
        parameterGroup.writeEntry("Method of Calc", (int)function);
        view->calcStatusBarOp();
        view->initCalcMenu();
        oldFunction = function;
    }

    const int unitIndex = interfaceOptions.m_unit->currentIndex();
    KoUnit unit((KoUnit::Unit)interfaceOptions.m_unit->itemData(unitIndex).toInt());
    if (unit != view->doc()->unit()) {
        view->doc()->setUnit(unit);
        parameterGroup.writeEntry("Unit", unit.indexInList());
        oldUnit = unit;
    }

    const double value = interfaceOptions.m_indentationStep->value();
    if (value != view->doc()->map()->settings()->indentValue()) {
        view->doc()->map()->settings()->setIndentValue(value);
        parameterGroup.writeEntry("Indent", unit.fromUserValue(value));
        oldIndentationStep = value;
    }

    const bool capture = interfaceOptions.m_captureAllArrowKeys->isChecked();
    if (capture != view->doc()->map()->settings()->captureAllArrowKeys()) {
        view->doc()->map()->settings()->setCaptureAllArrowKeys(capture);
        config->group("Editor").writeEntry("CaptureAllArrowKeys", capture);
        oldCaptureAllArrowKeys = capture;
    }

    const QColor gridColor = interfaceOptions.m_gridColor->color();
    if (gridColor != view->doc()->map()->settings()->gridColor()) {
        view->doc()->map()->settings()->setGridColor(gridColor);
        config->group("KSpread Color").writeEntry("GridColor", gridColor);
        oldGridColor = gridColor;
    }

    const QColor pageBorderColor = interfaceOptions.m_pageBorderColor->color();
    if (pageBorderColor != view->doc()->map()->settings()->pageBorderColor()) {
        view->doc()->map()->settings()->changePageBorderColor(pageBorderColor);
        config->group("KSpread Color").writeEntry("PageBorderColor", pageBorderColor);
        oldPageBorderColor = pageBorderColor;
    }

#if 0 // KSPREAD_COMPLETION_MODE_SETTING
    KGlobalSettings::Completion tmpCompletion = KGlobalSettings::CompletionNone;
    switch (typeCompletion->currentIndex()) {
    case 0:
        tmpCompletion = KGlobalSettings::CompletionNone;
        break;
    case 1:
        tmpCompletion = KGlobalSettings::CompletionShell;
        break;
    case 2:
        tmpCompletion = KGlobalSettings::CompletionPopup;
        break;
    case 3:
        tmpCompletion = KGlobalSettings::CompletionAuto;
        break;
    case 4:
        tmpCompletion = KGlobalSettings::CompletionMan;
        break;
    }


    if (comboChanged) {
        view->doc()->map()->settings()->setCompletionMode(tmpCompletion);
        parameterGroup.writeEntry("Completion Mode", (int)tmpCompletion);
    }
#endif
}

void PreferenceDialog::Private::defaultInterfaceOptions()
{
    interfaceOptions.m_cursorMovement->setCurrentIndex(0);
    interfaceOptions.m_statusBarFunction->setCurrentIndex(0);
    interfaceOptions.m_unit->setCurrentIndex(0);
    interfaceOptions.m_indentationStep->changeValue(10.0);
    interfaceOptions.m_captureAllArrowKeys->setChecked(true);
    interfaceOptions.m_gridColor->setColor(Qt::lightGray);
    interfaceOptions.m_pageBorderColor->setColor(Qt::red);
#if 0 // KSPREAD_COMPLETION_MODE_SETTING
    typeCompletion->setCurrentIndex(3);
#endif
}

void PreferenceDialog::Private::resetInterfaceOptions()
{
    KSharedConfigPtr config = Factory::global().config();
    const KConfigGroup parameterGroup = config->group("Parameters");

    oldCursorMovement = view->doc()->map()->settings()->moveToValue();
    oldFunction = view->doc()->map()->settings()->getTypeOfCalc();
    oldUnit = view->doc()->unit();
    oldIndentationStep = view->doc()->map()->settings()->indentValue();
    oldCaptureAllArrowKeys = view->doc()->map()->settings()->captureAllArrowKeys();

    const KConfigGroup colorGroup = config->group("KSpread Color");
    oldGridColor = colorGroup.readEntry("GridColor", QColor(Qt::lightGray));
    oldPageBorderColor = colorGroup.readEntry("PageBorderColor", QColor(Qt::red));

    const int moveToIndex = interfaceOptions.m_cursorMovement->findData(oldCursorMovement);
    interfaceOptions.m_cursorMovement->setCurrentIndex(moveToIndex);
    const int functionIndex = interfaceOptions.m_statusBarFunction->findData(oldFunction);
    interfaceOptions.m_statusBarFunction->setCurrentIndex(functionIndex);
    interfaceOptions.m_unit->setCurrentIndex(oldUnit.indexInList());
    interfaceOptions.m_indentationStep->changeValue(oldIndentationStep);
    interfaceOptions.m_captureAllArrowKeys->setChecked(oldCaptureAllArrowKeys);
    interfaceOptions.m_gridColor->setColor(oldGridColor);
    interfaceOptions.m_pageBorderColor->setColor(oldPageBorderColor);
}

void PreferenceDialog::Private::applyOpenSaveOptions()
{
    KSharedConfigPtr config = Factory::global().config();
    KConfigGroup parameterGroup = config->group("Parameters");
    Doc* doc = view->doc();

    int value = fileOptions.m_recentFilesEntries->value();
    if (value != oldRecentFilesEntries) {
        parameterGroup.writeEntry("NbRecentFile", value);
        view->changeNbOfRecentFiles(value);
        oldRecentFilesEntries = value;
    }

    value = fileOptions.m_autoSaveDelay->value();
    if (value != oldAutoSaveDelay) {
        parameterGroup.writeEntry("AutoSave", value);
        doc->setAutoSave(value * 60);
        oldAutoSaveDelay = value;
    }

    bool state = fileOptions.m_createBackupFile->isChecked();
    if (state != oldCreateBackupFile) {
        parameterGroup.writeEntry("BackupFile", state);
        doc->setBackupFile(state);
        oldCreateBackupFile = state;
    }
}

void PreferenceDialog::Private::defaultOpenSaveOptions()
{
    fileOptions.m_recentFilesEntries->setValue(10);
    fileOptions.m_autoSaveDelay->setValue(KoDocument::defaultAutoSave() / 60);
    fileOptions.m_createBackupFile->setChecked(true);
}

void PreferenceDialog::Private::resetOpenSaveOptions()
{
    KSharedConfigPtr config = Factory::global().config();
    const KConfigGroup parameterGroup = config->group("Parameters");

    oldCreateBackupFile = parameterGroup.readEntry("BackupFile", true);
    oldRecentFilesEntries = parameterGroup.readEntry("NbRecentFile", 10);
    oldAutoSaveDelay = parameterGroup.readEntry("AutoSave", KoDocument::defaultAutoSave() / 60);

    fileOptions.m_createBackupFile->setChecked(oldCreateBackupFile);
    fileOptions.m_recentFilesEntries->setValue(oldRecentFilesEntries);
    fileOptions.m_autoSaveDelay->setValue(oldAutoSaveDelay);
}



PreferenceDialog::PreferenceDialog(View* view)
        : KPageDialog(view)
        , d(new Private)
{
    setObjectName("PreferenceDialog");
    setCaption(i18n("Configure KSpread"));
    setFaceType(List);
    setButtons(Ok | Cancel | Default | Reset);
    setDefaultButton(Ok);

    d->view = view;

    connect(this, SIGNAL(okClicked()), this, SLOT(slotApply()));
    connect(this, SIGNAL(defaultClicked()), this, SLOT(slotDefault()));
    connect(this, SIGNAL(resetClicked()), this, SLOT(slotReset()));

    QWidget* widget = 0;
    KPageWidgetItem* page = 0;

    // Interface Options Widget
    widget = new QWidget(this);
    d->interfaceOptions.setupUi(widget);
    page = new KPageWidgetItem(widget, i18n("Interface"));
    page->setIcon(KIcon("preferences-desktop-theme"));
    addPage(page);
    d->page2 = page;

    d->interfaceOptions.m_cursorMovement->addItem(i18n("Down"), Bottom);
    d->interfaceOptions.m_cursorMovement->addItem(i18n("Up"), Top);
    d->interfaceOptions.m_cursorMovement->addItem(i18n("Right"), Right);
    d->interfaceOptions.m_cursorMovement->addItem(i18n("Left"), Left);
    d->interfaceOptions.m_cursorMovement->addItem(i18n("Down, First Column"), BottomFirst);
    d->interfaceOptions.m_cursorMovement->addItem(i18n("None"), NoMovement);

    d->interfaceOptions.m_statusBarFunction->addItem(i18n("Sum"), SumOfNumber);
    d->interfaceOptions.m_statusBarFunction->addItem(i18n("Min"), Min);
    d->interfaceOptions.m_statusBarFunction->addItem(i18n("Max"), Max);
    d->interfaceOptions.m_statusBarFunction->addItem(i18n("Average"), Average);
    d->interfaceOptions.m_statusBarFunction->addItem(i18n("Count"), Count);
    d->interfaceOptions.m_statusBarFunction->addItem(i18n("CountA"), CountA);
    d->interfaceOptions.m_statusBarFunction->addItem(i18n("None"), NoneCalc);

    KComboBox* unitComboBox = d->interfaceOptions.m_unit;
    unitComboBox->addItem(KoUnit::unitDescription(KoUnit(KoUnit::Millimeter)), KoUnit::Millimeter);
    unitComboBox->addItem(KoUnit::unitDescription(KoUnit(KoUnit::Point)), KoUnit::Point);
    unitComboBox->addItem(KoUnit::unitDescription(KoUnit(KoUnit::Inch)), KoUnit::Inch);
    unitComboBox->addItem(KoUnit::unitDescription(KoUnit(KoUnit::Centimeter)), KoUnit::Centimeter);
    unitComboBox->addItem(KoUnit::unitDescription(KoUnit(KoUnit::Decimeter)), KoUnit::Decimeter);
    unitComboBox->addItem(KoUnit::unitDescription(KoUnit(KoUnit::Pica)), KoUnit::Pica);
    unitComboBox->addItem(KoUnit::unitDescription(KoUnit(KoUnit::Cicero)), KoUnit::Cicero);
    unitComboBox->addItem(KoUnit::unitDescription(KoUnit(KoUnit::Pixel)), KoUnit::Pixel);
    connect(unitComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(unitChanged(int)));
    unitChanged(0);

    d->interfaceOptions.m_indentationStep->setMinMaxStep(0.0, 400.0, 10.0);

    d->resetInterfaceOptions(); // initialize values

    // Open/Save Options Widget
    widget = new QWidget(this);
    d->fileOptions.setupUi(widget);
    page = new KPageWidgetItem(widget, i18n("Open/Save"));
    page->setIcon(KIcon("document-save"));
    addPage(page);
    d->page3 = page;

    d->resetOpenSaveOptions(); // initialize values

    // Plugin Options Widget
    d->pluginSelector = new KPluginSelector(this);
    const QString serviceType = QLatin1String("KSpread/Plugin");
    const QString query = QLatin1String("([X-KSpread-InterfaceVersion] == 0)");
    const KService::List offers = KServiceTypeTrader::self()->query(serviceType, query);
    const QList<KPluginInfo> pluginInfoList = KPluginInfo::fromServices(offers);
    d->pluginSelector->addPlugins(pluginInfoList, KPluginSelector::ReadConfigFile,
                                  i18n("Function Modules"), "FunctionModule");
    d->pluginSelector->addPlugins(pluginInfoList, KPluginSelector::ReadConfigFile,
                                  i18n("Tools"), "Tool");
    d->pluginSelector->load();
    page = new KPageWidgetItem(d->pluginSelector, i18n("Plugins"));
    page->setIcon(KIcon("preferences-plugin"));
    addPage(page);
    d->pluginPage = page;

    // Spell Checker Options
    KSharedConfig::Ptr sharedConfigPtr = Factory::global().config();
    d->spellCheckPage = new Sonnet::ConfigWidget(sharedConfigPtr.data(), this);
    page = new KPageWidgetItem(d->spellCheckPage, i18n("Spelling"));
    page->setIcon(KIcon("tools-check-spelling"));
    page->setHeader(i18n("Spell Checker Behavior"));
    addPage(page);
    d->page4 = page;
}

PreferenceDialog::~PreferenceDialog()
{
    delete d;
}

void PreferenceDialog::openPage(int flags)
{
    if (flags & InterfacePage)
        setCurrentPage(d->page2);
    else if (flags & OpenSavePage)
        setCurrentPage(d->page3);
    else if (flags & SpellCheckerPage)
        setCurrentPage(d->page4);
    else if (flags & PluginPage)
        setCurrentPage(d->pluginPage);
}

void PreferenceDialog::slotApply()
{
    d->applyInterfaceOptions();
    d->applyOpenSaveOptions();

    // Plugin Options
    d->pluginSelector->save();
    FunctionModuleRegistry::instance()->loadFunctionModules();

    d->spellCheckPage->save();

    // The changes affect the document, not just a single view,
    // so all user interfaces have to be updated.
    d->view->doc()->updateAllViews();
}

void PreferenceDialog::slotDefault()
{
    if (currentPage() == d->page2) {
        d->defaultInterfaceOptions();
    } else if (currentPage() == d->page3) {
        d->defaultOpenSaveOptions();
    } else if (currentPage() == d->page4) {
        d->spellCheckPage->slotDefault();
    } else if (currentPage() == d->pluginPage) {
        d->pluginSelector->load();
    }
}

void PreferenceDialog::slotReset()
{
    if (currentPage() == d->page2) {
        d->resetInterfaceOptions();
    } else if (currentPage() == d->page3) {
        d->resetOpenSaveOptions();
    } else if (currentPage() == d->page4) {
        // TODO
    } else if (currentPage() == d->pluginPage) {
        d->pluginSelector->load(); // FIXME
    }
}

void PreferenceDialog::unitChanged(int index)
{
    KoUnit unit((KoUnit::Unit)d->interfaceOptions.m_unit->itemData(index).toInt());
    d->interfaceOptions.m_indentationStep->setUnit(unit);
}


#if 0 // KSPREAD_COMPLETION_MODE_SETTING
QLabel *label = new QLabel(i18n("&Completion mode:"), tmpQGroupBox);

typeCompletion = new KComboBox(tmpQGroupBox);
label->setBuddy(typeCompletion);
typeCompletion->setWhatsThis(i18n("Lets you choose the (auto) text completion mode from a range of options in the drop down selection box."));

listType += i18n("None");
listType += i18n("Manual");
listType += i18n("Popup");
listType += i18n("Automatic");
listType += i18n("Semi-Automatic");
typeCompletion->insertItems(0, listType);
typeCompletion->setCurrentIndex(0);
comboChanged = false;
connect(typeCompletion, SIGNAL(activated(const QString &)), this, SLOT(slotTextComboChanged(const QString &)));
#endif

#include "PreferenceDialog.moc"
