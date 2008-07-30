/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIFILTERDLG_H
#define KEXIFILTERDLG_H

#include <qdialog.h>

class QPushButton;
class Q3ListView;
class Q3ListViewItem;
class KexiProject;

class KEXIEXTWIDGETS_EXPORT KexiFilterDlg : public QDialog
{
  Q_OBJECT

  public:
    KexiFilterDlg(KexiProject *p, QWidget *parent=0, const char *name=0);
    ~KexiFilterDlg();

    QPushButton	*createMiniButton(const QString &text);
    void		setupCatalog(const QStringList &mimes);
    void		setupCatalog(const QString &mime);

  protected slots:
    void		insert(Q3ListViewItem *);

  protected:
    Q3ListView	*m_catalog;
    KexiProject	*m_project;
};

#endif
