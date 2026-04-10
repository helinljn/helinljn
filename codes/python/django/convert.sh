#!/bin/bash

find . -name "*.py" | xargs dos2unix
find . -name "*.md" | xargs unix2dos
find . -name "*.html" | xargs unix2dos