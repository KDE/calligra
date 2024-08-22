/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChangeConfigureDialog.h"
#include <QColorDialog>
#include <QPainter>

ColorDisplayLabel::ColorDisplayLabel(QWidget *parent)
    : QLabel(parent)
    , labelColor(255, 255, 0)
{
}

ColorDisplayLabel::~ColorDisplayLabel() = default;

void ColorDisplayLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setBrush(labelColor);
    painter.drawRect(rect().x(), rect().y(), rect().width(), rect().height());
}

const QColor &ColorDisplayLabel::color() const
{
    return labelColor;
}

void ColorDisplayLabel::setColor(const QColor &color)
{
    labelColor = color;
}

ChangeConfigureDialog::ChangeConfigureDialog(const QColor &insertionColor,
                                             const QColor &deletionColor,
                                             const QColor &formatChangeColor,
                                             const QString &authorName,
                                             KoChangeTracker::ChangeSaveFormat changeSaveFormat,
                                             QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.insertionColorDisplayLabel->setColor(insertionColor);
    ui.deletionColorDisplayLabel->setColor(deletionColor);
    ui.formatColorDisplayLabel->setColor(formatChangeColor);
    ui.authorNameLineEdit->setText(authorName);
    if (changeSaveFormat == KoChangeTracker::ODF_1_2) {
        ui.odf12RadioButton->setChecked(true);
    } else {
        ui.deltaXmlRadioButton->setChecked(true);
    }
    connect(ui.insertionColorButton, &QAbstractButton::clicked, this, &ChangeConfigureDialog::insertionColorSelect);
    connect(ui.deletionColorButton, &QAbstractButton::clicked, this, &ChangeConfigureDialog::deletionColorSelect);
    connect(ui.formatColorButton, &QAbstractButton::clicked, this, &ChangeConfigureDialog::formatChangeColorSelect);
    updatePreviewText();
}

const QString ChangeConfigureDialog::authorName()
{
    return ui.authorNameLineEdit->text();
}

KoChangeTracker::ChangeSaveFormat ChangeConfigureDialog::saveFormat()
{
    if (ui.odf12RadioButton->isChecked()) {
        return KoChangeTracker::ODF_1_2;
    } else {
        return KoChangeTracker::DELTAXML;
    }
}

const QColor &ChangeConfigureDialog::getInsertionBgColor()
{
    return ui.insertionColorDisplayLabel->color();
}

const QColor &ChangeConfigureDialog::getDeletionBgColor()
{
    return ui.deletionColorDisplayLabel->color();
}

const QColor &ChangeConfigureDialog::getFormatChangeBgColor()
{
    return ui.formatColorDisplayLabel->color();
}

void ChangeConfigureDialog::insertionColorSelect()
{
    colorSelect(eInsert);
}

void ChangeConfigureDialog::deletionColorSelect()
{
    colorSelect(eDelete);
}

void ChangeConfigureDialog::formatChangeColorSelect()
{
    colorSelect(eFormatChange);
}

void ChangeConfigureDialog::colorSelect(ChangeType type)
{
    QColor selectedColor;

    switch (type) {
    case eInsert:
        selectedColor = QColorDialog::getColor(ui.insertionColorDisplayLabel->color(), this);
        if (selectedColor.isValid()) {
            ui.insertionColorDisplayLabel->setColor(selectedColor);
            ui.insertionColorDisplayLabel->update();
        }
        break;
    case eDelete:
        selectedColor = QColorDialog::getColor(ui.deletionColorDisplayLabel->color(), this);
        if (selectedColor.isValid()) {
            ui.deletionColorDisplayLabel->setColor(selectedColor);
            ui.deletionColorDisplayLabel->update();
        }
        break;
    case eFormatChange:
        selectedColor = QColorDialog::getColor(ui.formatColorDisplayLabel->color(), this);
        if (selectedColor.isValid()) {
            ui.formatColorDisplayLabel->setColor(selectedColor);
            ui.formatColorDisplayLabel->update();
        }
        break;
    case eChangeTypeNone:
        break;
    }

    updatePreviewText();
}

void ChangeConfigureDialog::updatePreviewText(void)
{
    // update the insertion-text
    if (ui.previewTextEdit->find(i18n("This is a line of inserted text."))) {
        ui.previewTextEdit->setTextBackgroundColor(ui.insertionColorDisplayLabel->color());
        ui.previewTextEdit->moveCursor(QTextCursor::Start);
    }

    // update the deletion-text
    if (ui.previewTextEdit->find(i18n("This is a line of deleted text."))) {
        ui.previewTextEdit->setTextBackgroundColor(ui.deletionColorDisplayLabel->color());
        ui.previewTextEdit->moveCursor(QTextCursor::Start);
    }

    // update the format-change-text
    if (ui.previewTextEdit->find(i18n("This is a line of text whose format has been changed."))) {
        ui.previewTextEdit->setTextBackgroundColor(ui.formatColorDisplayLabel->color());
        ui.previewTextEdit->moveCursor(QTextCursor::Start);
    }
}

ChangeConfigureDialog::~ChangeConfigureDialog() = default;
