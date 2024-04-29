// SPDX-FileCopyrightText: 2008, 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_H
#define RTFREADER_H

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QStack>
#include <QTextDocument>
#include <QTextFormat>

#include "Destination.h"
#include "RtfGroupState.h"
#include "Tokenizer.h"
#include "rtfreader_export.h"
/**
   Namespace for the public interface for the RtfReader classes
*/
namespace RtfReader
{
class AbstractRtfOutput;
class RtfProperty;
class Reader;

/**
   Reader for RTF formatted documents

   This class provides a simple API for reading documents
   encoded in the Microsoft Rich Text Format (RTF).

   TODO: add spec reference
*/
class RTFREADER_EXPORT Reader : public QObject
{
    Q_OBJECT
public:
    /**
       Normal constructor
    */
    explicit Reader(QObject *parent = nullptr);

    /**
       Open a document

       \param filename the name of the document to open

       \return true if the document was successfully opened, otherwise false
    */
    bool open(const QString &filename);

    /**
       Close the open document

       This has no effect if the document is not open.
    */
    void close();

    /**
       The name of the file that is open.

       Will return an empty string if no file is open.
    */
    QString fileName() const;

    /**
       Parse the document

       \param output pointer to an output implementation
    */
    bool parseTo(AbstractRtfOutput *output);

    ~Reader() override;

    /////////////////////////////////////////////////
    //
    // Callback functions
    //
    ////////////////////////////////////////////////
    void startInfo();

    void todo(RtfReader::RtfProperty *property);
    void todoDest(RtfReader::RtfProperty *property);

private:
    /////////////////////////////////////////////////
    //
    // Implementation details
    //
    ////////////////////////////////////////////////

    // parse the RTF file, inserting elements into the document
    // being generated
    void parseFile();

    // parse the file header section, including sanity checks
    bool parseFileHeader();

    // check the file header for format / version compatibility
    bool headerFormatIsKnown(const QString &tokenName, int tokenValue);

    // parse the body of the document
    void parseDocument();

    // Change the destination
    void changeDestination(const QString &destinationName);

    // Destination factory
    Destination *makeDestination(const QString &destinationName);

    /////////////////////////////////////////////////
    //
    // Member variables below
    //
    ////////////////////////////////////////////////

    // The name of the file that is open (if any)
    QFile *m_inputDevice;

    // The tokenizer on the RTF document being parsed
    Tokenizer *m_tokenizer;

    // The output strategy
    AbstractRtfOutput *m_output;

    // The destination stack
    QStack<Destination *> m_destinationStack;

    QStack<RtfGroupState> m_stateStack;

    // debug things
    QString m_debugIndent;
};
}

#endif
