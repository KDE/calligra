/*
 *  Copyright (c) 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "psd_layer_section.h"

#include <QIODevice>

#include <kis_debug.h>

#include "psd_header.h"
#include "psd_utils.h"

PSDLayerSection::PSDLayerSection(const PSDHeader& header)
    : error(QString::null)
    , m_header(header)
{
    hasTransparency = false;
    layerInfoSize = 0;
    nLayers = 0;
}

PSDLayerSection::~PSDLayerSection()
{
    qDeleteAll(layers);
}

bool PSDLayerSection::read(QIODevice* io)
{
    dbgFile << "reading layer section. Pos:" << io->pos() <<  "bytes left:" << io->bytesAvailable();
    
    layerSectionSize = 0;
    if (m_header.m_version == 1) {
        quint32 _layerSectionSize = 0;
        if (!psdread(io, &_layerSectionSize) || _layerSectionSize > (quint64)io->bytesAvailable()) {
            error = QString("Could not read layer section size. Got %1. Bytes left %2")
                    .arg(_layerSectionSize).arg(io->bytesAvailable());
            return false;
        }
        layerSectionSize = _layerSectionSize;
    }
    else if (m_header.m_version == 2) {
        if (!psdread(io, &layerSectionSize) || layerSectionSize > (quint64)io->bytesAvailable()) {
            error = QString("Could not read layer section size. Got %1. Bytes left %2")
                    .arg(layerSectionSize).arg(io->bytesAvailable());
            return false;
        }
    }
    
    dbgFile << "layer section size" << layerSectionSize;
    
    dbgFile << "reading layer info block. Bytes left" << io->bytesAvailable() << "position" << io->pos();
    
    layerInfoSize = 0;
    nLayers = 0;
    if (m_header.m_version == 1) {
        quint32 layerInfoSize;
        if (!psdread(io, &layerInfoSize) || layerInfoSize > (quint64)io->bytesAvailable()) {
            error = "Could not read layer section size";
            return false;
        }
        layerInfoSize = layerInfoSize;
    }
    
    else if (m_header.m_version == 2) {
        if (!psdread(io, &layerInfoSize) || layerInfoSize > (quint64)io->bytesAvailable()) {
            error = "Could not read layer section size";
            return false;
        }
    }
    
    dbgFile << "Layer info block size" << layerInfoSize;
    
    if (layerInfoSize > 0 ) {
        
        // rounded to a multiple of 2
        if ((layerInfoSize & 0x01) != 0) {
            layerInfoSize++;
        }
        
        dbgFile << "Layer info block size after rounding" << layerInfoSize;
        
        if (!psdread(io, &nLayers) || nLayers == 0) {
            error = QString("Could not read read number of layers or no layers in image. %1").arg(nLayers);
            return false;
        }
        
        if (nLayers < 0) {
            hasTransparency = true; // first alpha channel is the alpha channel of the projection.
            nLayers = -nLayers;
        }
        else {
            hasTransparency = false;
        }

        dbgFile << "transparency" << hasTransparency;

        dbgFile << "Number of layers" << nLayers << "transparency" << hasTransparency;

        for (int i = 0; i < nLayers; ++i) {

            dbgFile << "Going to read layer " << i << "pos" << io->pos();
            PSDLayerRecord *layerRecord = new PSDLayerRecord(m_header, hasTransparency);
            if (!layerRecord->read(io)) {
                error = QString("Could not load layer %1: %2").arg(i).arg(layerRecord->error);
                return false;
            }
            dbgFile << "Read layer" << i << layerRecord->layerName << "blending mode"
                    << layerRecord->blendModeKey << io->pos();
            layers << layerRecord;
        }
    }

    // get the positions for the channels belonging to each layer
    for (int i = 0; i < nLayers; ++i) {
        
        dbgFile << "Going to seek channel positions for layer" << i;
        Q_ASSERT(i < layers.size());
        if (i > layers.size()) {
            error = QString("Expected layer %1, but only have %2 layers").arg(i).arg(layers.size());
            return false;
        }
        
        PSDLayerRecord *layerRecord = layers.at(i);
        
        for (int j = 0; j < layerRecord->nChannels; ++j) {
            

            Q_ASSERT(j < layerRecord->channelInfoRecords.size());
            if (j > layerRecord->channelInfoRecords.size()) {
                error = QString("Expected channel %1, but only have %2 channels for layer %3")
                        .arg(j)
                        .arg(layerRecord->channelInfoRecords.size())
                        .arg(i);
                return false;
            }

            PSDLayerRecord::ChannelInfo channelInfo = layerRecord->channelInfoRecords.at(j);
            dbgFile << "channel record" << j << "for layer" << i << "with length" << channelInfo.channelDataLength << "is at" << io->pos();
            channelInfo.channelDataStart = io->pos();
            io->seek(io->pos() + channelInfo.channelDataLength);
        }
    }

    quint32 globalMaskBlockLength;
    if (!psdread(io, &globalMaskBlockLength) || globalMaskBlockLength > (quint64)io->bytesAvailable()) {
        error = "Could not read global mask info block";
        return false;
    }

    if (globalMaskBlockLength > 0) {

        if (!psdread(io, &overlayColorSpace)) {
            error = "Could not read global mask info overlay colorspace";
            return false;
        }

        for (int i = 0; i < 4; ++i) {
            if (!psdread(io, &colorComponents[i])) {
                error = QString("Could not read mask info visualizaion color component %1").arg(i);
                return false;
            }
        }

        if (!psdread(io, &opacity)) {
            error = "Could not read global mask info visualisation opacity";
            return false;
        }

        if (!psdread(io, &kind)) {
            error = "Could not read global mask info visualisation type";
            return false;
        }
    }

    return valid();
}

bool PSDLayerSection::write(QIODevice* io)
{
    Q_UNUSED(io);
    Q_ASSERT(valid());
    if (!valid()) {
        error = "Cannot write an invalid Layer Section object";
        return false;
    }
    qFatal("TODO: implement writing the layer section");
    return false;
}

bool PSDLayerSection::valid()
{
    if (layerInfoSize > 0) {
        if (nLayers <= 0) return false;
        if (nLayers != layers.size()) return false;
        foreach(PSDLayerRecord* layer, layers) {
            if (!layer->valid()) {
                return false;
            }
        }
    }
    if (!error.isNull()) {
        return false;
    }
    return true;
}
