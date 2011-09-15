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

#include "Collaborate.h"

Collaborate::~Collaborate() {
    foreach(connection* peer, peers) {
        delete peer;
    }
}

void Collaborate::readyRead(QDataStream& stream, uint flag, int source) {
    if (flag == 0)
        stream >> flag;

    uint start;
    uint end;

    stream >> start;
    stream >> end;

    switch(flag) {
    case CollabBackspace:

        emit receivedBackspace(start, end);
        sendBackspace(start, end, source);

        return;

    case CollabUserList:
        // TODO: do something here...
        return;

    case CollabFormat: case CollabString: case CollabFontSize:
    case CollabFontType: case CollabTextColor:
        break;

    default:
        qDebug() << "Collaborate: Unknown CollabFlag " << flag;
        return;
    }

    QByteArray data;
    uint param;
    QRgb color;

    switch(flag) {
    case CollabFormat:

        stream >> param;

        emit receivedFormat(start, end, (FormatFlag)param);
        sendFormat(start, end, (FormatFlag)param, source);

        return;

    case CollabString:

        stream >> data;

        emit receivedString(start, end, data);
        sendString(start, end, data, source);

        return;

    case CollabFontSize:

        stream >> param;

        emit receivedFontSize(start, end, param);
        sendFontSize(start, end, param, source);

        return;

    case CollabFontType:

        stream >> data;

        emit receivedFontType(start, end, data);
        sendFontType(start, end, data, source);

        return;

    case CollabTextColor:

        stream >> color;

        emit receivedTextColor(start, end, color);
        sendTextColor(start, end, color, source);

        return;

    case CollabTextBackgroundColor:

        stream >> color;

        emit receivedTextBackgroundColor(start, end, color);
        sendTextBackgroundColor(start, end, color, source);
    }
}

void Collaborate::sendString(uint start, uint end, const QByteArray &msg, int source) {
    for( int i = 0; i < peers.length(); ++i ) {
        if ( i != source ) {
            *(peers.at(i)->stream) << (uint)CollabString << start << end << msg;
        }
    }
}

void Collaborate::sendBackspace(uint start, uint end, int source) {
    for( int i = 0; i < peers.length(); ++i ) {
        if ( i != source ) {
            *(peers.at(i)->stream) << (uint)CollabBackspace << start << end;
        }
    }
}

void Collaborate::sendFormat(uint start, uint end, FormatFlag format, int source) {
    for( int i = 0; i < peers.length(); ++i ) {
        if ( i != source ) {
            *(peers.at(i)->stream) << (uint)CollabFormat << start << end << (uint)format;
        }
    }
}

void Collaborate::sendFontSize(uint start, uint end, uint size, int source) {
    for( int i = 0; i < peers.length(); ++i ) {
        if ( i != source ) {
            *(peers.at(i)->stream) << (uint)CollabFontSize << start << end << size;
        }
    }
}

void Collaborate::sendTextColor(uint start, uint end, QRgb color, int source) {
    for( int i = 0; i < peers.length(); ++i ) {
        if ( i != source ) {
            *(peers.at(i)->stream) << (uint)CollabTextColor << start << end << color;
        }
    }
}

void Collaborate::sendTextBackgroundColor(uint start, uint end, QRgb color, int source) {
    for( int i = 0; i < peers.length(); ++i ) {
        if ( i != source ) {
            *(peers.at(i)->stream) << (uint)CollabTextBackgroundColor << start << end << color;
        }
    }
}

void Collaborate::sendFontType(uint start, uint end, const QString &font, int source) {
    QByteArray baFont = font.toUtf8();
    for( int i = 0; i < peers.length(); ++i ) {
        if ( i != source ) {
            *(peers.at(i)->stream) << (uint)CollabFontType << start << end << baFont;
        }
    }
}
