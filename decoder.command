#!/bin/bash
cd $(dirname $0)
if [ -f qmc-decoder ]; then
./qmc-decoder
fi
