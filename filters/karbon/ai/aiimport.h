/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __AIIMPORT_H__
#define __AIIMPORT_H__

#include <qobject.h>
#include <koFilter.h>

#include "karbonaiparserbase.h"

class QDomElement;
class QTextStream;

class AiImport : public KoFilter, KarbonAIParserBase
{
	Q_OBJECT

public:
	AiImport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~AiImport();

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );
private:
   QString m_result;

protected:
  void gotStartTag (const char *tagName, Parameters& params);
  void gotEndTag (const char *tagName);
  void gotSimpleTag (const char *tagName, Parameters& params);

  void parsingStarted();
  void parsingFinished();
  void parsingAborted();

};

#endif


