/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    explicit KWPageSettingsDialog(QWidget *parent, KWDocument *document, const KWPage &page);
    KPageWidgetItem *pageItem(const QString &name) const;

protected:
    void accept() override;
    void reject() override;

private Q_SLOTS:
    void slotApplyClicked();
    void slotButtonClicked(QAbstractButton *button);
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
    QMap<QString, KPageWidgetItem *> m_pages;
    QString m_prevSelectedPageStyle;

    void reloadPageStyles();
};

#endif
