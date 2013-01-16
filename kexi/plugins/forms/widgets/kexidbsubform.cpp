/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jarosław Staniek <staniek@kde.org>

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

#include "kexidbsubform.h"

#include "kexidbform.h"
#include "kexiformmanager.h"
#include "kexiformview.h"
#include <db/utils.h>
#include <KexiMainWindowIface.h>
#include <kexiutils/utils.h>
#include <formeditor/formIO.h>
#include <formeditor/objecttree.h>
#include <formeditor/utils.h>
#include <formeditor/container.h>
//2.0 #include <formeditor/formmanager.h>
//Added by qt3to4:
#include <Q3Frame>
#include <QSet>

KexiDBSubForm::KexiDBSubForm(KFormDesigner::Form *parentForm, QWidget *parent)
        : Q3ScrollView(parent), m_parentForm(parentForm), m_form(0), m_widget(0)
{
    setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
    QPalette pal(viewport()->palette());
    pal.setBrush(viewport()->backgroundRole(), pal.brush(QPalette::Mid));
    viewport()->setPalette(pal);
}
/*
void
KexiDBSubForm::paintEvent(QPaintEvent *ev)
{
  QScrollView::paintEvent(ev);
  QPainter p;

  setWFlags(WPaintUnclipped);

  QString txt("Subform");
  QFont f = font();
  f.setPointSize(f.pointSize() * 3);
  QFontMetrics fm(f);
  const int txtw = fm.width(txt), txth = fm.height();

  p.begin(this, true);
  p.setPen(black);
  p.setFont(f);
  p.drawText(width()/2, height()/2, txt, Qt::AlignCenter|Qt::AlignVCenter);
  p.end();

  clearWFlags( WPaintUnclipped );
}
*/
void
KexiDBSubForm::setFormName(const QString &name)
{
    if (m_formName == name)
        return;

    m_formName = name; //assign, even if the name points to nowhere

    if (name.isEmpty()) {
        delete m_widget;
        m_widget = 0;
        updateScrollBars();
        return;
    }

    QWidget *pw = parentWidget();
    KexiFormView *view = 0;
    QSet<QString> names;
    while (pw) {
        if (KexiUtils::objectIsA(pw, "KexiDBSubForm")) {
            if (names.contains(pw->objectName())) {
//! @todo error message
                return; // Be sure to don't run into a endless-loop cause of recursive subforms.
            }
            names.insert(pw->objectName());
        } else if (! view && KexiUtils::objectIsA(pw, "KexiFormView")) {
            view = static_cast<KexiFormView*>(pw); // we need a KexiFormView*
        }
        pw = pw->parentWidget();
    }

    if (!view || !view->window() || !KexiMainWindowIface::global()->project()->dbConnection())
        return;

    KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();

    // we check if there is a form with this name
    int id = KexiDB::idForObjectName(*conn, name, KexiPart::FormObjectType);
    if ((id == 0) || (id == view->window()->id())) // == our form
        return; // because of recursion when loading

    // we create the container widget
    delete m_widget;
    m_widget = new QWidget(viewport());
    m_widget->setObjectName("KexiDBSubForm_widget");
    m_widget->show();
    addChild(m_widget);
    m_form = new KFormDesigner::Form(m_parentForm);
    m_form->setObjectName(QString("KFormDesigner::Form_") + objectName());
    m_form->createToplevel(m_widget);

    // and load the sub form
    QString data;
    tristate res = conn->loadDataBlock(id, data, QString());
    if (res == true)
        res = KFormDesigner::FormIO::loadFormFromString(m_form, m_widget, data);
    if (res != true) {
        delete m_widget;
        m_widget = 0;
        updateScrollBars();
        m_formName.clear();
        return;
    }
    m_form->setMode(KFormDesigner::Form::DataMode);

    // Install event filters on the whole newly created form
    KFormDesigner::ObjectTreeItem *tree = m_parentForm->objectTree()->lookup(QObject::objectName());
    KFormDesigner::installRecursiveEventFilter(this, tree->eventEater());
}

#include "kexidbsubform.moc"
