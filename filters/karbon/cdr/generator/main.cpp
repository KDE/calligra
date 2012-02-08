/* This file is part of the Calligra project, made within the KDE community.

   Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

// plugin
#include "cheadergenerator.h"
#include "formatparser.h"
#include "formatdocument.h"
// Qt
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>


enum GeneratorErrorCode
{
    NoError = 0,
    InputFileOpenError = 1,
    OutputFileOpenError = 2,
    ParsingError = 3,
    GeneratingError = 4,
    ArgumentsError = 5
};


static GeneratorErrorCode
parseArguments( QString& outputFilePath, QString& inputFilePath,
                const QStringList& arguments )
{
    if( arguments.count() != 3 )
        return ArgumentsError;

    outputFilePath = arguments.at(1);
    inputFilePath = arguments.at(2);

    return NoError;
}

int
main( int argc, char** argv )
{
    QCoreApplication app( argc, argv );

    QString inputFilePath;
    QString outputFilePath;

    // get arguments
    const GeneratorErrorCode parseArgumentsResult = parseArguments( outputFilePath, inputFilePath, app.arguments() );
    if( parseArgumentsResult != NoError )
        return parseArgumentsResult;

    // prepare input
    QFile inputFile( inputFilePath );
    if( ! inputFile.open(QIODevice::ReadOnly) )
    {
        return InputFileOpenError;
    }

    // prepare output
    QFile outputFile( outputFilePath );
    if( ! outputFile.open(QIODevice::WriteOnly) )
    {
        return OutputFileOpenError;
    }

    // translate!
    FormatDocument* document = FormatParser::parse( &inputFile );
    if( ! document ) {
        return ParsingError;
    }

    if (! CHeaderGenerator::write(document, &outputFile) ) {
        return GeneratingError;
    }

    return NoError;
}