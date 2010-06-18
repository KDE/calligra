/* This file is part of the KDE project
   Copyright (C) 2006 Robert Knight <robertknight@gmail.com>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Werner Trobin <trobin@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2000 David Faure <faure@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_SORT_DIALOG
#define KSPREAD_SORT_DIALOG


#include <kdialog.h>
#include <QStringList>
//Added by qt3to4:
#include <QLabel>

class QCheckBox;
class KComboBox;
class QLabel;
class KLineEdit;
class QRadioButton;
class KTabWidget;
class QWidget;

namespace KSpread
{
class Selection;
class Sheet;

/**
 * \ingroup UI
 * Dialog to set options for the sort cell values command.
 */
class SortDialog : public KDialog
{
    Q_OBJECT

public:
    enum Orientation {
        SortColumns = 0,
        SortRows = 1
    };

    explicit SortDialog(QWidget* parent, Selection* selection);
    ~SortDialog();

protected:
    Orientation guessDataOrientation();
    QRect       sourceArea();

private slots:
    void sortKey2textChanged(int);
    void useCustomListsStateChanged(int);
    void firstRowHeaderChanged(int);
    virtual void slotOk();
    void slotOrientationChanged(int id);

private:
    void init();

    Selection    * m_selection;

    QStringList    m_listColumn;
    QStringList    m_listRow;

    QWidget      * m_page1;
    QWidget      * m_page2;

    KTabWidget   * m_tabWidget;

    KComboBox    * m_sortKey1;
    KComboBox    * m_sortOrder1;
    KComboBox    * m_sortKey2;
    KComboBox    * m_sortOrder2;
    KComboBox    * m_sortKey3;
    KComboBox    * m_sortOrder3;

    QCheckBox    * m_useCustomLists;
    KComboBox    * m_customList;

    QRadioButton * m_sortColumn;
    QRadioButton * m_sortRow;

    QCheckBox    * m_copyLayout;
    QCheckBox    * m_firstRowOrColHeader;
    QCheckBox    * m_respectCase;

    /*KComboBox    * m_outputSheet;
    KLineEdit    * m_outputCell;*/
};

} // namespace KSpread

#endif // KSPREAD_SORT_DIALOG
