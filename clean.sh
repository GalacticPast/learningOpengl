#!/bin/bash
# Build script for rebuilding everything
set echo on

echo "cleaning everything..."

if [[ -d "bin" ]]; then 
    rm -rf -v bin
fi

if [[ -d "src/platform/wayland" ]]; then 
    rm -rf -v src/platform/wayland
fi

ERROELEVEL=$?
if [[ $DERRORLEVEL -ne 0 ]]; then
echo "DERROR:"$DERRORLEVEL && exit
fi

if [[ -d "obj" ]]; then 
    rm -rf -v obj 
fi

ERROELEVEL=$?
if [[ $DERRORLEVEL -ne 0 ]]
then
echo "DERROR:"$DERRORLEVEL && exit
fi

echo "All assemblies cleaned successfully."

