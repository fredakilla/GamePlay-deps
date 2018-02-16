#!/bin/bash

GENIE='../bx/tools/bin/linux/genie'
OUT_INCLUDE='../out/external-deps/include'
#OUT_LIBRARY='../out/external-deps/lib/linux/x86_64'

BGFXBUILDER_DIR=$1
OUT_LIBRARY=$2

cd $BGFXBUILDER_DIR

# create projects
$GENIE --gcc=linux-gcc gmake

# compile projects
cd $BGFXBUILDER_DIR/build/projects/gmake-linux
make config=debug64
cd -

# copy bgfx libraries to output dir
echo "exporting bgfx libraries to $OUT_LIBRARY/"
cp build/linux64_gcc/bin/*.a $OUT_LIBRARY/

#
## copy necessary bgfx headers
#echo "exporting bgfx headers to $OUT_INCLUDE/"
#cp -r ../bx/include/* $OUT_INCLUDE/
#c#p -r ../bgfx/include/* $OUT_INCLUDE/
#cp -r ../bimg/include/* $OUT_INCLUDE/
#mkdir -p $OUT_INCLUDE/bgfx/tools/shaderc && cp -r ../bgfx/tools/shaderc/shaderc.h $OUT_INCLUDE/bgfx/tools/shaderc/
#mkdir -p $OUT_INCLUDE/bgfx/src && cp -r ../bgfx/src/vertexdecl.h $OUT_INCLUDE/bgfx/src

echo "Done."