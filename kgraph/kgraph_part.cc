/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@carinthia.com>

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

#include <qpainter.h>

//#include <kaction.h>
#include <kstdaction.h>
#include <kdebug.h>

#include <kgraph_view.h>
#include <kgraph_shell.h>
#include <kgraph_factory.h>
#include <kgraph_part.h>
#include <kgobjectpool.h>
#include <kggrouppool.h>


KGraphPart::KGraphPart(QObject *parent, const char *name, bool singleViewMode)
    : KoDocument(parent, name, singleViewMode) {

    groupPool=new KGGroupPool();
    objectPool=new KGObjectPool(this, groupPool);

    KStdAction::cut(this, SLOT( edit_cut() ), actionCollection(), "edit_cut" );
}

KGraphPart::~KGraphPart() {

    delete objectPool;
    objectPool=0L;
    delete groupPool;
    groupPool=0L;
}

bool KGraphPart::initDoc() {
    // If nothing is loaded, do initialize here (TODO)
    return true;
}

QCString KGraphPart::mimeType() const {
    return "application/x-kgraph";
}

KoView *KGraphPart::createView(QWidget *parent, const char *name) {

    KGraphView *view = new KGraphView(this, objectPool, parent, name);
    addView(view);
    return view;
}

KoMainWindow *KGraphPart::createShell() {

    KoMainWindow *shell = new KGraphShell;
    shell->setRootDocument(this);
    shell->show();
    return shell;
}

void KGraphPart::paintContent(QPainter &/*painter*/, const QRect &/*rect*/, bool /*transparent*/) {

    kdDebug(37001) << "KGraphPart::painEvent()" << endl;

    /*// ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.
    int left=rect.left()/20;
    int right=rect.right()/20+1;
    int top=rect.top()/20;
    int bottom=rect.bottom()/20+1;

    for(int x=left; x<right; ++x)
        painter.drawLine(x*20, top*20, x*20, bottom*20);
    for(int y=left; y<right; ++y)
        painter.drawLine(left*20, y*20, right*20, y*20);
    */
}

void KGraphPart::edit_cut() {
    kdDebug(37001) << "KGraphPart: edit_cut called" << endl;
}
#include <kgraph_part.moc>
