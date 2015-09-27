#TODO
#set(oxygen_icon_dirs 16x16 22x22  32x32 48x48 64x64 128x128 scalable)
#install( DIRECTORY ${oxygen_icon_dirs} DESTINATION ${DATA_INSTALL_DIR}/kexi/icons/oxygen)

ecm_install_icons(ICONS
    16-actions-kexi_ox_database-import.png 22-actions-kexi_ox_database-import.png
    32-actions-kexi_ox_database-import.png 48-actions-kexi_ox_database-import.png
    32-actions-kexi_ox_document-empty.png 32-actions-kexi_ox_document-empty.svg
    48-actions-kexi_ox_document-empty.png 64-actions-kexi_ox_document-empty.svg

    16-actions-kexi_ox_table.png 22-actions-kexi_ox_table.png 32-actions-kexi_ox_table.png
    16-actions-kexi_ox_query.png 22-actions-kexi_ox_query.png 32-actions-kexi_ox_query.png
    16-actions-kexi_ox_form.png 22-actions-kexi_ox_form.png 32-actions-kexi_ox_form.png
    16-actions-kexi_ox_report.png 22-actions-kexi_ox_report.png 32-actions-kexi_ox_report.png
    16-actions-kexi_ox_script.png
    16-actions-kexi_ox_macro.png 22-actions-kexi_ox_macro.png 32-actions-kexi_ox_macro.png

    DESTINATION ${ICON_INSTALL_DIR}
    THEME oxygen
)
