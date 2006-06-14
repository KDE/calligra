




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kchart/pics FILES  chart_bar_beside.png chart_bar_layer.png chart_bar_percent.png  			chart_hilo_close.png chart_hilo_normal.png chart_hilo_openclose.png   			chart_area_normal.png chart_area_percent.png chart_area_stacked.png  			chart_line_normal.png chart_line_percent.png chart_line_stacked.png  			chart_polar_normal.png chart_polar_percent.png chart_polar_stacked.png  			chart_legend_top.png chart_legend_topleft.png chart_legend_topright.png  			chart_legend_nolegend.png chart_legend_left.png chart_legend_right.png  			chart_legend_bottom.png chart_legend_bottomleft.png chart_legend_bottomright.png )

kde4_install_icons( ${ICON_INSTALL_DIR}  kchart )





#original Makefile.am contents follow:

#
#KDE_ICON = kchart
#
#kchartpics_DATA = chart_bar_beside.png chart_bar_layer.png chart_bar_percent.png \
#			chart_hilo_close.png chart_hilo_normal.png chart_hilo_openclose.png  \
#			chart_area_normal.png chart_area_percent.png chart_area_stacked.png \
#			chart_line_normal.png chart_line_percent.png chart_line_stacked.png \
#			chart_polar_normal.png chart_polar_percent.png chart_polar_stacked.png \
#			chart_legend_top.png chart_legend_topleft.png chart_legend_topright.png \
#			chart_legend_nolegend.png chart_legend_left.png chart_legend_right.png \
#			chart_legend_bottom.png chart_legend_bottomleft.png chart_legend_bottomright.png
#			
#kchartpicsdir = $(kde_datadir)/kchart/pics
