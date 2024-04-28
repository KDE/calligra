/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "PageLayoutDialog.h"
#include "ui_PageLayoutSheetPage.h"

// Sheets
#include "core/DocBase.h"
#include "core/Sheet.h"
#include "engine/CellBase.h"
#include "engine/MapBase.h"
#include "engine/Util.h"

// commands
#include "../commands/PageLayoutCommand.h"

// Calligra
#include <KoUnit.h>

using namespace Calligra::Sheets;

class PageLayoutDialog::Private
{
public:
    Sheet *sheet;
    Ui::SheetPage sheetPage;

public:
    void setup();
};

void PageLayoutDialog::Private::setup()
{
    const PrintSettings *settings = sheet->printSettings();
    sheetPage.gridCheckBox->setChecked(settings->printGrid());
    sheetPage.commentCheckBox->setChecked(settings->printCommentIndicator());
    sheetPage.formulaCheckBox->setChecked(settings->printFormulaIndicator());
    sheetPage.chartsCheckBox->setChecked(settings->printCharts());
    sheetPage.objectsCheckBox->setChecked(settings->printObjects());
    sheetPage.drawingsCheckBox->setChecked(settings->printGraphics());
    sheetPage.zeroValuesCheckBox->setChecked(settings->printZeroValues());
    sheetPage.headersCheckBox->setChecked(settings->printHeaders());
    sheetPage.ltrButton->setChecked(settings->pageOrder() == PrintSettings::LeftToRight);
    sheetPage.horizontalCheckBox->setChecked(settings->centerHorizontally());
    sheetPage.verticalCheckBox->setChecked(settings->centerVertically());

    // Setup the repeated columns comboboxes.
    const QRect usedArea = sheet->usedArea();
    const QPair<int, int> repeatedColumns = settings->repeatedColumns();
    sheetPage.columnsCheckBox->setChecked(repeatedColumns.first && repeatedColumns.second);
    const int maxColumn = qMax(usedArea.width(), repeatedColumns.second);
    for (int col = 1; col <= maxColumn; ++col) {
        const QString number = CellBase::columnName(col);
        sheetPage.startColumnComboBox->addItem(number);
        sheetPage.endColumnComboBox->addItem(number, col);
    }
    int index = sheetPage.startColumnComboBox->findText(CellBase::columnName(repeatedColumns.first));
    if (index == -1)
        index = 0;
    sheetPage.startColumnComboBox->setCurrentIndex(index);
    index = sheetPage.endColumnComboBox->findText(CellBase::columnName(repeatedColumns.second));
    if (index == -1)
        index = 0;
    sheetPage.endColumnComboBox->setCurrentIndex(index);

    // Setup the repeated rows comboboxes.
    const QPair<int, int> repeatedRows = settings->repeatedRows();
    sheetPage.rowsCheckBox->setChecked(repeatedRows.first && repeatedRows.second);
    const int maxRow = qMax(usedArea.height(), repeatedRows.second);
    for (int row = 1; row <= maxRow; ++row) {
        const QString number = QString::number(row);
        sheetPage.startRowComboBox->addItem(number);
        sheetPage.endRowComboBox->addItem(number, row);
    }
    index = sheetPage.startRowComboBox->findText(QString::number(repeatedRows.first));
    if (index == -1)
        index = 0;
    sheetPage.startRowComboBox->setCurrentIndex(index);
    index = sheetPage.endRowComboBox->findText(QString::number(repeatedRows.second));
    if (index == -1)
        index = 0;
    sheetPage.endRowComboBox->setCurrentIndex(index);

    // Setup the fixed zoom comboboxes.
    QStringList zoomLevels;
    for (int zoomLevel = 25; zoomLevel <= 500; zoomLevel += 25) {
        zoomLevels.append(i18n("%1%", zoomLevel));
        if (qRound(settings->zoom() * 100) > zoomLevel && qRound(settings->zoom() * 100) < zoomLevel + 25) {
            zoomLevels.append(i18n("%1%", qRound(settings->zoom() * 100)));
        }
    }
    sheetPage.zoomComboBox->insertItems(0, zoomLevels);
    const QString zoomLevel = i18n("%1%", qRound(settings->zoom() * 100));
    sheetPage.zoomComboBox->setCurrentIndex(sheetPage.zoomComboBox->findText(zoomLevel));

    // Setup the page limits comboboxes.
    const QSize pageLimits = settings->pageLimits();
    const bool pageLimitsSet = pageLimits.width() > 0 || pageLimits.height() > 0;
    sheetPage.pageLimitsButton->setChecked(pageLimitsSet);
    QStringList limits;
    limits.append(i18n("No Limit"));
    for (int limit = 1; limit <= 20; ++limit)
        limits.append(QString("%1").arg(limit));
    sheetPage.horizontalComboBox->insertItems(0, limits);
    sheetPage.verticalComboBox->insertItems(0, limits);
    if (pageLimits.width() > 20) {
        sheetPage.horizontalComboBox->addItem(QString("%1").arg(pageLimits.width()));
        sheetPage.horizontalComboBox->setCurrentIndex(20);
    } else
        sheetPage.horizontalComboBox->setCurrentIndex(qMax(0, pageLimits.width()));
    if (pageLimits.height() > 20) {
        sheetPage.verticalComboBox->addItem(QString("%1").arg(pageLimits.height()));
        sheetPage.verticalComboBox->setCurrentIndex(20);
    } else
        sheetPage.verticalComboBox->setCurrentIndex(qMax(0, pageLimits.height()));
}

PageLayoutDialog::PageLayoutDialog(QWidget *parent, Sheet *sheet)
    : KoPageLayoutDialog(parent, sheet->printSettings()->pageLayout())
    , d(new Private)
{
    showPageSpread(false);
    setUnit(sheet->doc()->unit());

    QWidget *page = new QWidget(this);
    d->sheetPage.setupUi(page);
    addPage(page, i18n("Sheet"));

    connect(d->sheetPage.columnsCheckBox, &QAbstractButton::toggled, d->sheetPage.startColumnComboBox, &QWidget::setEnabled);
    connect(d->sheetPage.columnsCheckBox, &QAbstractButton::toggled, d->sheetPage.endColumnComboBox, &QWidget::setEnabled);
    connect(d->sheetPage.rowsCheckBox, &QAbstractButton::toggled, d->sheetPage.startRowComboBox, &QWidget::setEnabled);
    connect(d->sheetPage.rowsCheckBox, &QAbstractButton::toggled, d->sheetPage.endRowComboBox, &QWidget::setEnabled);
    connect(d->sheetPage.zoomButton, &QAbstractButton::toggled, d->sheetPage.zoomComboBox, &QWidget::setEnabled);
    connect(d->sheetPage.pageLimitsButton, &QAbstractButton::toggled, d->sheetPage.horizontalComboBox, &QWidget::setEnabled);
    connect(d->sheetPage.pageLimitsButton, &QAbstractButton::toggled, d->sheetPage.verticalComboBox, &QWidget::setEnabled);

    d->sheet = sheet;
    d->setup();
}

PageLayoutDialog::~PageLayoutDialog()
{
    delete d;
}

void PageLayoutDialog::accept()
{
    PrintSettings settings;
    settings.setPageLayout(pageLayout());
    settings.setPrintGrid(d->sheetPage.gridCheckBox->isChecked());
    settings.setPrintCommentIndicator(d->sheetPage.commentCheckBox->isChecked());
    settings.setPrintFormulaIndicator(d->sheetPage.formulaCheckBox->isChecked());
    settings.setPrintCharts(d->sheetPage.chartsCheckBox->isChecked());
    settings.setPrintGraphics(d->sheetPage.drawingsCheckBox->isChecked());
    settings.setPrintObjects(d->sheetPage.objectsCheckBox->isChecked());
    settings.setPrintZeroValues(d->sheetPage.zeroValuesCheckBox->isChecked());
    settings.setPrintHeaders(d->sheetPage.headersCheckBox->isChecked());
    settings.setPageOrder(d->sheetPage.ltrButton->isChecked() ? PrintSettings::LeftToRight : PrintSettings::TopToBottom);
    settings.setCenterHorizontally(d->sheetPage.horizontalCheckBox->isChecked());
    settings.setCenterVertically(d->sheetPage.verticalCheckBox->isChecked());

    // Set the repeated columns.
    if (d->sheetPage.columnsCheckBox->isChecked()) {
        // TODO Stefan: Check if width of repeated columns exceeds page width.
        const int startColumn = Util::decodeColumnLabelText(d->sheetPage.startColumnComboBox->currentText());
        const int endColumn = Util::decodeColumnLabelText(d->sheetPage.endColumnComboBox->currentText());
        settings.setRepeatedColumns(qMakePair(qMin(startColumn, endColumn), qMax(startColumn, endColumn)));
    } else
        settings.setRepeatedColumns(QPair<int, int>());

    // Set the repeated rows.
    if (d->sheetPage.rowsCheckBox->isChecked()) {
        // TODO Stefan: Check if height of repeated rows exceeds page height.
        const int startRow = d->sheetPage.startRowComboBox->currentText().toInt();
        const int endRow = d->sheetPage.endRowComboBox->currentText().toInt();
        settings.setRepeatedRows(qMakePair(qMin(startRow, endRow), qMax(startRow, endRow)));
    } else
        settings.setRepeatedRows(QPair<int, int>());

    bool isValid = false;
    settings.setZoom(0.01 * d->sheetPage.zoomComboBox->currentText().remove('%').toDouble(&isValid));
    if (!isValid)
        settings.setZoom(1.0);

    QSize pageLimits;
    if (d->sheetPage.pageLimitsButton->isChecked()) {
        pageLimits.setWidth(d->sheetPage.horizontalComboBox->currentText().toInt(&isValid));
        if (!isValid)
            pageLimits.setWidth(0);
        pageLimits.setHeight(d->sheetPage.verticalComboBox->currentText().toInt(&isValid));
        if (!isValid)
            pageLimits.setHeight(0);
    }
    settings.setPageLimits(pageLimits);

    if (applyToDocument()) {
        // Apply to all sheets.
        KUndo2Command *macroCommand = new KUndo2Command(kundo2_i18n("Set Page Layout"));
        const QList<SheetBase *> sheets = d->sheet->map()->sheetList();
        for (SheetBase *bsheet : sheets) {
            Sheet *sheet = dynamic_cast<Sheet *>(bsheet);
            PageLayoutCommand *command = new PageLayoutCommand(sheet, settings, macroCommand);
            Q_UNUSED(command);
        }
        d->sheet->doc()->addCommand(macroCommand);
    } else {
        PageLayoutCommand *command = new PageLayoutCommand(d->sheet, settings);
        d->sheet->doc()->addCommand(command);
    }

    KoPageLayoutDialog::accept();
}
