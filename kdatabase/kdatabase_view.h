/* This file is part of the KDE project
   Copyright (C) 2002 Chris Machemer <machey@ceinetworks.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KDATABASE_VIEW
#define KDATABASE_VIEW

#include <koView.h>
#include "maindlg2.h"
#include "clstbldesigner2.h"

class KAction;
class KToggleAction;
class QPaintEvent;

class KDatabasePart;

class KDatabaseView : public KoView
{
    Q_OBJECT
public:
    KDatabaseView( KDatabasePart* part, QWidget* parent = 0, const char* name = 0 );

protected slots:
    void cut();
    void copy();
    void paste();
    void toggleDocBrowser();

protected:
    void paintEvent( QPaintEvent* );

    virtual void updateReadWrite( bool readwrite );

private:
    //KAction* m_cut;
    KToggleAction *m_actionToggleDocBrowser;
    MainDlg2* myMainDlg;
    
    bool m_docBrowserVisible;
};

#endif
