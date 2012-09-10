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
#ifndef _TjMessageHandler_h_
#define _TjMessageHandler_h_

#include <QObject>
#include <QString>

namespace TJ
{

/**
 * This class handles all error or warning messages that the library functions
 * can send out. Depending on the mode it either send the messages to STDERR
 * or raises a Qt signal.
 *
 * @short Handles all error or warning messages.
 * @author Chris Schlaeger <cs@kde.org>
 */

class CoreAttributes;

class TjMessageHandler : public QObject
{
    Q_OBJECT
public:
    TjMessageHandler(bool cm = true) :
        QObject(),
        consoleMode(cm),
        warnings(0),
        errors(0)
    { }
    virtual ~TjMessageHandler() { }

    void warningMessage( const QString& msg, const CoreAttributes *object = 0 );
    void warningMessage(const QString& msg, const QString& file,
                        int line = -1);

    void errorMessage(const QString& msg, const CoreAttributes *object = 0 );
    void errorMessage(const QString& msg, const QString& file,
                      int line = -1);

    void fatalMessage(const QString& msg, const QString& file = QString(),
                      int line = -1);

    void setConsoleMode(bool cm) { consoleMode = cm; }

    void resetCounters() { warnings = errors = infos = debugs = 0; }
    void reset() { resetCounters(); messages.clear(); }

    bool isWarning( int messagePos ) const { return warningPositions.contains( messagePos ); }
    int getWarnings() const { return warnings; }
    QString getWarningMessage( int pos ) const { return getMessage( warningPositions.value( pos ) ); }

    bool isError( int messagePos ) const { return errorPositions.contains( messagePos ); }
    int getErrors() const { return errors; }
    QString getErrorMessage( int pos ) const { return getMessage( errorPositions.value( pos ) ); }

    bool isInfo( int messagePos ) const { return infoPositions.contains( messagePos ); }
    void infoMessage( const QString &msg, const CoreAttributes *object = 0 );
    int getInfos() const { return infos; }
    QString getInfoMessage( int pos ) const { return getMessage( infoPositions.value( pos ) ); }

    bool isDebug( int messagePos ) const { return debugPositions.contains( messagePos ); }
    void debugMessage( const QString &msg, const CoreAttributes *object = 0 );
    int getDebugs() const { return debugs; }
    QString getDebugMessage( int pos ) const { return getMessage( debugPositions.value( pos ) ); }

    int getMessageCount() const { return messages.count(); }
    QString getMessage( int pos ) const { return messages.value( pos ); }

signals:
    void printWarning(const QString& msg, const QString& file, int line);
    void printError(const QString& msg, const QString& file, int line);
    void printFatal(const QString& msg, const QString& file, int line);

    void message( int type, const QString &msg, TJ::CoreAttributes *object );

private:
    bool consoleMode;
    QList<QString> messages;
    int warnings;
    QList<int> warningPositions;
    int errors;
    QList<int> errorPositions;
    int infos;
    QList<int> infoPositions;
    int debugs;
    QList<int> debugPositions;
};

extern TjMessageHandler TJMH;

} // namespace TJ

#endif
