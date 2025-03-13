#!/bin/bash
# Build script for rebuilding everything
set echo on

echo "cleaning everything..."

if [[ -d "bin" ]]; then 
    rm -rf -v bin
fi

ERROELEVEL=$?
if [[ $ERRORLEVEL -ne 0 ]]; then
echo "Error:"$ERRORLEVEL && exit
fi

if [[ -d "obj" ]]; then 
    rm -rf -v obj 
fi

ERROELEVEL=$?
if [[ $ERRORLEVEL -ne 0 ]]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "All assemblies cleaned successfully."

