#ifndef TKCOMMON_H
#define TKCOMMON_H

#include <qdom.h>

int XmlReadInt( const QDomElement &, const QString &, const int=0 );
void  XmlWriteInt( QDomElement &, const QString &, const int & );

uint XmlReadUInt( const QDomElement &, const QString &, const uint=0 );
void  XmlWriteUInt( QDomElement &, const QString &, const uint & );

double XmlReadDouble( const QDomElement &, const QString &, const double=0.0 );
void  XmlWriteDouble( QDomElement &, const QString &, const double & );

float XmlReadFloat( const QDomElement &, const QString &, const float=0.0 );
void  XmlWriteFloat( QDomElement &, const QString &, const float & );

QString XmlReadString( const QDomElement &, const QString &, const QString=QString::null );
void    XmlWriteString( QDomElement &,  const QString &, const QString & );

#endif

