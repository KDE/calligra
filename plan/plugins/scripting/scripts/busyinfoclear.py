#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import os, datetime, sys, traceback, pickle
import Kross, Plan

T = Kross.module("kdetranslation")

#TODO some ui
Plan.beginCommand( T.i18nc( "(qtundoformat)", "Clear all external appointments" ) )
Plan.project().clearExternalAppointments()
Plan.endCommand()
