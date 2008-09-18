/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KexiDBTextWidgetInterface_H
#define KexiDBTextWidgetInterface_H

#include <widget/utils/kexidisplayutils.h>
#include <QFrame>
#include <QEvent>

namespace KexiDB
{
class QueryColumnInfo;
}

//! @short An interface providing common text editor's functionality
/*! Widgets (e.g. KexiDBLineEdit, KexiDBTextEdit) implementing KexiFormDataItemInterface
 use this interface to customize painting and data handling. */
class KEXIFORMUTILS_EXPORT KexiDBTextWidgetInterface
{
public:
    KexiDBTextWidgetInterface();
    ~KexiDBTextWidgetInterface();

    //! Called from KexiFormDataItemInterface::setColumnInfo(KexiDB::QueryColumnInfo* cinfo) implementation.
    void setColumnInfo(KexiDB::QueryColumnInfo* cinfo, QWidget *w);

    //! Called from paintEvent( QPaintEvent *pe ) method of the data aware widget.
    void paint(QWidget *w, QPainter *p, bool textIsEmpty, Qt::Alignment alignment,
               bool hasFocus);

    //! Called from event( QEvent * e ) method of the data aware widget.
    void event(QEvent * e, QWidget *w, bool textIsEmpty);

protected:
    //! parameters for displaying autonumber sign
    KexiDisplayUtils::DisplayParameters *m_autonumberDisplayParameters;
};

#endif
