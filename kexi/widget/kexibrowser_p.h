/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIBROWSER_P_H
#define KEXIBROWSER_P_H

#include <QPointer>
#include <K3ListView>
#include <KMenu>
#include <KActionCollection>

/*! @internal */
class KexiBrowserListView : public K3ListView
{
  Q_OBJECT
  public:
    KexiBrowserListView(QWidget *parent);
    virtual ~KexiBrowserListView();
    
    virtual bool isExecuteArea( const QPoint& point );

    bool nameEndsWithAsterisk : 1;
    bool enableExecuteArea : 1; //!< used in isExecuteArea()
  public slots:
    virtual void rename(Q3ListViewItem *item, int c);
  protected:
};

/*! @internal */
class KexiMenuBase : public KMenu
{
  public:
    KexiMenuBase(QWidget *parent, KActionCollection *collection);
    ~KexiMenuBase();

    QAction* addAction(const QString& actionName);

  protected:
    QPointer<KActionCollection> m_actionCollection;
};

/*! @internal */
class KexiItemMenu : public KexiMenuBase
{
  public:
    KexiItemMenu(QWidget *parent, KActionCollection *collection);
    ~KexiItemMenu();

    //! Rebuilds the menu entirely using infromation obtained from \a partInfo 
    //! and \a partItem.
    void update(KexiPart::Info* partInfo, KexiPart::Item* partItem);
};

/*! @internal */
class KexiGroupMenu : public KexiMenuBase
{
  public:
    KexiGroupMenu(QWidget *parent, KActionCollection *collection);
    ~KexiGroupMenu();
  
    //! Rebuilds the menu entirely using infromation obtained from \a partInfo.
//unused		void update(KexiPart::Info* partInfo);
};

#endif
