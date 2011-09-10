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
#include <QtCore/QBuffer>
#include <QtCore/QDebug>

using namespace MSO;

bool
readStream(POLE::Storage& storage, const char* streampath, QBuffer& buffer)
{
    std::string path(streampath);
    if (storage.isDirectory("PP97_DUALSTORAGE")) {
        qDebug() << "PP97_DUALSTORAGE";
        path = "PP97_DUALSTORAGE" + path;
    }
    POLE::Stream stream(&storage, path);
    if (stream.fail()) {
        qDebug() << "Unable to construct " << streampath << "stream";
        return false;
    }

    QByteArray array;
    array.resize(stream.size());
    unsigned long r = stream.read((unsigned char*)array.data(), stream.size());
    if (r != stream.size()) {
        qDebug() << "Error while reading from " << streampath << "stream";
        return false;
    }
    buffer.setData(array);
    buffer.open(QIODevice::ReadOnly);
    return true;
}
template<class T>
QByteArray
parseStream(const char* path, POLE::Storage& storage, T& t)
{
    QBuffer buffer;
    QByteArray data;
    if (!readStream(storage, path, buffer)) {
        return QByteArray();
    }
    data = buffer.buffer();
    t = T(data.data(), data.size());
    if (!t.isValid()) {
        qDebug() << "caught unknown error while parsing " << path;
        return QByteArray();
    }
    if (t.getSize() != data.size()) {
        qDebug() << (data.size() - t.getSize())
                 << "bytes left at the end of " << path << " of length "
                    << data.size();
        //return QByteArray();
    }
    return data;
}
/**
  * get the UserEditAtom that is at position @offset in the stream.
  * return 0 if it is not present.
  **/
template <typename T>
T
get(const PowerPointStructs& pps, quint32 offset)
{
    if (offset > pps.getSize()) {
        return T();
    }
    return T(pps.getData() + offset, pps.getSize() - offset);
}
void
parsePersistDirectory(const PowerPointStructs& pps, const UserEditAtom& userEditAtom,
                      QMap<quint32, quint32>& persistDirectory)
{
    const PersistDirectoryAtom persistDirectoryAtom
            = get<PersistDirectoryAtom>(pps, userEditAtom.offsetPersistDirectory());
    if (!persistDirectoryAtom.isValid()) return;
    foreach(const PersistDirectoryEntry& pde, persistDirectoryAtom.rgPersistDirEntry()) {
        for (int i = 0; i < pde.rgPersistOffset().getCount(); ++i) {
            if (!persistDirectory.contains(pde.persistId() + i)) {
                persistDirectory[pde.persistId()+i] = pde.rgPersistOffset()[i];
            }
        }
    }
    quint32 offset = userEditAtom.offsetLastEdit();
    if (offset == 0) return;
    parsePersistDirectory(pps, get<UserEditAtom>(pps, offset), persistDirectory);
}

bool
ParsedPresentation::parse(POLE::Storage& storage)
{
    handoutMaster = HandoutContainer();
    notesMaster = NotesContainer();

// read the CurrentUserStream and PowerPointStructs
    presentationData = parseStream("/PowerPoint Document", storage, presentation);
    if (!presentation.isValid()) {
        qDebug() << "error parsing PowerPointStructs";
        return false;
    }
    qDebug() << "PRESENTATION " << presentation.getSize();
    currentUserStreamData = parseStream("/Current User", storage, currentUserStream);
    if (!currentUserStream.isValid()) {
        qDebug() << "error parsing CurrentUserStream";
        return false;
    }
    picturesData = parseStream("/Pictures", storage, pictures);
    if (!pictures.isValid()) {
        qDebug() << "error parsing PicturesStream";
        //return false;
    }
// Part 1: Construct the persist object directory
    const UserEditAtom userEditAtom = get<UserEditAtom>(presentation,
                                                         currentUserStream.anon1().offsetToCurrentEdit());
    if (!userEditAtom.isValid()) {
        qDebug() << "no userEditAtom " << currentUserStream.anon1().offsetToCurrentEdit() << " " << presentation.getSize() << " " << presentationData.size();
        return false;
    }
    parsePersistDirectory(presentation, userEditAtom, persistDirectory);
// Part 2: Identify the document persist object
    if (persistDirectory.contains(userEditAtom.docPersistIdRef())) {
        documentContainer = get<DocumentContainer>(presentation,
                                                   persistDirectory[userEditAtom.docPersistIdRef()]);
    }
    if (!documentContainer.isValid()) {
        qDebug() << "no documentContainer";
        return false;
    }
// Part 3: identify the note master slide persist object
    quint32 persistId = documentContainer.documentAtom().notesMasterPersistIdRef();
    if (persistId) {
        if (!persistDirectory.contains(persistId)) {
            qDebug() << "no notesMaster";
            return false;
        }
        notesMaster = get<NotesContainer>(presentation, persistDirectory[persistId]);
        if (!notesMaster.isValid()) {
            qDebug() << "no notesMaster";
            return false;
        }
    }
// Part 4: Identify the handout master slide persist object
    persistId = documentContainer.documentAtom().handoutMasterPersistIdRef();
    if (persistId) {
        if (!persistDirectory.contains(persistId)) {
            qDebug() << "no handoutMaster";
            return false;
        }
        handoutMaster = get<HandoutContainer>(presentation, persistDirectory[persistId]);
        if (!handoutMaster.isValid()) {
            qDebug() << "no handoutMaster";
            return false;
        }
    }
// Part 5: Identify the main master slide and title master slide persist objects
    int size = documentContainer.masterList().rgMasterPersistAtom().getCount();
    if (size == 0) {
        qDebug() << "no master slides";
        return false;
    }
    masters.resize(size);
    for (int i = 0; i < size;++i) {
        persistId = documentContainer.masterList().rgMasterPersistAtom()[i].persistIdRef();
        if (!persistDirectory.contains(persistId)) {
            qDebug() << "cannot load master " << i;
            return false;
        }
        masters[i] = get<MasterOrSlideContainer>(presentation, persistDirectory[persistId]);
        if (!masters[i].isValid()) {
            qDebug() << "cannot load master " << i;
            return false;
        }
    }
// Part 6: Identify the presentation slide persist objects
    if (documentContainer.slideList().isPresent()) {
        MSOArray<SlideListWithTextSubContainerOrAtom> c = (*documentContainer.slideList()).rgChildRec();
        size = c.getCount();
        slides.resize(size);
        notes.resize(size);
        notes.fill(NotesContainer());
        for (int i = 0; i < size;++i) {
            persistId = c[i].slidePersistAtom().persistIdRef();
            if (!persistDirectory.contains(persistId)) {
                qDebug() << "cannot find persistId " << persistId << " for slide " << i;
                return false;
            }
            slides[i] = get<SlideContainer>(presentation, persistDirectory[persistId]);
            if (!slides[i].isValid()) {
                qDebug() << "cannot find slide " << i << " at offset "
                << persistDirectory[persistId];
                return false;
            }
        }
    }
// Part 7: Identify the notes slide persist object.  Notes are not that
// important as presentation slides, so do not invoke a filter crash here!

    //NOTE: MS Office XP stores invalid NotesPersistAtom records.

    if (documentContainer.notesList().isPresent()) {
        MSOArray<NotesPersistAtom> a = (*documentContainer.notesList()).rgNotesPersistAtom();
        size = a.getCount();
        for (int i = 0; i < size; ++i) {
            const NotesPersistAtom& atom = a[i];
            persistId = atom.persistIdRef();
            if (!persistDirectory.contains(persistId)) {
                qDebug() << "Invalid persistIdRef: cannot load notes";
                continue;
            }
            const NotesContainer nc
               = get<NotesContainer>(presentation, persistDirectory[persistId]);
            if (!nc.isValid()) {
                qDebug() << "NotesContainer missing: cannot load notes" << i;
                continue;
            }
            // find the slide the note belongs to
            int pos = -1;
            for (int j=0; j < slides.size(); ++j) {
                if (slides[j].slideAtom().notesIdRef() == atom.notesId()) {
                    if (!notes[j].isValid()) {
                        qDebug() << "Invalid NotesContainer: slide " << j << " has already notes";
                        continue;
                    }
                    pos = j;
                }
            }
            if (pos == -1) {
                qDebug() << "Invalid NotesContainer: notes " << i << " do not belong to a slide";
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
MSO::MasterOrSlideContainer
ParsedPresentation::getMaster(const SlideContainer& slide) const
{
    //masterIdRef MUST be 0x00000000 if the record that contains this SlideAtom
    //record is a MainMasterContainer record (MS-PPT 2.5.10)
    foreach(const MasterPersistAtom& m, documentContainer.masterList().rgMasterPersistAtom()) {
        if (m.masterId() == slide.slideAtom().masterIdRef()) {
            quint32 offset = persistDirectory[m.persistIdRef()];
            return get<MasterOrSlideContainer>(presentation, offset);
        }
    }
    return MasterOrSlideContainer();
}
