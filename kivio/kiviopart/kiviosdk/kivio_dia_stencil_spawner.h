/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2001 Nikolas Zimmermann <wildfox@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef KIVIO_DIA_STENCIL_SPAWNER_H
#define KIVIO_DIA_STENCIL_SPAWNER_H

#include <qdom.h>
#include <qvaluelist.h>
#include <qptrlist.h>
#include <QString>

#include "kivio_connector_target.h"
//#define protected public // ewww... what a gory hack
#include "kivio_stencil_spawner.h"
//#undef protected
#include "kivio_sml_stencil_spawner.h"

class KivioDiaStencilSpawner : public KivioStencilSpawner
{
    public:
	KivioDiaStencilSpawner(KivioStencilSpawnerSet *);
	virtual ~KivioDiaStencilSpawner();

	virtual bool load(const QString &file);
	virtual bool loadXML(const QString &file, QDomDocument &d);

	virtual QDomElement saveXML(QDomDocument &d);

	virtual QString &filename() { return m_filename; }

	virtual KivioStencil *newStencil();

	QPtrList<KivioConnectorTarget> *targets() { return m_smlStencilSpawner->targets(); }

    protected:
	void calculateDimensions(float x, float y);
	float diaPointToKivio(float point, bool xpoint);

	KivioSMLStencilSpawner *m_smlStencilSpawner;
	QString m_filename;

	QValueList<float> m_xlist;
	QValueList<float> m_ylist;
	float m_lowestx;
	float m_lowesty;
	float m_highestx;
	float m_highesty;
	float m_xscale;
	float m_yscale;
};

#endif


