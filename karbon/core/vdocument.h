/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef VDOCUMENT_H
#define VDOCUMENT_H

#include "vlayer.h"

class QDomDocument;
class QDomElement;

class VDocument
{
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
	bool load( const QDomElement& element );

private:
	VLayerList m_layers;
	QString m_mime;
	QString m_version;
	QString m_editor;
	QString m_syntaxVersion;
};

#endif
