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

#include <kdebug.h>

#include <KWEFStructures.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include <ExportDialog.h>
#include <asciiexport.h>
#include <asciiexport.moc>

class ASCIIWorker : public KWEFBaseWorker
{
public:
    ASCIIWorker(void) : m_ioDevice(NULL), m_streamOut(NULL), m_eol("\n"),
        m_inList(false) { }
    virtual ~ASCIIWorker(void) { }
public:
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void); // Close file in normal conditions
    virtual bool doOpenDocument(void);
    virtual bool doCloseDocument(void);
    virtual bool doFullParagraph(QString& paraText, LayoutData& layout, ValueListFormatData& paraFormatDataList);
public:
    inline bool isUTF8 (void) const { return m_utf8; }
    inline void setUTF8 (const bool flag ) { m_utf8=flag; }
    inline QString getEndOfLine(void) const {return m_eol;}
    inline void setEndOfLine(const QString& str) {m_eol=str;}
private:
    void ProcessParagraphData (const QString& paraText, ValueListFormatData& paraFormatDataList);
private:
    QIODevice* m_ioDevice;
    QTextStream* m_streamOut;
    QString m_eol; // End of line character(s)
    CounterData::Style m_typeList; // What is the style of the current list (undefined, if we are not in a list)
    bool m_inList; // Are we currently in a list?
    bool m_orderedList; // Is the current list ordered or not (undefined, if we are not in a list)
    int  m_counterList; // Counter for te lists
    bool m_utf8;
};

bool ASCIIWorker::doOpenFile(const QString& filenameOut, const QString& to)
{
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

    if (isUTF8())
    {
        m_streamOut->setEncoding( QTextStream::UnicodeUTF8 );
    }
    else
    {
        m_streamOut->setEncoding( QTextStream::Locale );
    }

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

    *m_streamOut << m_eol; // Write end of line
}

bool ASCIIWorker::doFullParagraph(QString& paraText, LayoutData& layout, ValueListFormatData& paraFormatDataList)
{
    kdDebug(30503) << "Entering ASCIIWorker::doFullParagraph" << endl;
    
    // As KWord has only one depth of lists, we can process lists very simply.
    if ( layout.counter.numbering == CounterData::NUM_LIST )
    {
        // Are we still in a list of the right type?
        if (!m_inList || (layout.counter.style!=m_typeList))
        {
            // We are not yet part of a list
            m_inList=true;
            m_counterList=1; // Start numbering
            m_typeList=layout.counter.style;
        }

        switch (m_typeList)
        // TODO: when we would be able to save to UTF-8,
        //   use correct symbols
        {
        case CounterData::STYLE_CUSTOMBULLET: // We cannot keep the custom type/style
        default:
            {
                m_orderedList=false;
                *m_streamOut << "- ";
                break;
            }
        case CounterData::STYLE_NONE:
            {
                m_orderedList=false;
                break;
            }
        case CounterData::STYLE_CIRCLEBULLET:
            {
                m_orderedList=false;
                *m_streamOut << "o ";
                break;
            }
        case CounterData::STYLE_SQUAREBULLET:
            {
                m_orderedList=false;
                *m_streamOut << "~ "; // Not much a square
                break;
            }
        case CounterData::STYLE_DISCBULLET:
            {
                m_orderedList=false;
                *m_streamOut << "* "; // Not much a disc
                break;
            }
        case CounterData::STYLE_NUM:
        case CounterData::STYLE_CUSTOM:
            {
                m_orderedList=true;
                *m_streamOut << QString::number(m_counterList,10);
                break;
            }
        case CounterData::STYLE_ALPHAB_L:
            {
                m_orderedList=true;
                QString strTemp;
                for (int i=m_counterList;i>0;i/=26)
                     strTemp=QChar(0x40+i%26)+strTemp; // Lower alpha
                *m_streamOut << strTemp;
                break;
        }
        case CounterData::STYLE_ALPHAB_U:
            {
                m_orderedList=true;
                QString strTemp;
                for (int i=m_counterList;i>0;i/=26)
                     strTemp=QChar(0x40+i%26)+strTemp; // Lower alpha
                *m_streamOut << strTemp;
                break;
            }
        case CounterData::STYLE_ROM_NUM_L:
            {
                // For now, we do not support lower-case Roman numbering (TODO)
                m_orderedList=true;
                *m_streamOut << QString::number(m_counterList,10);
                break;
            }
        case CounterData::STYLE_ROM_NUM_U:
            {
                // For now, we do not support upper-case Roman numbering (TODO)
                m_orderedList=true;
                *m_streamOut << QString::number(m_counterList,10);
                break;
            }
        }
        m_counterList++; // Increment the list counter
    }
    else
    {
        m_inList=false; // Close an eventual list
        if ( layout.counter.numbering == CounterData::NUM_CHAPTER )
        {
            if (!layout.counter.depth)
            {   // HEAD 1
                *m_streamOut << "###################################" << m_eol;
                *m_streamOut << "# ";
                ProcessParagraphData ( paraText, paraFormatDataList);
                *m_streamOut << "###################################" << m_eol;
            }
            else if (layout.counter.depth==1)
            {   // HEAD 2
                *m_streamOut << "#### ";
                ProcessParagraphData ( paraText, paraFormatDataList);
            }
            else if (layout.counter.depth==2)
            {   // HEAD 3
                *m_streamOut << "## ";
                ProcessParagraphData ( paraText, paraFormatDataList);
            }
            else
            {
                ProcessParagraphData ( paraText, paraFormatDataList);
            }
        }
        else
        {
            ProcessParagraphData ( paraText, paraFormatDataList);
        }
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

    AsciiExportDialog* dialog = new AsciiExportDialog();

    if (!dialog)
    {
        kdError(30502) << "Dialog has not been created! Aborting!" << endl;
        return false;
    }

    if (!dialog->exec())
    {
        kdError(30502) << "Dialog was aborted! Aborting filter!" << endl;
        return false;
    }

    ASCIIWorker* worker=new ASCIIWorker();

    if (!worker)
    {
        kdError(30502) << "Cannot create Worker! Aborting!" << endl;
        delete dialog;
        return false;
    }

    worker->setUTF8(dialog->isEncodingUTF8());
    worker->setEndOfLine(dialog->getEndOfLine());

    delete dialog;
    
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
