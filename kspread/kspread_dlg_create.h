/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Montel Laurent <montell@club-internet.fr>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kspread_dlg_create__
#define __kspread_dlg_create__

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>




class KSpreadView;
class KSpreadTable;
class KSpreadCell;

class KSpreadcreate : public QDialog
{
  Q_OBJECT
public:
  KSpreadcreate( KSpreadView* parent, const QString& _name );

  QString create_formula();
  QString make_formula( const QString& _text, int nb_line);
  void setText(QString text);
  KSpreadView *view(){return m_pView;}
  int nb_button(){return button_select;}

  QString tableName() const { return m_tableName; }

  void set_nbbutton(int nb){ button_select=nb;}
  int paramCount() const { return nb_param; }
    
private slots:
  void slotOk();
  void slotClose();
  void slotFselect();
  void slotSselect();
  void slotTselect();
  void slotFOselect();
  void slotFIselect();

private:
    void init();

    int nb_param;
  int first_element;

  int dx;
  int dy;

  enum type_create {type_double,type_string,type_logic};

  QString old_text;
  KSpreadView* m_pView;
  QString name;
  QString m_tableName;
  QLineEdit* f_param;
  QLineEdit* s_param;
  QLineEdit* t_param;
  QLineEdit* fo_param;
  QLineEdit* fi_param;
  QPushButton* m_pOk;
  QPushButton* m_pClose;
  QPushButton* f_select;
  QPushButton* s_select;
  QPushButton* t_select;
  QPushButton* fo_select;
  QPushButton* fi_select;

  type_create edit[5];
  int button_select;
};

#endif
