// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001. 2002 Nicolas GOUTTE <nicog@snafu.de>

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

#ifndef KWEF_KWORDLEADER_H
#define KWEF_KWORDLEADER_H

#include <qiodevice.h>

#include <koFilterChain.h>

#include <KWEFBaseWorker.h>

class KWEFKWordLeader
{
    public:
        KWEFKWordLeader(void) : m_worker(NULL), m_chain(NULL) {}
        KWEFKWordLeader(KWEFBaseWorker* newWorker) : m_worker(newWorker), m_chain(NULL)
        { if (newWorker) newWorker->registerKWordLeader(this); }
        virtual ~KWEFKWordLeader(void) {}

    public:
        void setWorker ( KWEFBaseWorker *newWorker );
        KWEFBaseWorker *getWorker(void) const;
        KoFilter::ConversionStatus convert( KoFilterChain* chain,
            const QCString& from, const QCString& to);
    public: // callbacks
        bool loadSubFile(const QString& fileName, QByteArray& array);
        QIODevice* getSubFileDevice(const QString& fileName);
    public: // public leader/worker functions (DO NOT use in your own code!)
        bool doFullDocumentInfo (const KWEFDocumentInfo &docInfo);
        bool doFullDocument (const QValueList<ParaData> &);
        bool doFullPaperFormat (const int format, const double width, const double height, const int orientation);
        bool doFullPaperBorders (const double top, const double left, const double bottom, const double right);
        bool doOpenHead (void);
        bool doCloseHead (void);
        bool doOpenBody (void);
        bool doCloseBody (void);
        bool doOpenStyles (void);
        bool doCloseStyles (void);
        bool doFullDefineStyle (LayoutData& layout);
        bool doOpenSpellCheckIgnoreList (void);
        bool doCloseSpellCheckIgnoreList (void);
        bool doFullSpellCheckIgnoreWord (const QString& ignoreword);
    protected: // leader/worker functions
        bool doOpenFile (const QString& filenameOut, const QString& to);
        bool doCloseFile (void);
        bool doAbortFile (void);
        bool doOpenDocument (void);
        bool doCloseDocument (void);
    private:
        KWEFBaseWorker *m_worker;
        KoFilterChain* m_chain;
};

#endif /* KWEF_KWORDLEADER_H */
