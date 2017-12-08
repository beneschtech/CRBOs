#!/usr/bin/awk -f

BEGIN {
   print "#include <corelib/krnlsyms.h>";
   print
   print "struct symEntry symTable[] = {";
   print "   { 0x6000 , \"BOOTLOADER\" },"
   cmdstr="objdump -x -C ../kernel | sort | grep 'F .text' | uniq";
   while (cmdstr | getline) {
      ostr="   { 0x"$1", \""$6;
      for (i=7; i <= NF; i++) {
         ostr=ostr" "$i;
      }
      ostr=ostr"\" },";
      print ostr;
   }
   print "   { 0x0000000000000000, \"\" }";
   print "};"
}
