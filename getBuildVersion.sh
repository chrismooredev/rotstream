#!/usr/bin/env bash

MAJOR=0
MINOR=2

#Use short commit hash as ID
REVISION=$(git rev-parse --short HEAD)

#Append '-dirty' if there are uncommited changes
if ! git diff-index --quiet HEAD -- ; then
    REVISION+="-dirty"
fi

#This script is passed directly to the compiler through $(thisScript) so output defines as command line arguments
REVISION="-DVERSION_BUILD=$REVISION"
REVISION+=" -DVERSION_MAJOR=$MAJOR"
REVISION+=" -DVERSION_MINOR=$MINOR"

echo -n "$REVISION"