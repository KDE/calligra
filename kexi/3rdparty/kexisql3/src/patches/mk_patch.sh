#!/bin/sh

cd ..

diff -u . ../../sqlite-3.0.7.orig/ | grep -v "^Only in"
