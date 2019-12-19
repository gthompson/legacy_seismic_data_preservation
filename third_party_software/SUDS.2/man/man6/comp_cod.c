/* comp_cod.c: compile codelist.dat from code_lists.6 
   codelist.dat is a file containing code_list(3) structures each followed by
   the code_list.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "suds.h"

#define MAX_LINE 512

#define MAXLIST	500
#define MAX_LINE 512
#define SUDS_INTERNAL 2
#define BEGIN -99999

CHAR infile[] = "code_lists.tmp";
CHAR outfile[]= "codelist.dat";

#if defined (__STDC__) || defined (__cplusplus)
void die(INTV n) { exit(n); }
#else
void die(n) INTV n; { exit(n); }
#endif

STRING temp_list[MAXLIST*48];
SUDS_CODE_LIST cl;
INT4 chr_ptr,old_number;
INT4 *offsets;
INT4 number[2*MAXLIST];

#if defined (__STDC__) || defined (__cplusplus)
void process_line(CHAR *line, SUDS_STREAM *out_file)
#else
void process_line(line,out_file)
   CHAR *line;
   SUDS_STREAM *out_file;
#endif
{
   INT4    i,j,got_string,len,move;
   STRING  codelist_name[20],strg[128];
   CHAR    *strptr;
   STRING  num_str[16];

   /* omit troff commands */
   if(line[0]=='.')  return;
   /* omit blank lines */
   if(line[0]=='\0') return;
   /* omit newline character */
   line[strlen(line)-1]='\0';

   /* if beginning of a new code_list */
   if(isdigit(line[0])) {
      /* output current code_list */
      if(cl.number_members>BEGIN) {
         if(cl.number_members==0 && cl.code_list_id !=AUTHORITIES)
            fprintf(stderr,
            "comp_cod: Must be at least one member for codelist %s\n",
            cl.code_list_name);
         else {
            /* output structure */
            len=((chr_ptr+8)/8)*8;
            for(;chr_ptr<=len;chr_ptr++) temp_list[chr_ptr]='\0';
            move=8*cl.number_members;
            cl.data_length=len+move;
            fprintf(stderr,"num_members=%2ld len=%ld length=%ld\n",
               cl.number_members,len,cl.data_length);
            for(i=len-1;i>=0;i--) temp_list[i+move]=temp_list[i];
            /*
            for(i=0;i<len;i++) {
               if(i>0 && i%50==0) fprintf(stderr,"\n");
               if(temp_list[i+move]=='\0')fprintf(stderr,"|");
               else fprintf(stderr,"%c",temp_list[i+move]);
            }
            fprintf(stderr,"\n");
            */
            offsets=(INT4 *)temp_list;
            for(i=0;i<2*cl.number_members;i++) {
               offsets[i]=number[i];
               if(i%2==1)offsets[i]+=move;
            }
            st_put((GENPTR)&cl,temp_list,out_file);
         }
         chr_ptr=0;
         cl.number_members=0;
         old_number=NODATL;
      }

      /* input id and name of new code_list */
      for(i=0;line[i]!=':' && line[i]!='\0';i++) num_str[i]=line[i];
      if(line[i]=='\0') fprintf(stderr,
         "comp_cod: missing : for line\n%s\n",line);
      num_str[i]='\0';
      cl.code_list_id=atoi(num_str);
      cl.list_number=cl.code_list_id;
      for(j=0,i++;!isspace(line[i]) && line[i]!='\0';i++,j++)
         cl.code_list_name[j]=line[i];
      cl.code_list_name[j]='\0';
      fprintf(stderr,"\n%3ld:%s\n",cl.code_list_id,cl.code_list_name);
   }  /* end of new code_list */

   /* read code value line */
   else if(isspace(line[0])) {
      if(cl.number_members<0) cl.number_members=0;
      got_string=0;
      number[2*cl.number_members]=-1;
      strg[0]='\0';
      for(i=0; line[i]!='\0';i++) {
         if(line[i]==' ' || line[i]=='\t') continue; 
         if(line[i]=='=') continue; 
         if(line[i]=='\'') {
            number[2*cl.number_members]=line[++i];
            i++;
         }
         if(isdigit(line[i]) || line[i]=='-') {
            for(j=1;line[i+j]!='=' && line[i+j]!='\0';j++);
            line[i+j]='\0';
            strptr=&line[i+j+1];
            number[2*cl.number_members]=atoi(&line[i]);
            i+=j;
         }
         if(line[i]=='"') {
            for(i++,j=0;line[i+j]!='"' && line[i+j]!='\0';j++)
               strg[j]=line[i+j];
            if(line[i+j]=='\0') fprintf(stderr,
               "comp_cod: double quote missing on line \n%s\n",strptr);
            strg[j]='\0';
            got_string=1;
            goto done;
         }
      }  /* end for */
done: if(number[2*cl.number_members]==BEGIN || got_string!=1)
         fprintf(stderr,
            "comp_cod: Incorrect format for this line\n(%s)\n",line);
      else {
         if(number[2*cl.number_members]==old_number) fprintf(stderr,
            "comp_com: code %ld in %s is duplicate on line\n%s\n",
            number[2*cl.number_members],codelist_name,line);
         if(!(number[2*cl.number_members]==0 && strg[0]=='\0')) {
            if(number[2*cl.number_members] <old_number) fprintf(stderr,
               "comp_com: code %d in %s out of order on line\n%s\n",
               number[2*cl.number_members],codelist_name,line);
         }
         old_number=number[2*cl.number_members];

         number[2*cl.number_members+1]=chr_ptr;
         /*fprintf(stderr,"\tnum=%6ld off=%3ld \"%s\"\n",
            number[2*cl.number_members],number[2*cl.number_members+1],strg);*/

         for(i=0;strg[i]!='\0';i++)
            temp_list[chr_ptr++]=strg[i];
         temp_list[chr_ptr++]='\0';
      }
      cl.number_members++;
   }  /* end if line[0] is digit */
}

#if defined (__STDC__) || defined (__cplusplus)
INTV main(INTV argc, CHAR **argv)
#else
INTV main(argc,argv)
   INTV argc;
   CHAR **argv;
#endif
{
   FILE *in_file,*addit;
   SUDS_STREAM *out_file;
   char line[512];
   INT4 i,ret,move,len;

   st_init(CODE_LISTS,(GENPTR)&cl);
   cl.code_list_dc=SUDS_INTERNAL;
   cl.data_type=CHR;
   cl.number_members=BEGIN;
   old_number=NODATL;
   chr_ptr=0;

   out_file=st_open(outfile,"wb");
   if(out_file==NULL) {
      fprintf(stderr,"Unable to open output file %s\n",outfile);
      exit(3);
   }
   for(i=1;i<argc;i++) {
      in_file=fopen(argv[i],"r");
      if(in_file==NULL) {
         fprintf(stderr,"Unable to open input file %s\n",infile);
         exit(1);
      }

      while(fgets(line,MAX_LINE,in_file)!=NULL) {
         /* omit whole-line comments */
         if(line[0]=='/') {
            while(fgets(line,MAX_LINE,in_file)!=NULL && line[0]!='*');
            if(fgets(line,MAX_LINE,in_file)==NULL) break;
         }
         if(strncmp(".so",line,3)==0) {
            for(i=4;i<strlen(line);i++) if(line[i]=='\n')line[i]='\0';
            addit=fopen(&line[4],"r");
            if(addit==NULL) {
               fprintf(stderr,"Unable to open \"%s\"\n",&line[4]);
               exit(1);
            }
            while(fgets(line,MAX_LINE,addit)!=NULL) {
               if(line[0]=='/') {
                  while(fgets(line,MAX_LINE,addit)!=NULL && line[0]!='*');
                  if(fgets(line,MAX_LINE,addit)==NULL) break;
               }
               process_line(line,out_file);
            }
            fclose(addit);
         }  /* end included file */
         else process_line(line,out_file);
     }  /* end while */
     fclose(in_file);
   }
   /* output last structure */
   len=((chr_ptr+8)/8)*8;
   for(;chr_ptr<=len;chr_ptr++) temp_list[chr_ptr]='\0';
   move=8*cl.number_members;
   cl.data_length=len+move;
   fprintf(stderr,"num_members=%2ld len=%ld length=%ld\n",
      cl.number_members,len,cl.data_length);
   for(i=len-1;i>=0;i--) temp_list[i+move]=temp_list[i];
   offsets=(INT4 *)temp_list;
   for(i=0;i<2*cl.number_members;i++) {
      offsets[i]=number[i];
      if(i%2==1)offsets[i]+=move;
   }
   st_put((GENPTR)&cl,temp_list,out_file);

   st_close(out_file);

   out_file=st_open(outfile,"rb");
   if(out_file==NULL) {
      fprintf(stderr,"Unable to open file %s for reading\n",outfile);
      exit(3);
   }
   ret=st_index(outfile,out_file);
   if(ret==SUCCESSFUL) st_save_idx(outfile,out_file);
   st_close(out_file);
   return(0);
}


