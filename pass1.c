#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int si = 0,oi = 0;
struct optable
{
	char opc[10];
	char  hex[7];
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
			return 1;
		}
	}
		return 0;
}

int searchLabel(char a[],int hexa)
{
	int i = si;
	int found = 0;
	
	for(int j =0; j < i; j++)
	{
		if(strcmp(symtab[j].sym,a) == 0)
		{
			return 1;
		}
	}
	
	if(found == 0)
	{
		strcpy(symtab[i].sym,a);
		symtab[i].hex = hexa;
		si++;
		return 0;
	}
}

void readOp(FILE *fptr)
{
	oi = 0;
	while(!feof(fptr))
	{
		fscanf(fptr,"%s %s",optab[oi].opc,optab[oi].hex);
		oi++;	
	}
}

void readLine(FILE *fptr,char label[], char opcode[], char operand[])
{
	char line[50];
	strcpy(label,"");strcpy(opcode,"");strcpy(operand,"");

		if(fgets(line,sizeof(line),fptr))
		{
			int n = sscanf(line,"%s %s %s",label,opcode,operand);
			if(n == 1) { strcpy(opcode,label); strcpy(label,""); strcpy(operand,""); }
        	else if(n == 2) {strcpy(operand,opcode); strcpy(opcode,label);  strcpy(label,""); }
		}
		printf("%s %s %s\n",label,opcode,operand);
}

void main()
{
int loca = 0x0000;
	FILE * fptr1,*fptr2,*fptr3,*fptr4;

	char opcode[20],operand[20],label[20];

	fptr1 = fopen("input.txt","r");
	fptr2 = fopen("optab.txt","r");
	fptr3 = fopen("inter.txt","w");
	
	fptr4 = fopen("symtab.txt","w");

	readOp(fptr2);
	readLine(fptr1,label,opcode,operand);
	
	if(strcmp(opcode,"START")==0)
	{
		loca = strtol(operand,NULL,16);
		
	}
	else
	{
		loca = 0x0000;
	}
	fprintf(fptr3,"%5X\t%-10s %-10s %-10s\n",loca,label,opcode,operand);
	loca += 3;
	
	while(strcmp(opcode,"END"))
	{
		readLine(fptr1,label,opcode,operand);
		if(strcmp(opcode,"")==0)
		{
			continue;
		}
		fprintf(fptr3,"%5X\t%-10s %-10s %-10s\n",loca,label,opcode,operand);

		if(strcmp(label,"")!=0) searchLabel(label,loca);
		
		if(searchOpcode(opcode) == 1)
		{
			loca += 3;
		}
		else if(strcmp(opcode,"WORD") == 0)
		{
			loca = loca + 3;		
		}
		else if(strcmp(opcode,"RESW") == 0)
		{
			loca += atoi(operand) * 3;		
		}
		else if(strcmp(opcode,"RESB") == 0)
		{
			loca += atoi(operand);		
		}
		else if(strcmp(opcode,"BYTE")== 0)
		{
			if(operand[0] == 'C') loca += strlen(operand) - 3;
			else loca += (strlen(operand)-3)/2;
		}
	}

	for(int j = 0; j < si; j++)
	{
		fprintf(fptr4,"%-7s\t%-5X\n",symtab[j].sym,symtab[j].hex);
	}
	fclose(fptr1);
	fclose(fptr2);
	fclose(fptr3);
	fclose(fptr4);
}
