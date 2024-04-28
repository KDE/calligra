/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "EmfViewer.h"

#include <QApplication>

using namespace Libemf;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QSize size(1280, 800);
    EmfViewer viewer(size);
    viewer.show();

    if (argc > 1) {
        QString filename(argv[1]);
        viewer.loadFile(filename);
    }

    app.exec();
}
