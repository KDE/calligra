#!/bin/bash

cd ..
patches/remove_id.sh

diff -u ../../sqlite-3.2.8.orig/ . | grep -v "^Only in"
