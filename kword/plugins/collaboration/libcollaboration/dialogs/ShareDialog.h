/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KCOLLABORATE_SHAREDIALOG_H
#define KCOLLABORATE_SHAREDIALOG_H

#include <QDialog>
#include <QString>

#include <libcollaboration/kcollaborate_export.h>
#include <libcollaboration/network/Url.h>

namespace Ui
{
class ShareDialog;
};

namespace kcollaborate
{

class KCOLLABORATE_EXPORT ShareDialog: public QDialog
{
        Q_OBJECT
    public:
        ShareDialog( bool defaultUi = true, QWidget *parent = 0, Qt::WindowFlags f = 0 );
        ~ShareDialog();

        ///emits Share signal if accepted
        int exec();

    signals:
        void share( const Url& url );

    private:
        Ui::ShareDialog *ui;
        QString baseUrl;

    private slots:
        void on_documentNameLineEdit_textEdited( const QString &text );
};

};

#endif
