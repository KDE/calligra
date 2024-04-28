/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <EnhMetaFile.h>
#include <QApplication>
#include <QDebug>

using namespace EnhancedMetafile;

bool testOneFile(const QString &filename)
{
    Parser parser;
    PainterOutput output;
    parser.setOutput(&output);
    if (parser.load(filename) == false) {
        debugVectorImage() << "failed to load" << filename;
        return false;
    } else {
        debugVectorImage() << "successfully loaded" << filename;
        return true;
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QStringList filesToTest;
    filesToTest << "pyemf-1.emf"
                << "pyemf-arc-chord-pie.emf"
                << "pyemf-deleteobject.emf";
    filesToTest << "pyemf-drawing1.emf"
                << "pyemf-fontbackground.emf"
                << "pyemf-optimize16bit.emf";
    filesToTest << "pyemf-paths1.emf"
                << "pyemf-poly1.emf"
                << "pyemf-poly2.emf"
                << "pyemf-setpixel.emf";
    filesToTest << "snp-1.emf"
                << "snp-2.emf"
                << "snp-3.emf";
    filesToTest << "visio-1.emf";

    foreach (const QString &fileToTest, filesToTest) {
        if (testOneFile(fileToTest) == false) {
            return -1;
        }
    }
    return 0;
}
