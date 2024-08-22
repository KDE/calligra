# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

add_library(calligra_filter_wpg2svg MODULE WPGImport.cpp)

target_link_libraries(calligra_filter_wpg2svg
    komain
    LibWpg::LibWpg
    LibRevenge::LibRevenge
    LibRevenge::LibRevengeStream
)

install(TARGETS calligra_filter_wpg2svg DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)
