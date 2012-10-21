/* This file is part of the KDE project
 * Copyright (C) 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
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

#include "ui_StylesGeneralTab.h"

#include "StylesGeneralTab.h"

#include "StylesModel.h"
#include "AbstractStylesModel.h"
#include "ValidParentStylesProxyModel.h"

#include <KoCharacterStyle.h>
#include <KoIcon.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>

#include <KDebug>
#include <KLocale>

StylesGeneralTab::StylesGeneralTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StylesGeneralTab),
    m_style(0),
    m_styleManager(0),
    m_styleModel(0),
    m_validParentStylesModel(new ValidParentStylesProxyModel())
{
    ui->setupUi(this);

    ui->nextStyle->setStyleIsOriginal(true);
    ui->nextStyle->showEditIcon(false);

    ui->inheritStyle->setStyleIsOriginal(true);
    ui->inheritStyle->showEditIcon(false);

    const KIcon clearIcon(koIconName("edit-clear"));
    ui->clearInheritStyle->setIcon(clearIcon);
    ui->clearNextStyle->setIcon(clearIcon);


    connect(ui->name, SIGNAL(textChanged(const QString&)), this, SIGNAL(nameChanged(const QString&)));
    connect(ui->nextStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(styleChanged()));
    connect(ui->inheritStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(styleChanged()));
    connect(ui->clearNextStyle, SIGNAL(clicked()), this, SLOT(slotClearNextStyle()));
    connect(ui->clearInheritStyle, SIGNAL(clicked()), this, SLOT(slotClearInheritStyle()));
}

StylesGeneralTab::~StylesGeneralTab()
{
    delete ui;
}

void StylesGeneralTab::setStylesModel(AbstractStylesModel *model)
{
    Q_ASSERT(model);
/*    m_styleModel = model;
    m_validParentStylesModel->setStylesModel(m_styleModel);
    if (model->stylesType() == StylesModel::CharacterStyle) {
        m_styleType = CharacterStyle;
        ui->nextStyle->setVisible(false);
        ui->nextStyleLabel->setVisible(false);
    }
    else {
        m_styleType = ParagraphStyle;
        ui->nextStyle->setStylesModel(m_styleModel);
    }
    ui->inheritStyle->setStylesModel(m_validParentStylesModel);*/
}

void StylesGeneralTab::setStyleManager(KoStyleManager *sm)
{
    Q_ASSERT(sm);
    m_styleManager = sm;
    m_validParentStylesModel->setStyleManager(sm);
}

void StylesGeneralTab::setCurrentStyle(KoCharacterStyle *style)
{
    Q_ASSERT(m_styleModel);
    Q_ASSERT(m_styleManager);
    Q_ASSERT(style);
    if (!style || !m_styleModel || !m_styleManager) {
        return;
    }

    if (style->styleType() != KoCharacterStyle::ParagraphStyle && m_styleType == ParagraphStyle) {
        Q_ASSERT(false);
        return;
    }

    KoParagraphStyle *paragStyle = dynamic_cast<KoParagraphStyle*>(style);
    blockSignals(true);

    if (!style->name().isEmpty()) {
        ui->name->setText(style->name());
    }
    else {
        ui->name->setText(QString());
        ui->name->setPlaceholderText(i18n("New name"));
    }

    m_validParentStylesModel->setCurrentChildStyleId(style->styleId());

    if (paragStyle) {
//        m_styleModel->setCurrentParagraphStyle(paragStyle->styleId());
        KoParagraphStyle *nextStyle = m_styleManager->paragraphStyle(paragStyle->nextStyle());
        if (nextStyle) {
            ui->nextStyle->setCurrentIndex(m_styleModel->indexForParagraphStyle(*nextStyle).row());
        }
        else {
            ui->nextStyle->setCurrentIndex(-1);
        }
        KoParagraphStyle *parentStyle = paragStyle->parentStyle();
        if (parentStyle) {
            ui->inheritStyle->setCurrentIndex(m_validParentStylesModel->indexForParagraphStyle(*parentStyle).row());
        }
        else {
            ui->inheritStyle->setCurrentIndex(-1);
        }
    }
    else {
        KoCharacterStyle *parentStyle = style->parentStyle();
        if (parentStyle) {
            ui->inheritStyle->setCurrentIndex(m_validParentStylesModel->indexForCharacterStyle(*parentStyle).row());
        }
        else {
            ui->inheritStyle->setCurrentIndex(-1);
        }
    }

    m_style = style;

    blockSignals(false);
}

void StylesGeneralTab::save(KoCharacterStyle *style)
{
    Q_ASSERT(m_styleModel);
    Q_ASSERT(m_styleManager);
    if (!m_styleModel || !m_styleManager) {
        return;
    }

    KoCharacterStyle *savingStyle = style;
    if (savingStyle == 0) {
        if (m_style == 0) {
            return;
        }
        else {
            savingStyle = m_style;
        }
    }

    if (!ui->name->text().isEmpty() && ui->name->text() != savingStyle->name()) {
        savingStyle->setName(ui->name->text());
    }

    if (savingStyle->styleType() == KoCharacterStyle::ParagraphStyle) {
        KoParagraphStyle *paragStyle = dynamic_cast<KoParagraphStyle*>(savingStyle);
        Q_ASSERT(paragStyle); //this should not be the case

        if (ui->nextStyle->currentIndex() != -1) {
//            paragStyle->setNextStyle(m_styleModel->index(ui->nextStyle->currentIndex()).internalId());
        }
        else {
            paragStyle->remove(KoParagraphStyle::NextStyle);
        }

        if (ui->inheritStyle->currentIndex() != -1) {
            KoParagraphStyle *parentStyle = m_styleManager->paragraphStyle(m_validParentStylesModel->index(ui->inheritStyle->currentIndex(), 0, QModelIndex()).internalId());
            if (parentStyle) {
                paragStyle->setParentStyle(parentStyle);
            }
            else {
                paragStyle->setParentStyle(0);
            }
        }
        else {
            paragStyle->setParentStyle(0);
        }
    }
    else {
        if (ui->inheritStyle->currentIndex() != -1) {
            KoCharacterStyle *parentStyle = m_styleManager->characterStyle(m_validParentStylesModel->index(ui->inheritStyle->currentIndex(), 0, QModelIndex()).internalId());
            if (parentStyle) {
                savingStyle->setParentStyle(parentStyle);
            }
            else {
                savingStyle->setParentStyle(0);
            }
        }
        else {
            savingStyle->setParentStyle(0);
        }
    }
}

void StylesGeneralTab::slotClearNextStyle()
{/*
    if (m_styleModel->stylesType() != StylesModel::ParagraphStyle || m_style->styleType() != KoCharacterStyle::ParagraphStyle) {
        return;
    }
    else {
        QModelIndex index = m_styleModel->indexForParagraphStyle(*(dynamic_cast<KoParagraphStyle*>(m_style)));
        ui->nextStyle->setCurrentIndex(index.row());
    }*/
}

void StylesGeneralTab::slotClearInheritStyle()
{
    ui->inheritStyle->setCurrentIndex(-1);
}
