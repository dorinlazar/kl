#!/bin/bash

set -e

cmake --preset default

sed 's/\+\+23/\+\+2b/g' -i build/compile_commands.json
CodeChecker analyze build/compile_commands.json -o build/reports
CodeChecker parse build/reports -e html -o build/reports/html
