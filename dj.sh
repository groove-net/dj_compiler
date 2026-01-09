#!/bin/bash
# Rebuild silently
make -s all
# Run the binary with arguments
./bin/dj "$@"
