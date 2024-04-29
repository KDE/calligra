/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "StylesWidget.h"
#include "CharacterGeneral.h"
#include "ParagraphGeneral.h"
#include "StylesDelegate.h"
#include "StylesModel.h"
#include <KoStyleThumbnailer.h>

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>

#include <QDebug>
#include <QFormLayout>
#include <QHeaderView>
#include <QHideEvent>
#include <QRadioButton>
#include <QScrollBar>
#include <QShowEvent>

#include <QModelIndex>

StylesWidget::StylesWidget(QWidget *parent, bool paragraphMode, Qt::WindowFlags f)
    : QFrame(parent, f)
    , m_styleManager(0)
    , m_styleThumbnailer(0)
    , m_stylesModel(new StylesModel(0, StylesModel::ParagraphStyle))
    , m_stylesDelegate(new StylesDelegate())
    , m_blockSignals(false)
    , m_isHovered(false)
{
    m_styleThumbnailer = new KoStyleThumbnailer();
    m_styleThumbnailer->setThumbnailSize(QSize(250, 48));
    m_stylesModel->setStyleThumbnailer(m_styleThumbnailer);
    widget.setupUi(this);
    widget.stylesView->setModel(m_stylesModel);
    //    widget.stylesView->setItemDelegate(m_stylesDelegate);

    if (paragraphMode) {
        connect(widget.stylesView, SIGNAL(clicked(QModelIndex)), this, SLOT(applyParagraphStyle()));
    } else {
        connect(widget.stylesView, SIGNAL(clicked(QModelIndex)), this, SLOT(applyCharacterStyle()));
    }
}

StylesWidget::~StylesWidget()
{
    delete m_stylesDelegate;
    delete m_stylesModel;
    delete m_styleThumbnailer;
}

QSize StylesWidget::sizeHint() const
{
    return QSize(widget.stylesView->sizeHint().width() + 2 * widget.stylesView->verticalScrollBar()->width(), widget.stylesView->sizeHint().height());
}

void StylesWidget::setStyleManager(KoStyleManager *sm)
{
    m_styleManager = sm;
    m_stylesModel->setStyleManager(sm);
}

void StylesWidget::setCurrentFormat(const QTextBlockFormat &format)
{
    if (format == m_currentBlockFormat)
        return;
    m_currentBlockFormat = format;
    int id = m_currentBlockFormat.intProperty(KoParagraphStyle::StyleId);
    // bool unchanged = true;
    KoParagraphStyle *usedStyle = 0;
    if (m_styleManager)
        usedStyle = m_styleManager->paragraphStyle(id);
    if (usedStyle) {
        foreach (int property, m_currentBlockFormat.properties().keys()) {
            if (property == QTextFormat::ObjectIndex)
                continue;
            if (property == KoParagraphStyle::ListStyleId)
                continue;
            if (m_currentBlockFormat.property(property) != usedStyle->value(property)) {
                // unchanged = false;
                break;
            }
        }
    }

    m_blockSignals = true;
    //    m_stylesModel->setCurrentParagraphStyle(id, unchanged);
    m_blockSignals = false;
    widget.stylesView->setCurrentIndex(m_stylesModel->indexOf(*usedStyle));
}

void StylesWidget::setCurrentFormat(const QTextCharFormat &format)
{
    if (format == m_currentCharFormat)
        return;
    m_currentCharFormat = format;

    int id = m_currentCharFormat.intProperty(KoCharacterStyle::StyleId);
    // bool unchanged = true;
    KoCharacterStyle *usedStyle = 0;
    if (m_styleManager)
        usedStyle = m_styleManager->characterStyle(id);
    if (usedStyle) {
        QTextCharFormat defaultFormat;
        usedStyle->unapplyStyle(defaultFormat); // sets the default properties.
        foreach (int property, m_currentCharFormat.properties().keys()) {
            if (property == QTextFormat::ObjectIndex)
                continue;
            if (m_currentCharFormat.property(property) != usedStyle->value(property)
                && m_currentCharFormat.property(property) != defaultFormat.property(property)) {
                //          unchanged = false;
                break;
            }
        }
    }

    m_blockSignals = true;
    //    m_stylesModel->setCurrentCharacterStyle(id, unchanged);
    m_blockSignals = false;
    widget.stylesView->setCurrentIndex(m_stylesModel->indexOf(*usedStyle));
}

void StylesWidget::applyParagraphStyle()
{
    QModelIndex index = widget.stylesView->currentIndex();
    Q_ASSERT(index.isValid());
    KoParagraphStyle *paragraphStyle = m_stylesModel->paragraphStyleForIndex(index);
    if (paragraphStyle) {
        Q_EMIT paragraphStyleSelected(paragraphStyle);
        Q_EMIT doneWithFocus();
        return;
    }
}

void StylesWidget::applyCharacterStyle()
{
    QModelIndex index = widget.stylesView->currentIndex();
    Q_ASSERT(index.isValid());
    KoCharacterStyle *characterStyle = m_stylesModel->characterStyleForIndex(index);
    if (characterStyle) {
        Q_EMIT characterStyleSelected(characterStyle);
        Q_EMIT doneWithFocus();
        return;
    }
}
