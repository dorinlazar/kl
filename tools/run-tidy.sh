#!/bin/bash
set -e

sed 's/\+\+23/\+\+2b/g' -i build/compile_commands.json
if [ $# -eq 0 ]; then
  find src -name \*pp | xargs -P 8 -L 2 -r -- clang-tidy -p build/compile_commands.json 
else
  clang-tidy -p build/compile_commands.json "$@"
fi

