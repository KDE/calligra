/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef BIBLIOGRAPHYCONFIGUREDIALOG_H
#define BIBLIOGRAPHYCONFIGUREDIALOG_H

#include "ui_BibliographyConfigureDialog.h"

#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QTextDocument>

#include "KoOdfBibliographyConfiguration.h"

class BibliographyConfigureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BibliographyConfigureDialog(const QTextDocument *document, QWidget *parent = nullptr);

public Q_SLOTS:
    void addSortKey();
    void save(QAbstractButton *);
    void sortMethodChanged(bool);

private:
    Ui::BibliographyConfigureDialog dialog;
    const QTextDocument *m_document;
    KoOdfBibliographyConfiguration *m_bibConfiguration;
};

class SortKeyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SortKeyWidget(const QString &sortKey, Qt::SortOrder order, QWidget *parent);

    QString sortKey() const;
    Qt::SortOrder sortOrder() const;

    void setSortKey(const QString &sortKey);
    void setSortOrder(Qt::SortOrder order);

private:
    QComboBox *m_dataFields;
    QRadioButton *m_ascButton;
    QRadioButton *m_dscButton;
    QHBoxLayout *m_layout;
};

#endif // BIBLIOGRAPHYCONFIGUREDIALOG_H
