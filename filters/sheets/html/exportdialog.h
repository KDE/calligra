/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <KoDialog.h>
class QUrl;

#include <ui_exportwidget.h>

class ExportWidget : public QWidget, public Ui::ExportWidget
{
public:
    explicit ExportWidget(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

class ExportDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit ExportDialog(QWidget *parent = nullptr);
    ~ExportDialog() override;

    void setSheets(const QStringList &);
    QStringList sheets() const;

    /**
      Returns a valid URL if the custom button was selected.
      Else, it will return QUrl().
    */
    QUrl customStyleURL() const;

    /**
      Returns true if borders should be shown, false if borders
      should be hidden.
     */
    bool useBorders() const;

    bool separateFiles() const;

    int pixelsBetweenCells() const;
protected Q_SLOTS:
    void selectAll();

private:
    ExportWidget *m_mainwidget;
};

#endif
