/* comp_dom.c: compile domains.dat */

#include "suds.h"
#include <ctype.h>
#include <string.h>

#define MAX_LINE 512
STRING line[MAX_LINE];

CHAR infile1[]="domain.num";
CHAR infile2[]="domain.nam";
CHAR outfile[]="domains.dat";

INTV main()
{
   FILE *in_name,*in_numb,*output;
   INT4 bytes,i,total;
   CHAR c;
   INT4 numbers[10],letters[26];

   in_numb=fopen(infile1,"r");
   if(in_numb==NULL) {
      fprintf(stderr,"Unable to open input file %s\n",infile1);
      exit(1);
   }
   in_name=fopen(infile2,"r");
   if(in_name==NULL) {
      fprintf(stderr,"Unable to open input file %s\n",infile2);
      exit(1);
   }
   output=fopen(outfile,"w");
   if(output==NULL) {
      fprintf(stderr,"Unable to open output file %s\n",outfile);
      exit(2);
   }
   bytes=0;
   total=numbers[0]=0;
   c='1';
   while(fgets(line,MAX_LINE,in_numb)!=NULL) {
      total+=strlen(line)+1;
      while(line[0]>c) {
         numbers[c-'0']=-1;
         if(++c>'9') break;
      }
      if(c==line[0]) {
         numbers[c-'0']=bytes;
         if(++c>'9') break;
      }
      bytes+=strlen(line)+1;
   }

   /*
   for(i=0;i<10;i++) {
      if(numbers[i]>=0) {
         fseek(in_numb,numbers[i],0);
         fgets(line,MAX_LINE,in_numb);
         printf("%s",line);
      }
      else printf("none\n");
   }
   */

   bytes=0;
   letters[0]=0;
   c='A';
   c='B';
   while(fgets(line,MAX_LINE,in_name)!=NULL) {
      while((isupper(line[12]) && line[12]>c) ||
         (islower(line[12]) && line[12]>(c-'A'+'a'))) {
         letters[c-'A']=-1;
         if(++c>'Z') break;
      }
      if(c==line[12] || (c-'A'+'a')==line[12]) {
         letters[c-'A']=bytes;
         if(++c>'Z') break;
      }
      bytes+=strlen(line)+1;
   }
   while(c<='Z') {
      letters[c-'A']=-1;
      c++;
   }
   /*
   for(i=0;i<26;i++) {
      if(letters[i]>=0) {
         fseek(in_name,letters[i],0);
         fgets(line,MAX_LINE,in_name);
         printf("%s",line);
      }
      else printf("none\n");
   }
   */

   /* \r added so line lengths are same in MSDOS, UNIX, etc. */
   fprintf(output,
      "/* DO NOT MODIFY DIRECTLY! Change manual page authorities(6) */\r\n");
   fprintf(output,
      "/* This file is compiled automatically by suds/man/man6/Makefile */\r\n");
   for(i=0;i<10;i++) {
      fprintf(output,"%ld ",numbers[i]);
   }
   fprintf(output,"\r\n");
   for(i=0;i<26;i++) {
      if(letters[i]!=-1)letters[i]+=total;
      fprintf(output,"%ld ",letters[i]);
      if(i==13)fprintf(output,"\r\n");
   }
   fprintf(output,"\r\n");
   rewind(in_numb);
   while(fgets(line,MAX_LINE,in_numb)!=NULL) {
      i=strlen(line);
      line[i-1]='\r';
      line[i]  ='\n';
      line[i+1]='\0';
      fputs(line,output);
   }
   rewind(in_name);
   while(fgets(line,MAX_LINE,in_name)!=NULL) {
      i=strlen(line);
      line[i-1]='\r';
      line[i]  ='\n';
      line[i+1]='\0';
      fputs(line,output);
   }
   fclose(in_name);
   fclose(in_numb);
   fclose(output);
   return(0);
}
