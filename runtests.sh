#! /bin/bash

TEST_DIR=tests/
EXEC=hw2
OUTPUT=out
TEMP=temp
DIFF_FILE=diff_file

RED='\033[41;37m'
GREEN='\033[42m'
BLUE='\033[44m'
RESET='\033[0m'

# after exporting envfile,
# use absolute path for commands [date, cut, sed] since PATH might be overwritten
DATE=$(which date)
CUT=$(which cut)
SED=$(which sed)
ECHO=$(which echo)
CAT=$(which cat)
TEST=$(which test)
MV=$(which mv)
MD5SUM=$(which md5sum)
DIFF=$(which diff)

# a backup of the PATH variable
PATH_BACKUP=$PATH

make all

# check if running sub test

if [ $# -eq 1 ]; then
    TEST_DIR=tests/$1
fi

# run tests

for IN in $(ls -1 -d ${TEST_DIR}*.in | sort)
do
    PASS=1
    NAME=$($ECHO ${IN} | cut -d '.' -f1 | cut -d '/' -f2)
    # inform which test will run
    $ECHO -e "\n\n${BLUE} Running Test ${RESET} ${NAME}"

    # set up environment if applicable
    ENVFILE=$($SED 's/.in$/.env/g' <<< ${IN})
    export $($CAT $ENVFILE | xargs) &> /dev/null || true

    # exec
    T1=$($DATE +%s%N | $CUT -b1-13 | $SED s/N/000/g)
    ./${EXEC} < ${IN} &> $OUTPUT
    RET=$?
    T2=$($DATE +%s%N | $CUT -b1-13 | $SED s/N/000/g)
    TT=$((T2-T1))

    # identify important files
    TITLE="DIFF"
    EXPECTED=$($SED 's/.in$/.expected/g' <<< ${IN})
    MD5=$($SED 's/.in/.md5/g' <<< ${IN})
    if $TEST -f "$MD5"; then
        EXPECTED=$($ECHO ${MD5})
        $CAT $OUTPUT | $MD5SUM > $TEMP
        $MV $TEMP $OUTPUT
        TITLE="MD5"
    fi

    # determine if run failed
    if [ $RET -eq 0 ]; then
        $ECHO -e "${GREEN} exec OK in ${TT}ms ${RESET}"
    else
        $ECHO -e "${RED} exec FAIL in ${TT}ms ${RESET}"
        PASS=0
    fi

    # determine if output matches
    $DIFF -y $EXPECTED $OUTPUT &> "$DIFF_FILE"
    DIFF_RET=$?
    if [ $DIFF_RET -eq 0 ]; then
        $ECHO -e "${GREEN} ${TITLE} OK ${RESET}"
    else
        $ECHO -e "${RED} ${TITLE} FAILED ${RESET}"
        $ECHO "diff -y \$EXPECTED \$OUTPUT"
        $CAT "$DIFF_FILE"
        PASS=0
    fi

    # determine if test was passed
    if [ $PASS -eq 0 ]; then
        $ECHO -e "${RED} TEST ${NAME} FAILED ${RESET}"
    else
        $ECHO -e "${GREEN} TEST ${NAME} PASSED ${RESET}"
    fi
    PATH=$PATH_BACKUP
done

rm -f "$DIFF_FILE" $TEMP $OUTPUT
make clean
