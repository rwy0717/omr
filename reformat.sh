compiler_dirs="\
compiler
jitbuilder
fvtest/compilertest
fvtest/compilertriltest
fvtest/jitbuildertest
fvtest/tril
"

core_dirs="\
ddr
example
fvtest
gc
glue
include_core
nls
omr
omrmakefiles
omrsigcompat
omrtrace
perftest
port
scripts
thread
tools
util
fvtest/algotest
fvtest/coretest
fvtest/gc_api_test
fvtest/gctest
fvtest/omrGtestGlue
fvtest/porttest
fvtest/rastest
fvtest/sigtest
fvtest/threadextendedtest
fvtest/threadtest
fvtest/util
fvtest/utiltest
fvtest/vmtest
"

extra_core_c_sources="\
omrversionstrings.CMakeTemplate.h
omrcfg.CMakeTemplate.h"

# ignored dirs:
# - cmake
# - doc
# - third_party

function git-c-sources-in {
	git ls-files "$@" | grep -E '.*\.[ch](pp)?$'
}

function git-cmake-sources-in {
	git ls-files "$@" | grep -E "^(.*\.cmake|CMakeLists\.txt)$"
}

function reformat-core-c-source {
	echo "reformatting (core): $@"
	gawk -f ./tabs-to-4spaces.awk -i inplace "$@"
	clang-format -i "$@"
}

function reformat-compiler-c-source {
	echo "reformatting (compiler): $@"
	gawk -f ./3spaces-to-4spaces.awk -i inplace "$@"
	clang-format -i "$@"
}

function reformat-cmake-source {
	echo "reformatting (cmake): $@"
	gawk -f ./tabs-to-4spaces.awk -i inplace "$@"
}

for f in $(git-c-sources-in $core_dirs); do
	reformat-core-c-source "$f"
done

for f in $extra_core_c_sources; do
	reformat-core-c-source "$f"
done

for f in $(git-c-sources-in $compiler_dirs); do
	reformat-compiler-c-source "$f"
done

for f in $(git-cmake-sources-in ./); do
	reformat-cmake-source "$f"
done
