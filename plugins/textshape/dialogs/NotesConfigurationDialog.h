/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Brijesh Patel <brijesh3105@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef NOTESCONFIGURATIONDIALOG_H
#define NOTESCONFIGURATIONDIALOG_H

#include <KoListStyle.h>
#include <KoOdfNotesConfiguration.h>
#include <ui_NotesConfigurationDialog.h>

#include <QDialog>
#include <QObject>
#include <QTextDocument>

class KoStyleManager;

class NotesConfigurationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NotesConfigurationDialog(QTextDocument *doc, bool footnoteMode, QWidget *parent = nullptr);
    Ui::NotesConfigurationDialog widget;

public Q_SLOTS:
    void setStyleManager(KoStyleManager *sm);
    /**
     * sets up the footnote's default configuration in the dialog box
     */
    void footnoteSetup();
    /**
     * sets up the endnote's default configuration in the dialog box
     */
    void endnoteSetup();
    /**
     * stores the applied notes' configuration as globalnotesconfiguration of the document
     */
    void apply(QAbstractButton *);

private:
    KoOdfNotesConfiguration *m_notesConfig;
    KoStyleManager *m_styleManager;
    QTextDocument *m_document;
};

#endif
