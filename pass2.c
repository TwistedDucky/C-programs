//I am positive that this code works only if my pass one is used so alter however neccesary.
//The code is not perfect so copy at your own risk.
//It may be noted that this code uses some unconventional methods for the implementation.

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int si = 0,oi = 0;
struct optable
{
	char opc[10];
	int hex;
}optab[20];

struct symtable
{
	char sym[10];
	int hex;
}symtab[20];

int searchOpcode(char a[])
{
	int i = oi;
	
	for(int j =0; j < i; j++)
	{
		if(strcmp(optab[j].opc,a) == 0)
		{
			return optab[j].hex;
		}
	}
		return 0;
}

int searchLabel(char a[])
{
	int i = si;

	for(int j =0; j < i; j++)
	{
		if(strcmp(symtab[j].sym,a) == 0)
		{
			return symtab[j].hex;
		}
	}

	return 0;
}

void findObj(char obj[],char opcode[], char operand[])
{
	strcpy(obj,"");
	if(strcmp(opcode,"RESB")==0 || strcmp(opcode,"RESW")==0)
	{
		return;
	}
	else if(strcmp(opcode,"BYTE")==0)
	{
		int length = strlen(operand);
		char buf[3];

		if(operand[0] =='c' || operand[0] == 'C')
			for(int i = 2; i < length-1; i++)
			{
				sprintf(buf,"%02X",(unsigned char)operand[i]);
				strcat(obj,buf);
			}
		else
			for(int i = 2; i < length-1; i++)
			{
				sprintf(buf,"%c",operand[i]);
				strcat(obj,buf);
			}
	}
	else if(strcmp(opcode,"WORD")==0)
	{
		int x = strtol(operand,NULL,10);
		sprintf(obj,"%06X",x);
	}
	else
	{	
		int found = 0;
		for(int i = 0; operand[i] != '\0'; i++)
		{
			if(operand[i] == ',') 
			{
			found = 1;
			break;
			}
		}
		
		int xx;

		if(found == 1)
		{
			char buffer[10]="";
			int j;
			for(j = 0; operand[j] != ',';j++)
			{
				buffer[j] = operand[j];
			}buffer[j] = '\0';

			xx = searchLabel(buffer);
			xx |= 0x8000;
		}
		else
		{
			xx = searchLabel(operand);
			xx &= 0x7FFF;
		}

		sprintf(obj,"%02X%04X",searchOpcode(opcode),xx);
	}
}

void readSym(FILE *fptr)
{
	while(!feof(fptr))
	{
		fscanf(fptr,"%s %x",symtab[si].sym,&symtab[si].hex);
		si++;	
	}
}

void readOp(FILE *fptr)
{
	while(!feof(fptr))
	{
		fscanf(fptr,"%s %x",optab[oi].opc,&optab[oi].hex);
		oi++;	
	}
}

void readLine(FILE *fptr,char locctr[],char label[], char opcode[], char operand[])
{
	char line[50];
	strcpy(label,"");strcpy(opcode,"");strcpy(operand,"");

		if(fgets(line,sizeof(line),fptr))
		{
			int n = sscanf(line,"%s %s %s %s",locctr,label,opcode,operand);
			if(n == 2) { strcpy(opcode,label); strcpy(label,""); strcpy(operand,""); }
        	else if(n == 3) {strcpy(operand,opcode); strcpy(opcode,label);  strcpy(label,""); }
		}
		printf("%s %s %s %s\n",locctr,label,opcode,operand);
}

void main()
{
int start = 0x0000;
	FILE * fptr1,*fptr2,*fptr3,*fptr4,*fptr5;

	char opcode[20],operand[20],label[20],locctr[10],obj[15],header[23],text[100],end[10];

	char hexlen[3];

	int length = 0x178D,objlen,remlen;

	char prglen[6];
	printf("Enter the length of the program(HEX): ");
	scanf("%s",prglen);
	length = strtol(prglen,NULL,16);
	
	fptr1 = fopen("inter.txt","r");
	fptr2 = fopen("optab.txt","r");
	fptr3 = fopen("list.txt","w");
	fptr4 = fopen("symtab.txt","r");
	fptr5 = fopen("obj.txt","w");

	readOp(fptr2);
	readSym(fptr4);

	readLine(fptr1,locctr,label,opcode,operand);
	
	if(strcmp(opcode,"START")==0)
	{
		fprintf(fptr3,"%5s\t%-10s %-10s %-10s\n",locctr,label,opcode,operand);
		start = strtol(operand,NULL,16);

		sprintf(header,"H^%s^%06X^%06X",label,start,length);//header record
	}
	else
	{
		findObj(obj,opcode,operand);
		fprintf(fptr3,"%5s\t%-10s %-10s %-10s %-10s\n",locctr,label,opcode,operand,obj);

		sprintf(header,"H^%s^%06X^%06X","000000",start,length);//header record
	}

	fprintf(fptr5,"%s\n",header);

	sprintf(text,"T^00%s^00",locctr);
	remlen = 60;

	sprintf(end,"E^%06X",start);//End record
	
	readLine(fptr1,locctr,label,opcode,operand);

	while(strcmp(opcode,"END"))
	{
		findObj(obj,opcode,operand);

		if(strcmp(obj,"") != 0)
		{
			if(( objlen = strlen(obj) ) > remlen)
			{
				sprintf(hexlen,"%02X",(60-remlen)/2);
				text[9] = hexlen[0];
				text[10] = hexlen[1];

				fprintf(fptr5,"%s\n",text);
				strcpy(text,"");
				sprintf(text,"T^00%s^00",locctr);
				remlen = 60 - objlen;
				strcat(text,"^");
				strcat(text,obj);
			}
			else
			{
				strcat(text,"^");
				strcat(text,obj);
				remlen -= objlen;
			}
		}

		fprintf(fptr3,"%5s\t%-10s %-10s %-10s %-10s\n",locctr,label,opcode,operand,obj);
		readLine(fptr1,locctr,label,opcode,operand);
	}
	fprintf(fptr3,"%5s\t%-10s %-10s %-6s%04X\n",locctr,"",opcode,"",start);

	if(remlen <=60)
	{
		sprintf(hexlen,"%02X",(60-remlen)/2);
				text[9] = hexlen[0];
				text[10] = hexlen[1];
		fprintf(fptr5,"%s\n",text);
	}

	fprintf(fptr5,"%s\n",end);

	fclose(fptr1);
	fclose(fptr2);
	fclose(fptr3);
	fclose(fptr4);
	fclose(fptr5);
}
