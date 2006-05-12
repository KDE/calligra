kde4_header()




########### install files ###############

INSTALL_FILES( ${DATA_INSTALL_DIR}/kspread/functions FILES
	builtin.xml
	financial.xml
	trig.xml
	conversion.xml
	datetime.xml
	database.xml
	statistical.xml
	text.xml
	logic.xml
	math.xml
	information.xml
	engineering.xml
	reference.xml
)

kde4_footer()



#original Makefile.am contents follow:

#
#scripts_DATA = builtin.xml financial.xml trig.xml conversion.xml datetime.xml \
#	database.xml statistical.xml text.xml logic.xml math.xml information.xml \
#	engineering.xml reference.xml
#scriptsdir = $(kde_datadir)/kspread/functions
#
