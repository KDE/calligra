#ifndef PARSEDPRESENTATION_H
#define PARSEDPRESENTATION_H

#include "mso/simpleParser.h"
#include "pole.h"

class ParsedPresentation
{
public:
    PPT::CurrentUserStream currentUserStream;
    PPT::PowerPointStructs presentation;
    // map persistObjectIds to stream offsets
    QMap<quint32, quint32> persistDirectory;
    const PPT::DocumentContainer* documentContainer;
    const PPT::NotesContainer* notesMaster;
    const PPT::HandoutContainer* handoutMaster;
    QVector<const PPT::MasterOrSlideContainer*> masters;
    QVector<const PPT::SlideContainer*> slides;
    QVector<const PPT::NotesContainer*> notes;

    ParsedPresentation() {
        documentContainer = 0;
        notesMaster = 0;
        handoutMaster = 0;
    }

    const PPT::MasterOrSlideContainer* getMaster(const PPT::SlideContainer* slide) const;
    bool parse(POLE::Storage& storage);
};

#endif // PARSEDPRESENTATION_H
