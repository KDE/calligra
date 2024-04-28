/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>
  SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef EMFPARSER_H
#define EMFPARSER_H

#include "kovectorimage_export.h"

#include "EmfOutput.h"

#include <QRect> // also provides QSize
#include <QString>

/**
   \file

   Primary definitions for EMF parser
*/

/**
   Namespace for Enhanced Metafile (EMF) classes
*/
namespace Libemf
{

/**
    %Parser for an EMF format file
 */
class KOVECTORIMAGE_EXPORT Parser
{
public:
    Parser();
    ~Parser();

    /**
     * Load an EMF file
     *
     * \param fileName the name of the file to load
     *
     * \return true on successful load, or false on failure
     */
    bool load(const QString &fileName);
    /**
     * Load an EMF file
     *
     * \param contents a QByteArray containing the contents of the EMF.
     *
     * \return true on successful load, or false on failure
     */
    bool load(const QByteArray &contents);

    /**
     * Load an EMF file from a stream
     *
     * \param stream the stream to read from
     *
     * \return true on successful load, or false on failure
     */
    bool loadFromStream(QDataStream &stream);

    /**
       Set the output strategy for the parserr

       \param output pointer to a strategy implementation
    */
    void setOutput(AbstractOutput *output);

private:
    // read a single EMF record
    bool readRecord(QDataStream &stream);

    // temporary function to soak up unneeded bytes
    void soakBytes(QDataStream &stream, int numBytes);

    // temporary function to dump output bytes
    void outputBytes(QDataStream &stream, int numBytes);

    // Pointer to the output strategy
    AbstractOutput *mOutput;
};

}

#endif
