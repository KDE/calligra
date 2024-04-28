/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SetKeySignatureCommand.h"

#include "../core/Bar.h"
#include "../core/KeySignature.h"
#include "../core/Part.h"
#include "../core/Sheet.h"
#include "../core/Staff.h"

#include "../MusicShape.h"

#include <KLocalizedString>

using namespace MusicCore;

typedef QPair<Bar *, KeySignature *> BarKeySignaturePair;

SetKeySignatureCommand::SetKeySignatureCommand(MusicShape *shape, int bar, RegionType type, Staff *staff, int accidentals)
    : m_shape(shape)
    , m_staff(staff)
{
    setText(kundo2_i18n("Change key signature"));
    Sheet *sheet = shape->sheet();

    if (staff) {
        m_newKeySignatures.append(qMakePair(sheet->bar(bar), new KeySignature(staff, 0, accidentals)));

        for (int b = bar; b < sheet->barCount(); b++) {
            Bar *curBar = sheet->bar(b);

            for (int i = 0; i < curBar->staffElementCount(staff); i++) {
                KeySignature *c = dynamic_cast<KeySignature *>(curBar->staffElement(staff, i));
                if (c && c->startTime() == 0) {
                    m_oldKeySignatures.append(qMakePair(curBar, c));
                    break;
                }
            }

            if (type == NextChange)
                break;
        }
    } else {
        for (int p = 0; p < sheet->partCount(); p++) {
            Part *part = sheet->part(p);
            for (int s = 0; s < part->staffCount(); s++) {
                Staff *staff = part->staff(s);
                m_newKeySignatures.append(qMakePair(sheet->bar(bar), new KeySignature(staff, 0, accidentals)));

                for (int b = bar; b < sheet->barCount(); b++) {
                    Bar *curBar = sheet->bar(b);

                    for (int i = 0; i < curBar->staffElementCount(staff); i++) {
                        KeySignature *ks = dynamic_cast<KeySignature *>(curBar->staffElement(staff, i));
                        if (ks) {
                            m_oldKeySignatures.append(qMakePair(curBar, ks));
                            break;
                        }
                    }

                    if (type == NextChange)
                        break;
                }
            }
        }
    }
}

SetKeySignatureCommand::SetKeySignatureCommand(MusicShape *shape, int startBar, int endBar, MusicCore::Staff *staff, int accidentals)
    : m_shape(shape)
    , m_staff(staff)
{
    setText(kundo2_i18n("Change key signature"));
    Sheet *sheet = shape->sheet();

    if (staff) {
        m_newKeySignatures.append(qMakePair(sheet->bar(startBar), new KeySignature(staff, 0, accidentals)));

        for (int b = startBar; b <= endBar; b++) {
            Bar *curBar = sheet->bar(b);

            for (int i = 0; i < curBar->staffElementCount(staff); i++) {
                KeySignature *c = dynamic_cast<KeySignature *>(curBar->staffElement(staff, i));
                if (c && c->startTime() == 0) {
                    m_oldKeySignatures.append(qMakePair(curBar, c));
                    break;
                }
            }
        }

        // Figure out old key signature in endBar if more bars follow
        if (endBar < sheet->barCount() - 1) {
            KeySignature *ks = staff->lastKeySignatureChange(endBar + 1);
            if (!ks || ks->bar() != sheet->bar(endBar + 1)) {
                KeySignature *n = new KeySignature(staff, 0, ks ? ks->accidentals() : 0);
                m_newKeySignatures.append(qMakePair(sheet->bar(endBar + 1), n));
            }
        }
    } else {
        for (int p = 0; p < sheet->partCount(); p++) {
            Part *part = sheet->part(p);
            for (int s = 0; s < part->staffCount(); s++) {
                Staff *staff = part->staff(s);
                m_newKeySignatures.append(qMakePair(sheet->bar(startBar), new KeySignature(staff, 0, accidentals)));

                for (int b = startBar; b <= endBar; b++) {
                    Bar *curBar = sheet->bar(b);

                    for (int i = 0; i < curBar->staffElementCount(staff); i++) {
                        KeySignature *ks = dynamic_cast<KeySignature *>(curBar->staffElement(staff, i));
                        if (ks) {
                            m_oldKeySignatures.append(qMakePair(curBar, ks));
                            break;
                        }
                    }
                }

                // Figure out old key signature in endBar if more bars follow
                if (endBar < sheet->barCount() - 1) {
                    KeySignature *ks = staff->lastKeySignatureChange(endBar + 1);
                    if (!ks || ks->bar() != sheet->bar(endBar + 1)) {
                        KeySignature *n = new KeySignature(staff, 0, ks ? ks->accidentals() : 0);
                        m_newKeySignatures.append(qMakePair(sheet->bar(endBar + 1), n));
                    }
                }
            }
        }
    }
}

void SetKeySignatureCommand::redo()
{
    foreach (const BarKeySignaturePair &p, m_oldKeySignatures) {
        p.first->removeStaffElement(p.second, false);
    }
    foreach (const BarKeySignaturePair &p, m_newKeySignatures) {
        p.first->addStaffElement(p.second);
    }
    if (m_staff) {
        m_staff->updateAccidentals();
    } else {
        m_shape->sheet()->updateAccidentals();
    }
    m_shape->engrave();
    m_shape->update();
}

void SetKeySignatureCommand::undo()
{
    foreach (const BarKeySignaturePair &p, m_newKeySignatures) {
        p.first->removeStaffElement(p.second, false);
    }
    foreach (const BarKeySignaturePair &p, m_oldKeySignatures) {
        p.first->addStaffElement(p.second);
    }
    if (m_staff) {
        m_staff->updateAccidentals();
    } else {
        m_shape->sheet()->updateAccidentals();
    }
    m_shape->engrave();
    m_shape->update();
}
