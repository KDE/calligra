/***************************************************************************
                          vdocument.h  -  description
                             -------------------
    begin                : Tue Apr 16 2002
    copyright            : (C) 2002 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VDOCUMENT_H
#define VDOCUMENT_H

#include <core/vlayer.h>

/**
  *@author 
  */

class VDocument {
public: 
	VDocument();
	~VDocument();

	const QString& mime() { return m_mime; }
	void setMime( const QString& mime ) { m_mime = mime; }

	const QString& version() { return m_version; }
	void setVersion( const QString& version ) { m_version = version; }

	const QString& editor() { return m_editor; }
	void setEditor( const QString& editor ) { m_editor = editor; }

	const QString& syntaxVersion() { return m_syntaxVersion; }
	void setSyntaxVersion( const QString& syntaxVersion ) { m_syntaxVersion = syntaxVersion; }

	void insertLayer( const VLayer* layer );

	const VLayerList& layers() const { return m_layers; }

	void save( QDomDocument &doc ) const;
//	void load( const QDomElement& element );

private:
	VLayerList m_layers;
	QString m_mime;
	QString m_version;
	QString m_editor;
	QString m_syntaxVersion;



};

#endif
