/* This file is part of the KDE project
   Copyright (C) 2001 Ian Reinhart Geiser <geiseri@yahoo.com>
   This is based off of the KOffice Example found in the KOffice
   CVS.  Torben Weis <weis@kde.org> is the original author.

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
#include <SOAP.h>
#include <SOAPDebugger.h>
#include <SOAPSocket.h>

#include <qstring.h>
#include <qlist.h>
#include <kurl.h>

#ifndef SOAP_SESSION
#define SOAP_SESSION
class _parameter
{
public:
   _parameter(QString key = "", QString type = "", QString arg = "")
   {
      _key = key;
      _type = type;
      _arg = arg;
   }
   _parameter(_parameter& newOne)
   {
      _key = newOne.getKey();
      _type = newOne.getType();
      _arg = newOne.getType();
   }
   ~_parameter(){}
   QString getKey() {return _key;}
   QString getType() {return _type;}
   QString getArg() {return _arg;}
   void setKey(QString key) {_key = key;}
   void setType(QString type) {_type = type;}
   void setArg(QString arg) {_arg = arg;}
private:
   QString _key;
   QString _type;
   QString _arg;
};

class soapsession
{
public:
   soapsession();
   ~soapsession();

   void setServerAddress( QString serverAddress = "localhost");
   void setServerPort( int serverPort = 80);
   void setServerType( QString serverType = "http");
   void setSOAPAction(QString theAction);
   void setSOAPNameSpace( QString theNameSpace);
   void setSOAPMethod( QString theMethod);

   bool sendRequest();
   void setRequestList( QList<_parameter> theRequestList);
   QList<_parameter> getResponseList();

private:
   void buildEnvelope();
   void openEnvelope();

   QList<_parameter> requestList;
   QList<_parameter> responseList;
   KURL theURL;
   SOAPProxy *proxy;
   SOAPMethod *method;

};

#endif
