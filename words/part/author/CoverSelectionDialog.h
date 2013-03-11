/*
This file is part of the KDE project
Copyright (C) 2013 Jeremy Bourdiol <jerem.dante@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA
*/

#ifndef COVERSELECTIONDIALOG_H
#define COVERSELECTIONDIALOG_H

#include "CAuView.h"
#include <QDialog>
#include <QLabel>

namespace Ui {
class CoverSelectionDialog;
}

class CoverSelectionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CoverSelectionDialog(CAuView *au, QWidget *parent = 0);
    ~CoverSelectionDialog();

private slots:
    void open();
    void reset();
    void ok();

private:
    Ui::CoverSelectionDialog *ui;

    void refresh();
    void createActions();

    //needed to ask for oparations on the kdocument use
    //by the given view
    CAuView *view;

    //represent the volatil data use in the preview
    //this data will be store only when 'ok' will be clicked
    QPair<QString, QByteArray> img;

    QAction *openAct;
    QAction *exitAct;
};

#endif // COVERSELECTIONDIALOG_H
