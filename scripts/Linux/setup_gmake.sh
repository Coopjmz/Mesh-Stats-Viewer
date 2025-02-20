#!/bin/bash

pushd ../..
tools/Premake/Linux/premake5 --verbose gmake
popd