# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(autocorrection_gui autocorrection_gui.cpp)
target_link_libraries(autocorrection_gui
  KF5::KIOCore
  KF5::PimCommon
  KF5::PimTextEdit
  KF5::I18n
)

add_executable(richtexteditwithautocorrection_gui richtexteditwithautocorrection_gui.cpp)
target_link_libraries(richtexteditwithautocorrection_gui
  KF5::KIOCore
  KF5::PimCommon
  KF5::PimTextEdit
  KF5::I18n
)

