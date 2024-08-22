/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014-2015 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWDebugWidget.h"
#include <KWView.h>
#include <KoElementReference.h>
#include <KoParagraphStyle.h>
#include <KoSection.h>
#include <KoSectionEnd.h>
#include <KoSectionUtils.h>
#include <KoShapeController.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <frames/KWTextFrameSet.h>

#include <QHBoxLayout>
#include <QString>
#include <QTimer>

KWDebugWidget::KWDebugWidget(QWidget *parent)
    : QWidget(parent)
    , m_canvas(nullptr)
{
    initUi();
    initLayout();

    updateData();
}

KWDebugWidget::~KWDebugWidget() = default;

void KWDebugWidget::initUi()
{
    m_label = new QLabel(this);
    m_label->setText("Some debug info will be here."); // No i18n as it's for debug only.

    m_buttonSet = new QPushButton(this);
    m_buttonSet->setText("Set"); // No i18n as it's for debug only.
    connect(m_buttonSet, &QAbstractButton::clicked, this, &KWDebugWidget::doSetMagic);

    m_buttonGet = new QPushButton(this);
    m_buttonGet->setText("Get"); // No i18n as it's for debug only.
    connect(m_buttonGet, &QAbstractButton::clicked, this, &KWDebugWidget::doGetMagic);
}

void KWDebugWidget::initLayout()
{
    QVBoxLayout *mainBox = new QVBoxLayout(this);
    mainBox->addWidget(m_label);
    mainBox->addWidget(m_buttonSet);
    mainBox->addWidget(m_buttonGet);

    setLayout(mainBox);
}

void KWDebugWidget::updateData()
{
    QTimer().singleShot(100, this, &KWDebugWidget::updateData);
    if (!isVisible()) {
        return;
    }

    KoTextEditor *editor = nullptr;
    if (m_canvas) {
        editor = KoTextEditor::getTextEditorFromCanvas(m_canvas);
        if (!editor) {
            return;
        }
    } else {
        return;
    }

    QTextBlock curBlock = editor->block();
    QTextBlockFormat fmt = curBlock.blockFormat();

    QString willShow = "This sections starts here :";
    foreach (const KoSection *sec, KoSectionUtils::sectionStartings(fmt)) {
        QPair<int, int> bnds = sec->bounds();
        willShow += " \"" + sec->name() + "\"(" + QString::number(bnds.first) + "; " + QString::number(bnds.second) + ")";
    }
    willShow.append("\n");

    willShow += "This sections end here :";
    foreach (const KoSectionEnd *sec, KoSectionUtils::sectionEndings(fmt)) {
        willShow += " \"" + sec->name() + "\"";
    }
    willShow.append("\n");

    willShow += "block number is " + QString::number(editor->constCursor().block().blockNumber()) + "\n";
    willShow += "cur pos " + QString::number(editor->constCursor().position()) + "\n";
    willShow += "pos in block " + QString::number(editor->constCursor().positionInBlock()) + "\n";
    willShow += "length of block " + QString::number(editor->constCursor().block().length()) + "\n";

    m_label->setText(willShow);
}

void KWDebugWidget::setCanvas(KWCanvas *canvas)
{
    m_canvas = canvas;
}

void KWDebugWidget::unsetCanvas()
{
}

void KWDebugWidget::doSetMagic()
{
    updateData();
}

void KWDebugWidget::doGetMagic()
{
    m_canvas->view()->setShowSectionBounds(true);
}
