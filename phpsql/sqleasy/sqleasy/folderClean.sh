#!/bin/bash
rm *~
echo -e "*~ Back Up files deleted.\n";
#rm *.dot
#echo -e "*.dot Data files deleted.\n";
rm *.o
echo -e "*.o Compiled but non linked files deleted.\n";
rm *.out
echo -e "*.out Output files deleted.\n";
rm lex.yy.c y.tab.c
echo -e "lex and yacc files deleted.\n";
