/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#ifndef __KWEditPersonnalExpression__
#define __KWEditPersonnalExpression__

#include <kdialogbase.h>
#include <qmap.h>

class QPushButton;
class QComboBox;
class QListBox;
class QLineEdit;

class KWEditPersonnalExpression : public KDialogBase
{
    Q_OBJECT
public:
    KWEditPersonnalExpression( QWidget *parent, const char *name = 0 );

protected:
    void init(const QString& filename );
    void loadFile();
    void initGroupList();
    void saveFile();
    void updateWidget();
    void updateExpression();

protected slots:
    void slotGroupSelected();
    void slotExpressionSelected();
    void slotUpdateGroupName(const QString &newGroupName);
    void slotUpdateExpression(const QString &newExpression);
    void slotRemoveExpression();
    void slotAddExpression();
    void slotRemoveGroup();
    void slotAddGroup();
    virtual void slotOk();

 private:
    typedef QMap<QString,QStringList> list;
    list listExpression;
    QListBox *m_groupList;
    QListBox *m_ExpressionsList;
    QPushButton *m_addGroup;
    QPushButton *m_delGroup;
    QPushButton *m_addExpression;
    QPushButton *m_delExpression;
    QLineEdit *m_groupLineEdit;
    QLineEdit *m_expressionLineEdit;
    bool m_bChanged;
};

#endif
