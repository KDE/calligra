// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

#include <KWEFBaseWorker.h>

class KWEFKWordLeader
{
    public:
        KWEFKWordLeader(void) : m_worker (0 /*NULL*/) {}
        KWEFKWordLeader(KWEFBaseWorker* newWorker) : m_worker (newWorker)
        { if (newWorker) newWorker->registerKWordLeader(this); }
        virtual ~KWEFKWordLeader(void) {}

    public:
        void setWorker ( KWEFBaseWorker *newWorker );
        KWEFBaseWorker *getWorker(void) const;
        bool filter ( const QString& filenameIn, const QString& filenameOut,
                      const QString& from, const QString& to, const QString& param );
    public: // callbacks
        bool loadKoStoreFile(const QString& fileName, QByteArray& array);
    public: // public leader/worker functions (DO NOT use in your own code!)
        bool doFullDocumentInfo (const KWEFDocumentInfo &docInfo);
        bool doFullDocument (const QValueList<ParaData> &, QString &, QString &);
        bool doFullPaperFormat (const int format, const double width, const double height, const int orientation);
        bool doOpenHead (void);
        bool doCloseHead (void);
        bool doOpenBody (void);
        bool doCloseBody (void);
        bool doOpenStyles (void);
        bool doCloseStyles (void);
        bool doFullDefineStyle (LayoutData& layout);
    protected: // leader/worker functions
        bool doOpenFile (const QString& filenameOut, const QString& to);
        bool doCloseFile (void);
        bool doAbortFile (void);
        bool doOpenDocument (void);
        bool doCloseDocument (void);
    private:
        KWEFBaseWorker *m_worker;
        QString m_filenameIn;
};

#endif /* KWEF_KWORDLEADER_H */
