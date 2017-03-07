/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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
#include "ParsedPresentation.h"
#include "generated/leinputstream.h"
#include "PptDebug.h"

#include <QBuffer>

using namespace MSO;

bool
readStream(POLE::Storage& storage, const char* streampath, QBuffer& buffer)
{
    std::string path(streampath);
    if (storage.isDirectory("PP97_DUALSTORAGE")) {
        debugPpt << "PP97_DUALSTORAGE";
        path = "PP97_DUALSTORAGE" + path;
    }
    POLE::Stream stream(&storage, path);
    if (stream.fail()) {
        debugPpt << "Unable to construct " << streampath << "stream";
        return false;
    }

    QByteArray array;
    array.resize(stream.size());
    unsigned long r = stream.read((unsigned char*)array.data(), stream.size());
    if (r != stream.size()) {
        debugPpt << "Error while reading from " << streampath << "stream";
        return false;
    }
    buffer.setData(array);
    buffer.open(QIODevice::ReadOnly);
    return true;
}
bool
parseCurrentUserStream(POLE::Storage& storage, CurrentUserStream& cus)
{
    QBuffer buffer;
    if (!readStream(storage, "/Current User", buffer)) {
        return false;
    }
    LEInputStream stream(&buffer);
    try {
        parseCurrentUserStream(stream, cus);
    } catch (const IOException& e) {
        debugPpt << "caught IOException while parsing CurrentUserStream: " << " " << e.msg;
        debugPpt << "stream position: " << stream.getPosition();
        return false;
    } catch (...) {
        debugPpt << "caught unknown exception while parsing CurrentUserStream";
        return false;
    }
    if (stream.getPosition() != buffer.size()) {
        debugPpt << (buffer.size() - stream.getPosition())
        << "bytes left at the end of CurrentUserStream";
        return false;
    }
    return true;
}
bool
parsePowerPointStructs(POLE::Storage& storage, PowerPointStructs& pps)
{
    QBuffer buffer;
    if (!readStream(storage, "/PowerPoint Document", buffer)) {
        return false;
    }
    LEInputStream stream(&buffer);
    try {
        parsePowerPointStructs(stream, pps);
    } catch (const IOException& e) {
        debugPpt << "caught IOException while parsing PowerPointStructs " << " " << e.msg;
        debugPpt << "stream position: " << stream.getPosition();
        return false;
    } catch (...) {
        debugPpt << "caught unknown exception while parsing PowerPointStructs";
        return false;
    }
    if (stream.getPosition() != buffer.size()) {
        debugPpt << (buffer.size() - stream.getPosition())
        << "bytes left at the end of PowerPointStructs, so probably an error at position " << stream.getMaxPosition();
        return false;
    }
    return true;
}
bool
parsePictures(POLE::Storage& storage, PicturesStream& pps)
{
    QBuffer buffer;
    if (!readStream(storage, "/Pictures", buffer)) {
        debugPpt << "Failed to open /Pictures stream, no big deal (OPTIONAL).";
        return true;
    }
    LEInputStream stream(&buffer);
    try {
        parsePicturesStream(stream, pps);
    } catch (const IOException& e) {
        debugPpt << "caught IOException while parsing Pictures " << " " << e.msg;
        debugPpt << "stream position: " << stream.getPosition();
        return false;
    } catch (...) {
        debugPpt << "caught unknown exception while parsing Pictures";
        return false;
    }
    if (stream.getPosition() != buffer.size()) {
        debugPpt << (buffer.size() - stream.getPosition())
        << "bytes left at the end of PicturesStream, so probably an error at position " << stream.getMaxPosition();
        return false;
    }
    return true;
}

bool
parseSummaryInformationStream(POLE::Storage& storage, SummaryInformationPropertySetStream& sis)
{
    QBuffer buffer;
    if (!readStream(storage, "/SummaryInformation", buffer)) {
        debugPpt << "Failed to open /SummaryInformation stream, no big deal (OPTIONAL).";
        return true;
    }
    LEInputStream stream(&buffer);
    try {
        parseSummaryInformationPropertySetStream(stream, sis);
    } catch (const IOException& e) {
        debugPpt << "caught IOException while parsing SummaryInformation" << " " << e.msg;
        debugPpt << "stream position: " << stream.getPosition();
        return false;
    } catch (...) {
        debugPpt << "caught unknown exception while parsing SummaryInformation";
        return false;
    }
    return true;
}

/**
 * get the UserEditAtom that is at position @offset in the stream.
 * return 0 if it is not present.
 **/
template <typename T>
const T*
get(const PowerPointStructs& pps, quint32 offset)
{
    foreach(const PowerPointStruct& p, pps.anon) {
        if (p.anon.is<T>()
                && p.anon.get<T>()->streamOffset == offset) {
            return p.anon.get<T>();
        } else if (p.anon.is<MasterOrSlideContainer>()) {
            const MasterOrSlideContainer* m = p.anon.get<MasterOrSlideContainer>();
            if (m->anon.is<T>() && m->anon.get<T>()->streamOffset == offset) {
                return m->anon.get<T>();
            }
        }
    }
    return 0;
}
void
parsePersistDirectory(const PowerPointStructs& pps, const UserEditAtom* userEditAtom,
                      QMap<quint32, quint32>& persistDirectory)
{
    if (!userEditAtom) return;
    const PersistDirectoryAtom* persistDirectoryAtom
    = get<PersistDirectoryAtom>(pps, userEditAtom->offsetPersistDirectory);
    if (!persistDirectoryAtom) return;
    foreach(const PersistDirectoryEntry& pde, persistDirectoryAtom->rgPersistDirEntry) {
        for (int i = 0; i < pde.rgPersistOffset.size(); ++i) {
            if (!persistDirectory.contains(pde.persistId + i)) {
                persistDirectory[pde.persistId+i] = pde.rgPersistOffset[i];
            }
        }
    }
    quint32 offset = userEditAtom->offsetLastEdit;
    if (offset == 0) return;
    userEditAtom = get<UserEditAtom>(pps, offset);
    parsePersistDirectory(pps, userEditAtom, persistDirectory);
}

bool
ParsedPresentation::parse(POLE::Storage& storage)
{
    handoutMaster = 0;
    notesMaster = 0;

// read the CurrentUserStream and PowerPointStructs
    if (!parsePowerPointStructs(storage, presentation)) {
        debugPpt << "error parsing PowerPointStructs";
        return false;
    }
    if (!parseCurrentUserStream(storage, currentUserStream)) {
        debugPpt << "error parsing CurrentUserStream";
        return false;
    }
    if (!parsePictures(storage, pictures)) {
        debugPpt << "error parsing PicturesStream";
        return false;
    }
    if (!parseSummaryInformationStream(storage, summaryInfo)) {
        debugPpt << "error parsing SummaryInformationStream";
        return false;
    }

// Part 1: Construct the persist object directory
    const UserEditAtom* userEditAtom = get<UserEditAtom>(presentation,
                                       currentUserStream.anon1.offsetToCurrentEdit);
    if (!userEditAtom) {
        debugPpt << "no userEditAtom";
        return false;
    }
    parsePersistDirectory(presentation, userEditAtom, persistDirectory);
// Part 2: Identify the document persist object
    if (persistDirectory.contains(userEditAtom->docPersistIdRef)) {
        documentContainer = get<DocumentContainer>(presentation,
                            persistDirectory[userEditAtom->docPersistIdRef]);
    }
    if (!documentContainer) {
        debugPpt << "no documentContainer";
        return false;
    }
// Part 3: identify the note master slide persist object
    quint32 persistId = documentContainer->documentAtom.notesMasterPersistIdRef;
    if (persistId) {
        if (!persistDirectory.contains(persistId)) {
            debugPpt << "no notesMaster";
            return false;
        }
        notesMaster = get<NotesContainer>(presentation, persistDirectory[persistId]);
        if (!notesMaster) {
            debugPpt << "no notesMaster";
            return false;
        }
    }
// Part 4: Identify the handout master slide persist object
    persistId = documentContainer->documentAtom.handoutMasterPersistIdRef;
    if (persistId) {
        if (!persistDirectory.contains(persistId)) {
            debugPpt << "no handoutMaster";
            return false;
        }
        handoutMaster = get<HandoutContainer>(presentation, persistDirectory[persistId]);
        if (!handoutMaster) {
            debugPpt << "no handoutMaster";
            return false;
        }
    }
// Part 5: Identify the main master slide and title master slide persist objects
    int size = documentContainer->masterList.rgMasterPersistAtom.size();
    if (size == 0) {
        debugPpt << "no master slides";
        return false;
    }
    masters.resize(size);
    for (int i = 0; i < size;++i) {
        persistId = documentContainer->masterList.rgMasterPersistAtom[i].persistIdRef;
        if (!persistDirectory.contains(persistId)) {
            debugPpt << "cannot load master " << i;
            return false;
        }
        masters[i] = get<MasterOrSlideContainer>(presentation, persistDirectory[persistId]);
        if (!masters[i]) {
            debugPpt << "cannot load master " << i;
            return false;
        }
    }
// Part 6: Identify the presentation slide persist objects
    if (documentContainer->slideList) {
        size = documentContainer->slideList->rgChildRec.size();
        slides.resize(size);
        notes.resize(size);
        notes.fill(0);
        for (int i = 0; i < size;++i) {
            persistId = documentContainer->slideList->rgChildRec[i].slidePersistAtom.persistIdRef;
            if (!persistDirectory.contains(persistId)) {
                debugPpt << "cannot find persistId " << persistId << " for slide " << i;
                return false;
            }
            slides[i] = get<SlideContainer>(presentation, persistDirectory[persistId]);
            if (!slides[i]) {
                debugPpt << "cannot find slide " << i << " at offset "
                << persistDirectory[persistId];
                return false;
            }
        }
    }
// Part 7: Identify the notes slide persist object.  Notes are not that
// important as presentation slides, so do not invoke a filter crash here!

    //NOTE: MS Office XP stores invalid NotesPersistAtom records.

    if (documentContainer->notesList) {
        size = documentContainer->notesList->rgNotesPersistAtom.size();
        for (int i = 0; i < size; ++i) {
            const NotesPersistAtom& atom
                    = documentContainer->notesList->rgNotesPersistAtom[i];
            persistId = atom.persistIdRef;
            if (!persistDirectory.contains(persistId)) {
                debugPpt << "Invalid persistIdRef: cannot load notes";
                continue;
            }
            const NotesContainer* nc
               = get<NotesContainer>(presentation, persistDirectory[persistId]);
            if (!nc) {
                debugPpt << "NotesContainer missing: cannot load notes" << i;
                continue;
            }
            // find the slide the note belongs to
            int pos = -1;
            for (int j=0; j < slides.size(); ++j) {
                if (slides[j]->slideAtom.notesIdRef == atom.notesId) {
                    if (notes[j] != 0) {
                        debugPpt << "Invalid NotesContainer: slide " << j << " has already notes";
                        continue;
                    }
                    pos = j;
                }
            }
            if (pos == -1) {
                debugPpt << "Invalid NotesContainer: notes " << i << " do not belong to a slide";
                continue;
            }
            notes[pos] = nc;
        }
    }
// Part 8: Identify the ActiveX control persist objects
// Part 9: Identify the embedded OLE object persist objects
// Part 10: Identify the linked OLE object persist objects
// Part 11: Identify the VBA project persist object
    return true;
}
const MSO::MasterOrSlideContainer*
ParsedPresentation::getMaster(const SlideContainer* slide) const
{
    //masterIdRef MUST be 0x00000000 if the record that contains this SlideAtom
    //record is a MainMasterContainer record (MS-PPT 2.5.10)
    if (!slide) return 0;
    foreach(const MasterPersistAtom& m, documentContainer->masterList.rgMasterPersistAtom) {
        if (m.masterId == slide->slideAtom.masterIdRef) {
            quint32 offset = persistDirectory[m.persistIdRef];
            return get<MasterOrSlideContainer>(presentation, offset);
        }
    }
    return 0;
}
