/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Arjun Asthana <arjun@iiitd.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef COLLABORATE_H
#define COLLABORATE_H

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include <QTcpSocket>
#include <QColor>

// TODO:
// 1) ***IMPROVE ERROR HANDLING***
// 2) Add undo, redo, cut and paste
// 3) Use KO/QT enums instead of defining our own
// 4) Create proper instruction set/protocol definition
// 5) Access control lists

class Collaborate: public QObject
{
    Q_OBJECT

public:
    // TODO: Notify other side about errors and gracefully disconnect
    enum CollabFlag { CollabError = 0, CollabInit, CollabString, CollabBackspace,
                      CollabFormat, CollabFontSize, CollabFontType, CollabTextColor,
                      CollabTextBackgroundColor, CollabUserList, CollabDisconnect };

    enum FormatFlag { INVALID = 0, FormatBold, FormatItalic, FormatUnderline,
                      FormatSubScript, FormatSuperScript, FormatAlignLeft,
                      FormatAlignRight, FormatAlignCenter, FormatAlignJustify,
                      FormatListBullet, FormatListNumber };

    // TODO: create new error flag definition
    enum CollabError { Disconnect = 0, ShortRead, FileError, SocketError };

    Collaborate(const QString &nick, QObject* parent = 0) :
            QObject(parent), nick(nick)
    {
    }

    ~Collaborate();

    // TODO: call from required functions in MainWindow
    // When called from MainWindow, make sure the source is -1
    virtual void sendString(uint start, uint end, const QByteArray &msg, int source = -1);
    virtual void sendBackspace(uint start, uint end, int source = -1);
    virtual void sendFormat(uint start, uint end, FormatFlag format, int source = -1);
    virtual void sendFontSize(uint start, uint end, uint size, int source = -1);
    virtual void sendTextColor(uint start, uint end, QRgb color, int source = -1);
    virtual void sendTextBackgroundColor(uint start, uint end, QRgb color, int source = -1);
    virtual void sendFontType(uint start, uint end, const QString &font, int source = -1);

protected:

    virtual void readyRead(QDataStream& stream, uint flag = 0, int source = -1);

    QString nick;
    QString filename;

    struct connection {
        connection(QTcpSocket* socket) :
                nick(0), socket(socket), stream(new QDataStream(socket))
        {
        }

        ~connection() {
            delete nick;
            delete socket;
            delete stream;
        }

        QString* nick;
        QTcpSocket* socket;
        QDataStream* stream;
    };

    QList<connection*> peers;

protected slots:
    virtual void readyRead(int source = -1) = 0;
    virtual void disconnected(int source = -1) = 0;
    // TODO: add default implementation of socketError
    //       and connect it to the correct signals

signals:
    void receivedString(uint start, uint end, QByteArray msg);
    void receivedBackspace(uint start, uint end); // TODO: add delete direction
    void receivedFormat(uint start, uint end, Collaborate::FormatFlag format);
    void receivedFontSize(uint start, uint end, uint size);
    void receivedTextColor(uint start, uint end, QRgb color);
    void receivedTextBackgroundColor(uint start, uint end, QRgb color);
    void receivedFontType(uint start, uint end, const QString &font);
    void error(quint16 err); // TODO: improve error handling
};

#endif // COLLABORATE_H
