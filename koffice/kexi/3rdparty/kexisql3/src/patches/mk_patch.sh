#!/bin/sh

cd ..

diff -u ../../sqlite-3.2.8.orig/ . | grep -v "^Only in"
