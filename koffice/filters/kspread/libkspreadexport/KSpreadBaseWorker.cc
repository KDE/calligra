/*
This file is part of the KDE project
Copyright (C) 2002 Fred Malabre <fmalabre@yahoo.com>

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
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KSpreadBaseWorker.h"

#include <kdebug.h>


KSpreadBaseWorker::KSpreadBaseWorker() {
}


KSpreadBaseWorker::~KSpreadBaseWorker() {
}


KoFilter::ConversionStatus KSpreadBaseWorker::startDocument(KSpreadFilterProperty property) {
	KSpreadFilterProperty::Iterator it;
	for (it = property.begin(); it != property.end(); ++it) {
		kdDebug(30508) << "startDocument: " << it.key() << "->" << it.data() << endl;
	}
	return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startInfoLog(KSpreadFilterProperty property) {
	KSpreadFilterProperty::Iterator it;
	for (it = property.begin(); it != property.end(); ++it) {
		kdDebug(30508) << "startInfoLog: " << it.key() << "->" << it.data() << endl;
	}
	return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startInfoAuthor(KSpreadFilterProperty property) {
	KSpreadFilterProperty::Iterator it;
	for (it = property.begin(); it != property.end(); ++it) {
		kdDebug(30508) << "startInfoAuthor: " << it.key() << "->" << it.data() << endl;
	}
	return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startInfoAbout(KSpreadFilterProperty property) {
	KSpreadFilterProperty::Iterator it;
	for (it = property.begin(); it != property.end(); ++it) {
		kdDebug(30508) << "startInfoAbout: " << it.key() << "->" << it.data() << endl;
	}
	return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startSpreadBook(KSpreadFilterProperty property) {
	KSpreadFilterProperty::Iterator it;
	for (it = property.begin(); it != property.end(); ++it) {
		kdDebug(30508) << "startSpreadBook: " << it.key() << "->" << it.data() << endl;
	}
	return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startSpreadSheet(KSpreadFilterProperty property) {
	KSpreadFilterProperty::Iterator it;
	for (it = property.begin(); it != property.end(); ++it) {
		kdDebug(30508) << "startSpreadSheet: " << it.key() << "->" << it.data() << endl;
	}
	return KoFilter::OK;
}


KoFilter::ConversionStatus KSpreadBaseWorker::startSpreadCell(KSpreadFilterProperty property) {
	KSpreadFilterProperty::Iterator it;
	for (it = property.begin(); it != property.end(); ++it) {
		kdDebug(30508) << "startSpreadCell: " << it.key() << "->" << it.data() << endl;
	}
	return KoFilter::OK;
}
