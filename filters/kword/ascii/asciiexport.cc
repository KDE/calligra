/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (C) 2003 Clarence Dang <dang@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include <limits.h>

#include <q3cstring.h>
#include <QFile>
#include <QIODevice>
#include <QString>
#include <QTextCodec>
#include <QTextStream>
//Added by qt3to4:
#include <Q3ValueList>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoStore.h>

#include <KWEFStructures.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include <ExportDialog.h>
#include <asciiexport.h>


class ASCIIExportFactory : KGenericFactory<ASCIIExport>
{
public:
    ASCIIExportFactory() : KGenericFactory<ASCIIExport>("kwordasciiexport")
    {
    }

protected:
    virtual void setupTranslations(void)
    {
        KGlobal::locale()->insertCatalog("kofficefilters");
    }
};

K_EXPORT_COMPONENT_FACTORY(libasciiexport, ASCIIExportFactory())


class ASCIIWorker : public KWEFBaseWorker
{
public:
    ASCIIWorker() : m_ioDevice(NULL), m_streamOut(NULL), m_eol("\n")/*,
        m_inList(false)*/
    {
    }

    virtual ~ASCIIWorker()
    {
        delete m_streamOut; delete m_ioDevice;
    }

public:
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void); // Close file in normal conditions

    virtual bool doOpenDocument(void);
    virtual bool doCloseDocument(void);

    virtual bool doFullParagraphList(const Q3ValueList<ParaData>& paraList);
    virtual bool doFullParagraph(const ParaData& para);
    virtual bool doFullParagraph(const QString& paraText,
        const LayoutData& layout,
        const ValueListFormatData& paraFormatDataList);

public:
    QString getEndOfLine(void) const { return m_eol; }
    void setEndOfLine(const QString& str) { m_eol = str; }

    QTextCodec* getCodec(void) const { return m_codec; }
    void setCodec(QTextCodec* codec) { m_codec = codec; }

private:
    virtual bool ProcessTable(const Table& table);
    virtual bool ProcessParagraphData (const QString& paraText,
        const ValueListFormatData& paraFormatDataList);

private:
    QIODevice* m_ioDevice;
    QTextStream* m_streamOut;

    QTextCodec* m_codec; // QTextCodec in which the file will be written
    QString m_eol; // End of line character(s)
    QStringList m_automaticNotes; // Automatic foot-/endnotes
    QString m_manualNotes; // Manual foot-/endnotes

#if 0
    CounterData::Style m_typeList; // What is the style of the current list (undefined, if we are not in a list)
    bool m_inList; // Are we currently in a list?
    bool m_orderedList; // Is the current list ordered or not (undefined, if we are not in a list)
    int  m_counterList; // Counter for te lists
#endif
};

bool ASCIIWorker::doOpenFile(const QString& filenameOut, const QString& /*to*/)
{
    m_ioDevice = new QFile(filenameOut);

    if (!m_ioDevice)
    {
        kError(30502) << "No output file! Aborting!" << endl;
        return false;
    }

    if (!m_ioDevice->open(QIODevice::WriteOnly))
    {
        kError(30502) << "Unable to open output file!" << endl;
        return false;
    }

    m_streamOut = new QTextStream(m_ioDevice);
    if (!m_streamOut)
    {
        kError(30502) << "Could not create output stream! Aborting!" << endl;
        m_ioDevice->close();
        return false;
    }

    kDebug(30502) << "Charset used: " << getCodec()->name() << endl;

    if (!getCodec())
    {
        kError(30502) << "Could not create QTextCodec! Aborting" << endl;
        return false;
    }

    m_streamOut->setCodec(getCodec());

    return true;
}

bool ASCIIWorker::doCloseFile(void)
{
    delete m_streamOut;
    m_streamOut=NULL;
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
    // Add foot-/endnotes
    if (!m_automaticNotes.empty())
    {
        *m_streamOut << m_eol;
        int noteNumber = 1;
        for (QStringList::Iterator it = m_automaticNotes.begin(); it != m_automaticNotes.end(); ++it)
        {
            *m_streamOut << "[" << noteNumber << "] " << *it;
            noteNumber++;
        }
    }

    if (!m_manualNotes.isEmpty())
        *m_streamOut << m_eol << m_manualNotes;

    return true;
}

bool ASCIIWorker::doFullParagraphList(const Q3ValueList<ParaData>& paraList)
{
    for (Q3ValueList<ParaData>::ConstIterator it = paraList.begin();
         it != paraList.end();
         it++)
    {
        if (!doFullParagraph(*it)) return false;
    }

    return true;
}

bool ASCIIWorker::doFullParagraph(const ParaData& para)
{
    return doFullParagraph(para.text, para.layout, para.formattingList);
}

bool ASCIIWorker::doFullParagraph(const QString& paraText, const LayoutData& layout,
    const ValueListFormatData& paraFormatDataList)
{
    kDebug(30502) << "Entering ASCIIWorker::doFullParagraph" << endl;

#if 0
    // As KWord has only one depth of lists, we can process lists very simply.
    // --
    // Not anymore - Clarence
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
        ProcessParagraphData ( paraText, paraFormatDataList);
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
            else if (layout.counter.depth==3)
            {   // HEAD 4
                *m_streamOut << "# ";
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
#else
    if (!layout.counter.text.isEmpty())
        *m_streamOut << layout.counter.text << " ";

    if (!ProcessParagraphData(paraText, paraFormatDataList)) return false;
#endif

    kDebug(30502) << "Exiting ASCIIWorker::doFullParagraph" << endl;
    return true;
}


bool ASCIIWorker::ProcessTable(const Table& table)
{
    kDebug(30502) << "processTable CALLED!" << endl;

    // just dump the table out (no layout for now)
    for (Q3ValueList<TableCell>::ConstIterator it = table.cellList.begin();
         it != table.cellList.end();
         it++)
    {
        if (!doFullParagraphList(*(*it).paraList)) return false;
    }

    return true;
}

// ProcessParagraphData () mangles the pure text through the
// formatting information stored in the FormatData list and prints it
// out to the export file.
bool ASCIIWorker::ProcessParagraphData(const QString& paraText,
    const ValueListFormatData& paraFormatDataList)
{
    bool lastSegmentWasText = true;

    if (!paraText.isEmpty())
    {
        ValueListFormatData::ConstIterator  paraFormatDataIt;

        for (paraFormatDataIt = paraFormatDataList.begin ();
             paraFormatDataIt != paraFormatDataList.end ();
             paraFormatDataIt++)
        {
            lastSegmentWasText = true;

            switch ((*paraFormatDataIt).id)
            {
                case 1: // Normal text
                {
                    QString strText(paraText.mid((*paraFormatDataIt).pos,(*paraFormatDataIt).len));
                    strText = strText.replace(QChar(10), m_eol, true);
                    *m_streamOut << strText;
                    break;
                }
                case 4: // Variable
                {
                    if (11==(*paraFormatDataIt).variable.m_type)
                    {
                        // Footnote
                        QString value = (*paraFormatDataIt).variable.getFootnoteValue();
                        bool automatic = (*paraFormatDataIt).variable.getFootnoteAuto();
                        Q3ValueList<ParaData> *paraList = (*paraFormatDataIt).variable.getFootnotePara();
                        if (paraList)
                        {
                            QString notestr;
                            Q3ValueList<ParaData>::ConstIterator it;
                            Q3ValueList<ParaData>::ConstIterator end(paraList->end());
                            for (it=paraList->begin();it!=end;++it)
                                notestr += (*it).text.stripWhiteSpace().replace(QChar(10), m_eol, true) + m_eol;

                            *m_streamOut << "[";
                            if (automatic) {
                                // Automatic footnote
                                *m_streamOut << m_automaticNotes.count() + 1;
                                m_automaticNotes.append(notestr);
                            }
                            else
                            {
                                // Manual footnote
                                *m_streamOut << value;
                                m_manualNotes += "[" + value + "] " + notestr;
                            }
                            *m_streamOut << "]";
                        }
                    }
                    else
                    {
                        // Generic variable
                        *m_streamOut << (*paraFormatDataIt).variable.m_text;
                    }
                    break;
                }
                case 6: // Frame Anchor
                {
                    if ((*paraFormatDataIt).frameAnchor.type == 6) // Table
                    {
                        if ((*paraFormatDataIt).pos)
                            *m_streamOut << m_eol;

                        if (!ProcessTable((*paraFormatDataIt).frameAnchor.table))
                            return false;
                    }
                    else
                    {
                        kWarning(30502) << "Unsupported frame anchor type: "
                            << (*paraFormatDataIt).frameAnchor.type << endl;
                    }

                    lastSegmentWasText = false;
                    break;
                }
                default:
                {
                    kWarning(30502) << "Not supported paragraph type: "
                        << (*paraFormatDataIt).id << endl;
                    break;
                }
            }
        }
    }

    if (lastSegmentWasText)
        *m_streamOut << m_eol; // Write end of line

    return true;
}


ASCIIExport::ASCIIExport(QObject* parent, const QStringList&)
           : KoFilter(parent)
{
}

KoFilter::ConversionStatus ASCIIExport::convert(const QByteArray& from, const QByteArray& to)
{
    if (to != "text/plain" || from != "application/x-kword")
    {
        return KoFilter::NotImplemented;
    }
    AsciiExportDialog* dialog = 0;
    if (!m_chain->manager()->getBatchMode())
    {
      dialog = new AsciiExportDialog();
      if (!dialog)
      {
	kError(30502) << "Dialog has not been created! Aborting!" << endl;
	return KoFilter::StupidError;
      }

      if (!dialog->exec())
      {
	kError(30502) << "Dialog was aborted! Aborting filter!" << endl;
	return KoFilter::UserCancelled;
      }
    }
    ASCIIWorker* worker = new ASCIIWorker();

    if (!worker)
    {
        kError(30502) << "Cannot create Worker! Aborting!" << endl;
        delete dialog;
        return KoFilter::StupidError;
    }
    QTextCodec* codec;
    if (dialog)
    	codec = dialog->getCodec();
    else
        codec = QTextCodec::codecForName("UTF-8");

    if ( !codec )
    {
        kError(30502) << "No codec!" << endl;
        delete dialog;
        return KoFilter::StupidError;
    }

    worker->setCodec( codec );
    if (dialog)
    	worker->setEndOfLine(dialog->getEndOfLine());
    else
        worker->setEndOfLine("\n");

    delete dialog;

    KWEFKWordLeader* leader = new KWEFKWordLeader(worker);

    if (!leader)
    {
        kError(30502) << "Cannot create Worker! Aborting!" << endl;
        delete worker;
        return KoFilter::StupidError;
    }

    KoFilter::ConversionStatus result = leader->convert(m_chain,from,to);

    delete leader;
    delete worker;

    return result;
}

#include <asciiexport.moc>
