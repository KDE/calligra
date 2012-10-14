/* This file is part of the KDE project
   Copyright (C) 2004-2012 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXINAMEDIALOG_H
#define KEXINAMEDIALOG_H

#include <KDialog>
#include <core/kexiproject.h>
#include <core/kexipart.h>

class KexiNameWidget;

class KEXIEXTWIDGETS_EXPORT KexiNameDialog : public KDialog
{
    Q_OBJECT

public:
    explicit KexiNameDialog(const QString& message, QWidget * parent = 0);

    KexiNameDialog(const QString& message,
                   const QString& nameLabel, const QString& nameText,
                   const QString& captionLabel, const QString& captionText,
                   QWidget * parent = 0);

    virtual ~KexiNameDialog();

    KexiNameWidget* widget() const;

    void setDialogIcon(const QString &iconName);

    /*! Shows the dialog as a modal dialog, blocking until the user closes it, like KDialog::exec()
        but uses @a project and @a part to check if object of given type and name already exists.
        If so, warning or question is displayed.
        You can check @a overwriteNeeded after calling this method.
        If it's true, user agreed on overwriting, if it's false, user picked
        nonexisting name, so no overwrite will be needed. */
    int execAndCheckIfObjectExists(const KexiProject &project, const KexiPart::Part &part,
                                   bool *overwriteNeeded);

    //! If set to true, the dialog will ask for overwriting the existing object if needed.
    //! If set to false, the dialog will inform about existing object and reject renaming.
    //! False by default.
    void setAllowOverwriting(bool set);

protected slots:
    void slotTextChanged();
    virtual void accept();
    void updateSize();

protected:
    void init();
    virtual void showEvent(QShowEvent * event);
    //! Checks if specified name already exists.
    bool canOverwrite();

    QLabel *m_icon;
    KexiNameWidget* m_widget;
    const KexiProject *m_project;
    const KexiPart::Part *m_part;
    bool m_checkIfObjectExists;
    bool m_allowOverwriting;
    bool *m_overwriteNeeded;
};

#endif
