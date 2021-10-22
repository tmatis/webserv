#!/bin/bash

# CAR for collect and run

# CAR will collect all the prototypes of the unit tests
# and put them in a file collected.hpp then compile them with OBJS and OBJS_TEST

# Check if the file collected.hpp exists

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color


if [ -f collected.hpp ]; then
	rm collected.hpp;
fi

if [ -f collected.cpp ]; then
	rm collected.cpp;
fi

echo "// This file is generated by CAR.sh" > collected.hpp;
echo "#ifndef COLLECTED_HPP" >> collected.hpp;
echo "# define COLLECTED_HPP" >> collected.hpp;
echo "" >> collected.hpp;
echo '# include "car.hpp"' >> collected.hpp;
echo "" >> collected.hpp;

echo "//this file is generated by CAR.sh" > collected.cpp;
echo "#include \"collected.hpp\"" >> collected.cpp;
echo "" >> collected.cpp;
echo "int main(int argc, char **argv)" >> collected.cpp;
echo "{" >> collected.cpp;
echo "	if (argc == 2)" >> collected.cpp;
echo "		filter = argv[1];" >> collected.cpp;
echo "	else" >> collected.cpp;
echo "		filter = \"\";" >> collected.cpp;
echo "	test_function *tests[] = {" >> collected.cpp;


# Collect all the prototypes of the unit tests
echo -e -n "${BLUE}Collecting tests modules... ${NC}"
CPP_FILES=`find . -type f -name "*.cpp" -exec ls -rt '{}' +`;
for file in $CPP_FILES; do
	PROTO=`grep car_test $file`;
	if [ "$PROTO" != "" ]; then
		N_PROTO=`echo "$PROTO" | wc -l`;
		IFS=$'\n';
		for proto in $PROTO; do
			echo "	$proto; // $file" >> collected.hpp;
			NAME=`echo $proto | sed 's/	/ /g' | cut -d ' ' -f2 | cut -d '(' -f1`
			echo "		$NAME," >> collected.cpp;
		done
	fi
done

echo "		NULL" >> collected.cpp;
echo "	};" >> collected.cpp;
echo "" >> collected.cpp;
echo "	return (run_tests(tests));" >> collected.cpp;
echo "}" >> collected.cpp;

echo "" >> collected.hpp;
echo "#endif" >> collected.hpp;

OBJS=""
for objs in "$@"; do
	OBJS="$OBJS ../$objs";
done
if [ "${OBJS:0:1}" == " " ]; then
	OBJS="${OBJS:1}";
fi

echo -e "${GREEN}done${NC}" 

CPP_FILES=`echo $CPP_FILES`
echo -e -n "${BLUE}Generating Makefile... ${NC}"
cat Makefile.template | sed "s@$(echo objs_template | sed 's/\./\\./g')@${OBJS}@g" | sed "s@$(echo srcs_template | sed 's/\./\\./g')@${CPP_FILES}@g" > Makefile;
echo -e "${GREEN}done${NC}"
echo -e -n "${BLUE}Compiling... ${NC}"
make -s
echo -e "${GREEN}done${NC}"
echo 
