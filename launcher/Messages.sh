#! /usr/bin/env bash
# SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
# SPDX-License-Identifier: CC0-1.0
$XGETTEXT `find -name \*.cpp -o -name \*.qml` -o $podir/calligralauncher.pot
