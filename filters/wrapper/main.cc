/* This file is part of the KDE project
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

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

#include <main.h>
#include <iostream>
#include <stdlib.h>

#include <qvaluelist.h>
#include <qapplication.h>
#include <kinstance.h>
#include <koFilter.h>
#include <koQueryTrader.h>
#include <ktempfile.h>

/**
 * This small wrapper program is used to invoke KOffice filters as a
 * separate process. This is quite nice for "alpha-quality" filters which
 * tend to crash from time to time. This wrapper can only be used if you
 * use the most simple of all possibilities to im/export files (via two
 * temporary files). To tell the filter manager that you'd like to use this
 * wrapper, please simply replace "Implemented=file" with
 * "Implemented=bulletproof" in your filter's .desktop file.
 * Details: The wrapper gets the information about filtering from the filter
 * manager as commandline args and prints progress and the name of the
 * temp file to stdout.
 * Return codes: 0 = OK, 1 = Wrong number of cmdline args, 2 = Error on
 * reading the cmdline args, 3 = The query didn't return enough results,
 * 4 = Couldn't create the filter, 5 = unable to open the temp. file, 6 = Error
 * on filtering (the filter didn't return ok)
 */

void WrapperProgress::slotProgress(int p) {
    std::cout << "P" << p << endl;
}

int main(int argc, char **argv) {

    if(argc!=8)
        exit(1);

    QString constr(argv[1]);
    QString file(argv[2]);
    QString storePrefix(argv[3]);
    QString mimeType(argv[4]);
    QString _native_format(argv[5]);
    QString config(argv[6]);
    QString number(argv[7]);
    bool ok=true;
    unsigned int index=number.toUInt(&ok);
    if(!ok)
        exit(2);
    QString tempfname;

    KInstance instance("filter_wrapper");
    QValueList<KoFilterEntry> vec=KoFilterEntry::query(constr);
    if(index>=vec.count())
        exit(3);

    KoFilter* filter=vec[index].createFilter();
    if(!filter)
        exit(4);

    WrapperProgress *prog=new WrapperProgress();
    QObject::connect(filter, SIGNAL(sigProgress(int)), prog, SLOT(slotProgress(int)));

    KTempFile tempFile; // create with default file prefix, extension and mode
    if(tempFile.status()!=0)
        exit(5);
    tempfname=tempFile.name();

    // gah... we have to create a QApplication for QFont and Co. :}
    QApplication app(argc, argv);

    if (filter->supportsEmbedding())
        ok=filter->filter( file, tempfname, storePrefix, mimeType, _native_format, config );
    else
        ok=filter->filter( file, tempfname, mimeType, _native_format, config );
    if(!ok)
        exit(6);

    std::cout << "F" << tempfname.latin1() << endl;
    delete prog;
    delete filter;
    return 0; // success :)
}

#include <main.moc>
