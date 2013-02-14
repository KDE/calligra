/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KRITAPROGRAM_H
#define KRITAPROGRAM_H

#include "kis_about.h"

class QSplashScreen;

namespace Kasten2
{

class DialogHandler;
class DocumentManager;
class ViewManager;
class MultiDocumentStrategy;

class KritaProgram
{
public:
    KritaProgram( int argc, char* argv[] );
    ~KritaProgram();

public:
    int execute();
    void quit();

public:

    DocumentManager* documentManager();
    ViewManager* viewManager();
    MultiDocumentStrategy* documentStrategy();

private:

    KritaAboutData mAboutData;

    DocumentManager* mDocumentManager;
    ViewManager* mViewManager;
    MultiDocumentStrategy* mDocumentStrategy;

    DialogHandler* mDialogHandler;

    QSplashScreen *m_splashScreen;

};


inline DocumentManager* KritaProgram::documentManager() {
    return mDocumentManager;
}

inline ViewManager* KritaProgram::viewManager() {
    return mViewManager;
}

inline MultiDocumentStrategy* KritaProgram::documentStrategy() {
    return mDocumentStrategy;
}


}

#endif
