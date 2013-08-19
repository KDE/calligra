/*
 *   kassistantdialogtest - a test program for the KAssistantDialog class
 *   Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <QApplication>
#include <QLabel>
#include <QLayout>
//Added by qt3to4:
#include <QBoxLayout>
#include <kassistantdialog.h>

int main(int argc, char **argv)
{
  QApplication::setApplicationName(QStringLiteral("test"));
  QApplication a(argc, argv);
  KAssistantDialog *dlg = new KAssistantDialog();
  QObject::connect(dlg, SIGNAL(finished(int)),  &a, SLOT(quit()));
  for(int i = 1; i < 11; i++)
  {
    QWidget *p = new QWidget;
    QString msg = QStringLiteral("This is page %1 out of 10").arg(i);
    QLabel *label = new QLabel(msg, p);
    QHBoxLayout *layout = new QHBoxLayout(p);
    label->setAlignment(Qt::AlignCenter);
    label->setFixedSize(300, 200);
    layout->addWidget(label);
    QString title = QStringLiteral("%1. page").arg(i);
    dlg->addPage(p, title);
  }

  dlg->show();
  return a.exec();
}
