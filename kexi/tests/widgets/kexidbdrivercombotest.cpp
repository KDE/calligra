/***************************************************************************
 *   This file is part of the KDE project                                  *
 *   Copyright (C) 2005 Martin Ellis <martin.ellis@kdemail.net>            *
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/

#include <qlayout.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <kdebug.h>
#include <kcmdlineargs.h>
#include <kapplication.h>
#include <kcomponentdata.h>

#include <kexidb/drivermanager.h>
#include <widget/kexidbdrivercombobox.h>

/*
   This is an example of the KexiDBDriverComboBox class, used to 
   allow the user to pick a database driver.

   When run it shows two comboboxes. The top one allows the user to
   pick any database driver. The second allows the user to pick
   any of the drivers for database servers (i.e. it does not include
   file based drivers).
*/

int main(int argc, char** argv)
{
  // Initialise the program
  KCmdLineArgs::init(argc, argv, "kexidbcomboboxtest", "", "", "", true);
  KApplication* app = new KApplication(true, true);

  // Look for installed database drivers
  KexiDB::DriverManager manager;
  KexiDB::Driver::InfoMap drvs = manager.driversInfo();

  // Set up a combo box and a quit widget in a new container
  QWidget* vbox = new QWidget();
  Q3VBoxLayout* vbLayout = new Q3VBoxLayout(vbox);

  KexiDBDriverComboBox* all = new KexiDBDriverComboBox(vbox, drvs);
  KexiDBDriverComboBox* srvOnly = new KexiDBDriverComboBox(vbox, drvs,
  	KexiDBDriverComboBox::ShowServerDrivers);

  QPushButton* quit = new QPushButton("Quit", vbox);

  vbLayout->addWidget(all);     // Combobox listing all drivers
  vbLayout->addWidget(srvOnly); // Combobox only drivers for DB servers
  vbLayout->addWidget(quit);

  // Show the whole lot
  QObject::connect(quit, SIGNAL(clicked()), app, SLOT(quit()));
  vbox->show();
  app->exec();

  delete app;
}

