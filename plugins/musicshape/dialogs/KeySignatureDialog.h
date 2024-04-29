/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KEYSIGNATUREDIALOG_H
#define KEYSIGNATUREDIALOG_H

#include <ui_KeySignatureDialog.h>

#include <KoDialog.h>
namespace MusicCore
{
class KeySignature;
}

class KeySignatureDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit KeySignatureDialog(QWidget *parent = nullptr);

    void setBar(int bar);
    int accidentals();
    void setAccidentals(int accidentals);
    void setMusicStyle(MusicStyle *style);
    bool updateAllStaves();

    bool updateToNextChange();
    bool updateTillEndOfPiece();

    int startBar();
    int endBar();
private Q_SLOTS:
    void accidentalsChanged(int accidentals);

private:
    Ui::KeySignatureDialog widget;
    MusicCore::KeySignature *m_ks;
};

#endif // KEYSIGNATUREDIALOG_H
