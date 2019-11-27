/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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
#ifndef KWPAGESETTINGSDIALOG_H
#define KWPAGESETTINGSDIALOG_H

#include <KWPage.h>
#include <KWPageStyle.h>
#include <KoPageLayoutDialog.h>

class QPushButton;
class QListWidget;
class KWDocument;
class KWDocumentColumns;
class KPageWidgetItem;

/// A dialog to show the settings for one page and apply them afterwards.
class KWPageSettingsDialog : public KoPageLayoutDialog
{
    Q_OBJECT
public:
    explicit KWPageSettingsDialog(QWidget *parent, KWDocument * document, const KWPage &page);
    KPageWidgetItem* pageItem(const QString &name) const;

protected:
    void accept() override;
    void reject() override;

private Q_SLOTS:
    void slotApplyClicked();
    void slotButtonClicked(QAbstractButton* button);
    void setDocumentUnit(const KoUnit &unit);
    void onDocumentUnitChange(const KoUnit &unit);
    void pageStyleCloneClicked();
    void pageStyleDeleteClicked();
    void pageStyleCurrentRowChanged(int row);
private:
    KWDocument *m_document;
    KWPage m_page;
    KWPageStyle m_pageStyle;
    KWDocumentColumns *m_columns;
    QListWidget *m_pageStylesView;
    QPushButton *m_clonePageStyleButton, *m_deletePageStyleButton;
    QMap<QString, KPageWidgetItem*> m_pages;
    QString m_prevSelectedPageStyle;

    void reloadPageStyles();
};

#endif
