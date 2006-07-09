/* This file is part of the KDE project
   Copyright (C) 1999-2005 Laurent Montel <montel@kde.org>

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

#ifndef __kspread_dlg_find__
#define __kspread_dlg_find__


#include <kfinddialog.h>
#include <QStringList>
#include <kreplacedialog.h>

class QCheckBox;
class QPushButton;
class QComboBox;

namespace KSpread
{

class FindOption : public QObject
{
  Q_OBJECT
 public:
    FindOption( QWidget *parent);
    bool searchInAllSheet() const;

    enum searchTypeValue { Value, Note};
    enum searchDirectionValue { Row, Column};

    searchTypeValue searchType() const;
    searchDirectionValue searchDirection() const;

private slots:
    void slotMoreOptions();

signals:
    void adjustSize();

private:
    QPushButton *m_moreOptions;
    QCheckBox *m_searchInAllSheet;
    QWidget *m_findExtension;
    QComboBox *m_searchIn;
    QComboBox *m_searchDirection;
};

class FindDlg : public KFindDialog
{
  Q_OBJECT

 public:
    FindDlg( QWidget *parent = 0, const char *name = 0, long options = 0, const QStringList &findStrings = QStringList(), bool hasSelection = false );
    ~FindDlg();
    bool searchInAllSheet() const;
    FindOption::searchTypeValue searchType() const { return m_findOptions->searchType(); }
    FindOption::searchDirectionValue searchDirection() const { return m_findOptions->searchDirection(); }

 private slots:
    void slotAjustSize();
 private:
    FindOption *m_findOptions;
};

class SearchDlg : public KReplaceDialog
{
  Q_OBJECT

 public:
    SearchDlg( QWidget *parent = 0, const char *name = 0, long options = 0, const QStringList &findStrings = QStringList(), const QStringList &replaceStrings = QStringList(), bool hasSelection = false );
    ~SearchDlg();
    bool searchInAllSheet() const;
    FindOption::searchTypeValue searchType() const { return m_findOptions->searchType(); }

 private slots:
    void slotAjustSize();
 private:
    FindOption *m_findOptions;
};

} // namespace KSpread

#endif
