/* This file is part of the KDE project
  Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
  Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>

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

#ifndef KPTPERTEDITOR_H
#define KPTPERTEDITOR_H

#include <kptviewbase.h>
#include <kptitemmodelbase.h>

#include <QMenu>
#include <QPainter>
#include <QStyle>
#include <QList>
#include <QHeaderView>
#include <QObject>
#include <QTreeWidget>
#include <QStringList>
#include <QVBoxLayout>
#include <QTreeWidget>

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QSpinBox>
#include <QtGui/QLineEdit>

#include <kactionselector.h>

#include "kptcontext.h"

class QPoint;
class QTreeWidgetItem;
class QSplitter;

class KToggleAction;
class KPrinter;

namespace KPlato
{

class View;
class Project;

class PertEditor : public ViewBase
{
    Q_OBJECT
public:

    PertEditor( Part *part, QWidget *parent );

    void setupGui();

private:
    KActionSelector * assignList;
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QTableWidget *tableTaskWidget;
    QVBoxLayout *vboxLayout1;
    QLabel *label1;

};

}  //KPlato namespace

#endif
