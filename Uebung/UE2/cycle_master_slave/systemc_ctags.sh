#!/bin/bash

echo "Generate Tags Database"
ctags -e -R -f TAGS .
ctags -e -R -a -f TAGS /usr/local/systemc-2.3.2/include/
ctags -e -R -a --language-force=c++ -h=".h.H.hh.hpp.hxx.h++.inc.def." --c++-kinds=+l --file-scope=no --tag-relative=no -f TAGS /usr/include/c++/7
echo "Finished Generation"
