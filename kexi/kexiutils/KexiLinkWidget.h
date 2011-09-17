/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXILINKWIDGET_H
#define KEXILINKWIDGET_H

#include "kexiutils_export.h"

#include <QtGui/QLabel>

//! Link widget
class KEXIUTILS_EXPORT KexiLinkWidget : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QString link READ link WRITE setLink)
    Q_PROPERTY(QString linkText READ linkText WRITE setLinkText)
    Q_PROPERTY(QString format READ format WRITE setFormat)
public:
    explicit KexiLinkWidget(QWidget* parent = 0);

    KexiLinkWidget(const QString& link, const QString& linkText, QWidget* parent = 0);
    
    virtual ~KexiLinkWidget();

    QString link() const;

    QString linkText() const;
    
    QString format() const;

public slots:
    void setLink(const QString& link);
    
    void setLinkText(const QString& linkText);

    //! Sets format for the button. 
    /*! Format defines user-visible text written around the link. 
        Use "%L" as a placeholder for the link, e.g. when format
        is "‹ %L" and link text is "Back", the widget will show "‹ Back"
        where "Back" is a link. By default format is empty, what means
        only the link is displayed. */
    void setFormat(const QString& format);
    
protected:
    virtual void changeEvent(QEvent* event);

private:
    QString text() const { return QLabel::text(); }
    void setText(const QString& text) { QLabel::setText(text); }

    class Private;
    Private * const d;
};

#endif
