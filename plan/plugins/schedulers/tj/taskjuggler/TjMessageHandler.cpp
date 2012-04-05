/*
 * TjMessageHandler.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007
 *               by Chris Schlaeger <cs@kde.org>
 * Copyright (c) 2011 by Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#include "TjMessageHandler.h"
#include "taskjuggler.h"
#include "Utility.h"

#include <QDebug>

namespace TJ
{

TjMessageHandler TJMH(false);

void
TjMessageHandler::warningMessage(const QString& msg, const CoreAttributes *object )
{
    warningMessage( msg, QString() );
    emit message( (int)TJ::WarningMsg, msg, const_cast<CoreAttributes*>(object));
}
void
TjMessageHandler::warningMessage(const QString& msg, const QString& file, int
                                 line)
{
    warnings++;
    warningPositions << messages.count();
    messages << msg;

    if (consoleMode)
    {
        if (file.isEmpty())
            qWarning()<<msg;
        else
            qWarning()<<file<<":"<<line<<":"<<msg;
    }
    else
        printWarning(msg, file, line);
}

void
TjMessageHandler::errorMessage( const QString& msg, const CoreAttributes *object )
{
    errorMessage( msg, QString() );
    emit message( (int)TJ::ErrorMsg, msg, const_cast<CoreAttributes*>(object) );
}

void
TjMessageHandler::errorMessage(const QString& msg, const QString& file, int
                               line)
{
    errors++;
    errorPositions << messages.count();
    messages << msg;

    if (consoleMode)
    {
        if (file.isEmpty())
            qWarning()<<msg;
        else
            qWarning()<<file<<":"<<line<<":"<<msg;
    }
    else
        printError(msg, file, line);
}

void
TjMessageHandler::fatalMessage(const QString& msg, const QString& file, int
                               line)
{
    if (consoleMode)
    {
        if (file.isEmpty())
            qWarning()<<msg;
        else
            qWarning()<<file<<":"<<line<<":"<<msg;
    }
    else
        printFatal(msg, file, line);
}

void
TjMessageHandler::infoMessage( const QString &msg, const CoreAttributes *object )
{
    ++infos;
    infoPositions << messages.count();
    messages << msg;
    emit message( (int)TJ::InfoMsg, msg, const_cast<CoreAttributes*>(object) );
}
void
TjMessageHandler::debugMessage( const QString &msg, const CoreAttributes *object )
{
    ++debugs;
    debugPositions << messages.count();
    messages << msg;
    emit message( (int)TJ::DebugMsg, msg, const_cast<CoreAttributes*>(object) );
}

} // namespace TJ

#include "TjMessageHandler.moc"
