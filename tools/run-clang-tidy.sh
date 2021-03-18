#!/bin/sh
# SPDX-FileCopyrightText: 2020 Daniel Vrátil <dvratil@kde.org>
#
# SPDX-License-Identifier: LGPL-2.0-or-later

if [ $# -lt 2 ]; then
    >&2 echo "Usage: $0 SOURCE_DIR BUILD_DIR"
    exit 1
fi

set -xe

source_dir=$1; shift
build_dir=$1; shift 1

function sanitize_compile_commands
{
    local cc_file=${build_dir}/compile_commands.json
    local filter_file="${source_dir}/.clang-tidy-ignore"

    if [ ! -f "${cc_file}" ]; then
        >&2 echo "Couldn't find compile_commands.json"
        exit 1
    fi

    if [ ! -f "${filter_file}" ]; then
        return 0
    fi

    filter_files=$(cat ${filter_file} | grep -vE "^#\.*|^$" | tr '\n' '|' | head -c -1)

    local cc_bak_file=${cc_file}.bak
    mv ${cc_file} ${cc_bak_file}

    cat ${cc_bak_file} \
        | jq -r "map(select(.file|test(\"${filter_files}\")|not))" \
        > ${cc_file}

    task_count=$(cat ${cc_file} | jq "length")
}

sanitize_compile_commands

run-clang-tidy -p ${build_dir} -j$(nproc) -q $@ | tee ${build_dir}/clang-tidy.log
cat ${build_dir}/clang-tidy.log | ${source_dir}/tools/clang-tidy-to-junit.py ${source_dir} > ${build_dir}/clang-tidy-report.xml

