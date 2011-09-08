/*
 * TjMessageHandler.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007
 *               by Chris Schlaeger <cs@kde.org>
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

namespace TJ
{

TjMessageHandler TJMH(false);

void
TjMessageHandler::warningMessage(const QString& msg, CoreAttributes *object )
{
    warningMessage( msg, QString() );
    emit message( (int)TJ::WarningMsg, msg, object);
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
            qWarning("%s", msg.latin1());
        else
            qWarning("%s:%d: %s", file.latin1(), line, msg.latin1());
    }
    else
        printWarning(msg, file, line);
}

void
TjMessageHandler::errorMessage( const QString& msg, CoreAttributes *object )
{
    errorMessage( msg, QString() );
    emit message( (int)TJ::ErrorMsg, msg, object );
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
            qWarning("%s", msg.latin1());
        else
            qWarning("%s:%d: %s", file.latin1(), line, msg.latin1());
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
            qFatal("%s", msg.latin1());
        else
            qFatal("%s:%d: %s", file.latin1(), line, msg.latin1());
    }
    else
        printFatal(msg, file, line);
}

void
TjMessageHandler::infoMessage( const QString &msg, CoreAttributes *object )
{
    ++infos;
    infoPositions << messages.count();
    messages << msg;
    emit message( (int)TJ::InfoMsg, msg, object );
}
void
TjMessageHandler::debugMessage( const QString &msg, CoreAttributes *object )
{
    ++debugs;
    debugPositions << messages.count();
    messages << msg;
    emit message( (int)TJ::DebugMsg, msg, object );
}

} // namespace TJ

#include "TjMessageHandler.moc"
