
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
    cohere-blue.desktop
    cohere-gold.desktop
    cohere-magenta.desktop
    cohere-red.desktop
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
    cohere-blue.otp
    cohere-gold.otp
    cohere-magenta.otp
    cohere-red.otp
   )

set(thumbnail_FILES
    blue_orange_vector.png
    burning_desire.png
    business.png
    curious_penguin.png
    flood_light.png
    kde_events.png
    rounded_square.png
    simple_waves.png
    skyline_monotone.png
    skyline_monotone_wide.png
    skyline_morning.png
    skyline_morning_wide.png
    skyline_night.png
    skyline_night_wide.png
    strange_far_hills.png
    cohere-blue.png
    cohere-gold.png
    cohere-magenta.png
    cohere-red.png
)
#file(GLOB desktop_FILES *.desktop)
#file(GLOB otp_FILES *.otp)

install( FILES ${desktop_FILES} DESTINATION ${KDE_INSTALL_DATADIR}/calligrastage/templates/odf/ )
install( FILES ${otp_FILES} DESTINATION ${KDE_INSTALL_DATADIR}/calligrastage/templates/odf/.source)
install( FILES ${thumbnail_FILES} DESTINATION ${KDE_INSTALL_DATADIR}/calligrastage/templates/odf/.thumbnail)
install( FILES .directory DESTINATION ${KDE_INSTALL_DATADIR}/calligrastage/templates/odf/)

ecm_install_icons( ICONS
    64-actions-blue_orange_vector.png
    64-actions-burning_desire.png
    64-actions-business.png
    64-actions-curious_penguin.png
    64-actions-flood_light.png
    64-actions-kde_events.png
    64-actions-rounded_square.png
    64-actions-simple_waves.png
    64-actions-skyline_monotone.png
    64-actions-skyline_monotone_wide.png
    64-actions-skyline_morning.png
    64-actions-skyline_morning_wide.png
    64-actions-skyline_night.png
    64-actions-skyline_night_wide.png
    64-actions-strange_far_hills.png
    sc-actions-cohere-blue.svgz
    sc-actions-cohere-gold.svgz
    sc-actions-cohere-magenta.svgz
    sc-actions-cohere-red.svgz

    DESTINATION ${KDE_INSTALL_DATADIR}/calligrastage/icons
    THEME hicolor
)
