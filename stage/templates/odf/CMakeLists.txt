
# for now only install the desktop files of the templates that are ready
# but include all templates so that the translation team can do their work
set(desktop_FILES
    blue_orange_vector.desktop
    burning_desire.desktop
    business.desktop
    curious_penguin.desktop
    flood_light.desktop
    kde_events.desktop
    simple_waves.desktop
    rounded_square.desktop
    skyline_monotone.desktop
    skyline_monotone_wide.desktop
    skyline_morning.desktop
    skyline_morning_wide.desktop
    skyline_night.desktop
    skyline_night_wide.desktop
    strange_far_hills.desktop
   )

set(otp_FILES
    blue_orange_vector.otp
    burning_desire.otp
    business.otp
    curious_penguin.otp
    flood_light.otp
    kde_events.otp
    rounded_square.otp
    simple_waves.otp
    skyline_monotone.otp
    skyline_monotone_wide.otp
    skyline_morning.otp
    skyline_morning_wide.otp
    skyline_night.otp
    skyline_night_wide.otp
    strange_far_hills.otp
   )
#file(GLOB desktop_FILES *.desktop)
#file(GLOB otp_FILES *.otp)

install( FILES ${desktop_FILES} DESTINATION ${DATA_INSTALL_DIR}/stage/templates/odf/ )
install( FILES ${otp_FILES} DESTINATION ${DATA_INSTALL_DIR}/stage/templates/odf/.source)
install( FILES .directory DESTINATION ${DATA_INSTALL_DIR}/stage/templates/odf/)

kde4_install_icons( ${DATA_INSTALL_DIR}/stage/icons  )
