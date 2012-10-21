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

#include "ui_StylesManager.h"

#include "StylesManager.h"
#include "StylesManagerStylesListItemDelegate.h"
#include "StylesManagerStylesModel.h"
#include "StylesManagerStylesListModel.h"
#include "StylesModel.h"
#include "StylesModelV1.h"

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>

#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QTreeView>

#include <KDebug>

StylesManager::StylesManager(KoStyleManager *sm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StylesManager),
    m_styleManager(sm),
    m_styleThumbnailer(new KoStyleThumbnailer()),
    m_characterStyleBaseModel(new StylesModel(0,StylesModel::CharacterStyle)),
    m_paragraphStyleBaseModel(new StylesModel(0,StylesModel::ParagraphStyle)),
    m_characterStylesModel(new StylesManagerStylesModel()),
    m_paragraphStylesModel(new StylesManagerStylesModel()),
    m_characterStylesListModel(new StylesManagerStylesListModel()),
    m_paragraphStylesListModel(new StylesManagerStylesListModel()),
    m_characterStylesListItemDelegate(new StylesManagerStylesListItemDelegate()),
    m_paragraphStylesListItemDelegate(new StylesManagerStylesListItemDelegate())
{
    ui->setupUi(this);

    // Force "Base" background of the style listviews to white, so the background
    // is consistent with the one of the preview area. Also the usual document text colors
    // are dark, because made for a white paper background, so with a dark UI
    // color scheme they are hardly seen.
    // TODO: update to background color of currently selected/focused shape/page
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, QColor(Qt::white));

    ui->characterStylesListView->setPalette(palette);
    ui->paragraphStylesListView->setPalette(palette);

//    ui->characterGeneralTab->setVisible(false);
//    ui->paragraphGeneralTab->setVisible(false);
//    ui->characterStylesListView->setVisible(false);

}

StylesManager::~StylesManager()
{
    delete ui;
    delete m_characterStylesListModel;
    delete m_paragraphStylesListModel;
    delete m_characterStyleBaseModel;
    delete m_paragraphStyleBaseModel;
    delete m_characterStylesModel;
    delete m_paragraphStylesModel;
    delete m_characterStylesListItemDelegate;
    delete m_paragraphStylesListItemDelegate;
}

void StylesManager::setStyleManager(KoStyleManager *sm)
{

    Q_ASSERT(sm);
    m_styleManager = sm;

//    m_characterStyleBaseModel->setProvideStyleNone(false);
//    m_characterStyleBaseModel->setStyleManager(m_styleManager);
//    m_characterStyleBaseModel->setStyleThumbnailer(m_styleThumbnailer);
    m_paragraphStyleBaseModel->setStyleManager(m_styleManager);
    m_paragraphStyleBaseModel->setStyleThumbnailer(m_styleThumbnailer);

//    m_characterStylesModel->setStyleManager(m_styleManager);
//    m_characterStylesModel->setStylesModel(m_characterStyleBaseModel);
    m_paragraphStylesModel->setStyleManager(m_styleManager);
    m_paragraphStylesModel->setStylesModel(m_paragraphStyleBaseModel);

//    m_characterStylesListModel->setStylesModel(m_characterStylesModel);
    m_paragraphStylesListModel->setStylesModel(m_paragraphStylesModel);

//    ui->characterStylesListView->setModel(m_characterStylesListModel);
//    ui->characterStylesListView->setItemDelegate(m_characterStylesListItemDelegate);
//    ui->characterStylesListView->expandAll();
//    ui->characterStylesListView->setIndentation(5);
//    ui->characterStylesListView->setHeaderHidden(true);
//    ui->characterStylesListView->setRootIsDecorated(false);
    ui->paragraphStylesListView->setModel(m_paragraphStylesListModel);
    ui->paragraphStylesListView->setItemDelegate(m_paragraphStylesListItemDelegate);
    ui->paragraphStylesListView->expandAll();
    ui->paragraphStylesListView->setIndentation(5);
    ui->paragraphStylesListView->setHeaderHidden(true);
    ui->paragraphStylesListView->setRootIsDecorated(false);

    ui->characterGeneralTab->setStyleManager(m_styleManager);
//    ui->characterGeneralTab->setStylesModel(m_characterStylesModel);
    ui->paragraphGeneralTab->setStyleManager(m_styleManager);
//    ui->paragraphGeneralTab->setStylesModel(m_paragraphStylesModel);

    connect(ui->characterStylesListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotCharacterStyleSelected(const QModelIndex&)));
    connect(ui->paragraphStylesListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotParagraphStyleSelected(const QModelIndex&)));

    connect(ui->saveCharacterStyleButton, SIGNAL(clicked()), this, SLOT(slotSaveCharacterStyle()));
    connect(ui->saveParagraphStyleButton, SIGNAL(clicked()), this, SLOT(slotSaveParagraphStyle()));

    connect(ui->characterGeneralTab, SIGNAL(styleChanged()), this, SLOT(slotCharacterStyleChanged()));
    connect(ui->paragraphGeneralTab, SIGNAL(styleChanged()), this, SLOT(slotParagraphStyleChanged()));

}

void StylesManager::slotCharacterStyleSelected(const QModelIndex &index)
{
    Q_ASSERT(m_styleManager);
    if (!m_styleManager) {
        return;
    }

    KoCharacterStyle *style = m_styleManager->characterStyle(index.internalId());
    if (!style) {
        return;
    }
    setCurrentCharacterStyle(style);
}

void StylesManager::slotParagraphStyleSelected(const QModelIndex &index)
{
    kDebug() << "in slot parag activated. internal id: " << index.internalId();
//    kDebug() << "mapped internalId: " << m_paragraphStylesListModel->mapToSource(index).internalId();
    kDebug() << "selection name: " << index.data();

    Q_ASSERT(m_styleManager);
    if (!m_styleManager) {
        return;
    }

    blockSignals(true);

    m_paragraphStylesModel->slotSetCurrentIndex(index);

    if (m_paragraphStylesModel->currentSelectedStyle()) {
        ui->paragraphGeneralTab->setCurrentStyle(m_paragraphStylesModel->currentSelectedStyle());
    }
//    m_currentCharacterStyle = style;

    blockSignals(false);

}

void StylesManager::slotSaveCharacterStyle()
{
    if (m_currentCharacterStyle) {
        kDebug() << "-----------------------CharacterStyle saving----------";
        kDebug() << "current name: " << m_currentCharacterStyle->name();
        if (m_currentCharacterStyle->parentStyle())
            kDebug() << "current parent: " << m_currentCharacterStyle->parentStyle()->name();
        else
            kDebug() << "current parent: None";
        ui->characterGeneralTab->save(m_currentCharacterStyle);
        kDebug() << "-------------after saving";
        kDebug() << "current name: " << m_currentCharacterStyle->name();
        if (m_currentCharacterStyle->parentStyle())
            kDebug() << "current parent: " << m_currentCharacterStyle->parentStyle()->name();
        else
            kDebug() << "current parent: None";
    }
}

void StylesManager::slotSaveParagraphStyle()
{
    if (m_currentParagraphStyle) {
        kDebug() << "-----------------------ParagraphStyle saving-----------";
        kDebug() << "current name: " << m_currentParagraphStyle->name();
        if (m_currentParagraphStyle->parentStyle())
            kDebug() << "current parent: " << m_currentParagraphStyle->parentStyle()->name();
        else
            kDebug() << "current parent: None";
        KoParagraphStyle *nextStyle = m_styleManager->paragraphStyle(m_currentParagraphStyle->nextStyle());
        if (nextStyle)
            kDebug() << "current next style: " << nextStyle->name();
        else
            kDebug() << "current next style: None";

        ui->paragraphGeneralTab->save(m_currentParagraphStyle);

        kDebug() << "-----------------------after saving";
        kDebug() << "current name: " << m_currentParagraphStyle->name();
        if (m_currentParagraphStyle->parentStyle())
            kDebug() << "current parent: " << m_currentParagraphStyle->parentStyle()->name();
        else
            kDebug() << "current parent: None";
        nextStyle = m_styleManager->paragraphStyle(m_currentParagraphStyle->nextStyle());
        if (nextStyle)
            kDebug() << "current next style: " << nextStyle->name();
        else
            kDebug() << "current next style: None";
    }
}

void StylesManager::slotCharacterStyleChanged()
{

}

void StylesManager::slotParagraphStyleChanged()
{
    kDebug() << "in slotParagStyle changed";
    m_paragraphStylesModel->slotModifiedStyle(/*m_currentParagraphStyle*/);
    ui->paragraphGeneralTab->save(dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->currentSelectedStyle()));
    kDebug() << "saved. parent style: " << ((m_paragraphStylesModel->currentSelectedStyle()->parentStyle())?m_paragraphStylesModel->currentSelectedStyle()->parentStyle()->name():"None");
    ui->paragraphStylesListView->expandAll();
}

void StylesManager::setCurrentCharacterStyle(KoCharacterStyle *style)
{
    blockSignals(true);

    ui->characterGeneralTab->setCurrentStyle(style);
//    m_currentCharacterStyle = style;

    blockSignals(false);
}

void StylesManager::setCurrentParagraphStyle(KoParagraphStyle *style)
{
    blockSignals(true);

    if (style) {
        kDebug() << "setCurrent parStyle: "  << style->styleId();
        kDebug() << "style name: " << style->name();
        ui->paragraphGeneralTab->setCurrentStyle((KoCharacterStyle*)(style));
        m_currentParagraphStyle = style;
    }

    blockSignals(false);
}
