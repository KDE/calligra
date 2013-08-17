/*
Copyright (c) 2005 Helge Deller <deller@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "test9.h"
#include <QtCore/QDir>
#include <QApplication>
#include <QDebug>

int main( int argc, char **argv )
{
  QApplication app(argc, argv);
  Q_UNUSED(app);
  Test9 *t = new Test9( QString(), QString() );

  QStringList myPathsList2 = t->myPathsList2();
  qWarning() << myPathsList2;

  // add another path
  QStringList newlist;
  myPathsList2 << QDir::homePath() + QString::fromLatin1("/.kde");
  qWarning() << myPathsList2;

  t->setMyPathsList2(myPathsList2);
  qWarning() << t->myPathsList2();

  delete t;
  return 0;
}
