/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef ADD_SPAWNER_SET_DLG_H
#define ADD_SPAWNER_SET_DLG_H

#include "tkaction.h"
#include <kpopupmenu.h>

#include <qstring.h>
#include <qdialog.h>
#include <qlist.h>

class QPopupMenu;

class AddSpawnerSetAction : public TKAction
{ Q_OBJECT
public:
  AddSpawnerSetAction( const QString& text, const QString&, int accel, QObject* parent, const char* name = 0 );

  virtual ~AddSpawnerSetAction();

signals:
  void activated( const QString& );

public slots:
  void updateMenu();

protected:
  virtual void initToolBarButton(TKToolBarButton*);

  QString dirDesc( const QString& dir );
  QPixmap dirtPixmap( const QString& dir );
  void loadCollections( const QString& rootDirStr );
  void loadSet( QPopupMenu* menu, const QString& rootDirStr );

protected slots:
  void slotActivated(int);

private:
  uint m_id;
  QPopupMenu* m_pPopupMenu;
  QList<QPopupMenu> childMenuList;
  QList<QString> pathList;
};
/************************************************************************************************/
class QListView;
class QListViewItem;
class QPushButton;

class AddSpawnerSetDlg : public QDialog
{
    Q_OBJECT

protected:
    QString m_rootDir;
    QListView *m_pListView;
    QPushButton *m_ok;

    QString dirDesc( QString );
    void setPixmap( QString, QListViewItem * );
    void loadCollections( QListView *, QString &  );
    void loadSet( QListView *, QListViewItem *, const QString & );
//    void loadSpawnerInfo( QListView * );

protected slots:
    void slotOk();
    void itemSelected( QListViewItem * );

public:
    AddSpawnerSetDlg( QWidget *, const char *name, QString );
    virtual ~AddSpawnerSetDlg();

    QString spawnerSetName();
};

#endif


