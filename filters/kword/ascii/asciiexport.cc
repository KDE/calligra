/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>
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

#include <qiodevice.h>
#include <qtextstream.h>
#include <qdom.h>

#include <kdebug.h>

#include <KWEFStructures.h>
#include <TagProcessing.h>
#include <KWEFBaseClass.h>
#include <ProcessDocument.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include <asciiexport.h>
#include <asciiexport.moc>

class ASCIIWorker : public KWEFBaseWorker
{
public:
    ASCIIWorker(void) : m_ioDevice(NULL), m_streamOut(NULL) { }
    virtual ~ASCIIWorker(void) { }
public:
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void); // Close file in normal conditions
    virtual bool doOpenDocument(void);
    virtual bool doCloseDocument(void);
    virtual bool doFullParagraph(QString& paraText, LayoutData& layout, ValueListFormatData& paraFormatDataList);
private:
    void ProcessParagraphData (const QString& paraText, ValueListFormatData& paraFormatDataList);
private:
    QIODevice* m_ioDevice;
    QTextStream* m_streamOut;
};

bool ASCIIWorker::doOpenFile(const QString& filenameOut, const QString& to)
{
    kdDebug(30503) << "Entering ASCIIWorker::doOpenFile" << endl;
    if ( to != "text/plain" )
    {
        kdError(30502) << "Unexpected mime type " << to <<" Aborting!" << endl;
        return false;
    }

    m_ioDevice=new QFile(filenameOut);

    if (!m_ioDevice)
    {
        kdError(30502) << "No output file! Aborting!" << endl;
        return false;
    }

    if ( !m_ioDevice->open (IO_WriteOnly) )
    {
        kdError(30502) << "Unable to open output file!" << endl;
        return false;
    }

    m_streamOut=new QTextStream(m_ioDevice);
    if (!m_ioDevice)
    {
        kdError(30502) << "Could not create output stream! Aborting!" << endl;
        m_ioDevice->close();
        return false;
    }

    // TODO: ask the user for the encoding!
    m_streamOut->setEncoding( QTextStream::Locale );
    kdDebug(30503) << "Exiting ASCIIWorker::doOpenFile" << endl;
    return true;
}

bool ASCIIWorker::doCloseFile(void)
{
    if (m_ioDevice)
        m_ioDevice->close();
    return (m_ioDevice);
}

bool ASCIIWorker::doOpenDocument(void)
{
    // We have nothing to do, but to give our OK to continue
    return true;
}

bool ASCIIWorker::doCloseDocument(void)
{
    // We have nothing to do, but to give our OK to continue
    return true;
}

// ProcessParagraphData () mangles the pure text through the
// formatting information stored in the FormatData list and prints it
// out to the export file.

void ASCIIWorker::ProcessParagraphData (const QString& paraText,
    ValueListFormatData& paraFormatDataList)
{
    if ( paraText.length () > 0 )
    {
        ValueListFormatData::Iterator  paraFormatDataIt;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            *m_streamOut << paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );
        }
    }

    *m_streamOut << "\n";
}

bool ASCIIWorker::doFullParagraph(QString& paraText, LayoutData& layout, ValueListFormatData& paraFormatDataList)
{
    kdDebug(30503) << "Entering ASCIIWorker::doFullParagraph" << endl;
    QString paraLayout=layout.styleName;

    if ( paraLayout == "Head 1" )
    {
        *m_streamOut << "###################################\n";
        *m_streamOut << "# ";
        ProcessParagraphData ( paraText, paraFormatDataList);
        *m_streamOut << "###################################\n";
    }
    else if ( paraLayout == "Head 2" )
    {
        *m_streamOut << "#### ";
        ProcessParagraphData ( paraText, paraFormatDataList);
    }
    else if ( paraLayout == "Head 3" )
    {
        *m_streamOut << "## ";
        ProcessParagraphData ( paraText, paraFormatDataList);
    }
    else if ( paraLayout == "Bullet List" )
    {
        *m_streamOut << "o ";
        ProcessParagraphData ( paraText, paraFormatDataList);
    }
    else if ( paraLayout == "Enumerated List" )
    {
        *m_streamOut << "1. ";   // less than perfect
        ProcessParagraphData ( paraText, paraFormatDataList);
    }
    else if ( paraLayout == "Alphabetical List" )
    {
        *m_streamOut << "a) ";   // less than perfect
        ProcessParagraphData ( paraText, paraFormatDataList);
    }
    else
    {
        if ( paraLayout != "Standard" )
        {
            kdError(30502) << "Unknown layout " + paraLayout + "!" << endl;
        }

        ProcessParagraphData ( paraText, paraFormatDataList);
    }
    kdDebug(30503) << "Exiting ASCIIWorker::doFullParagraph" << endl;
    return true;
}


ASCIIExport::ASCIIExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name)
{
}

bool ASCIIExport::filter(const QString  &filenameIn,
                         const QString  &filenameOut,
                         const QString  &from,
                         const QString  &to,
                         const QString  &param )
{
    if ( to != "text/plain" || from != "application/x-kword" )
    {
        return false;
    }

    ASCIIWorker* worker=new ASCIIWorker();

    if (!worker)
    {
        kdError(30502) << "Cannot create Worker! Aborting!" << endl;
        return false;
    }

    KWEFKWordLeader* leader=new KWEFKWordLeader(worker);

    if (!leader)
    {
        kdError(30502) << "Cannot create Worker! Aborting!" << endl;
        delete worker;
        return false;
    }

    bool flag=leader->filter(filenameIn,filenameOut,from,to,param);

    delete leader;
    delete worker;

    return flag;
}
