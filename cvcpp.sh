#!/bin/sh

if [ $# -gt 0 ] ; then
    base=`basename $1`
    rm $base
    echo "compiling $base"
    g++ -ggdb `pkg-config --cflags opencv` -o $base $base.cpp `pkg-config --libs opencv`;
fi
