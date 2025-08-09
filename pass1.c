#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int si = 0,oi = 0,line=0;
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

struct loo
{
	char opc[10];
	char lab[10];
	char ope[10];
	int loc;
}tab[20];

int searchOpcode(char a[])
{
	int i = oi;
	int found = 0;
	
	for(int j =0; j < i; j++)
	{
		if(strcmp(optab[j].opc,a) == 0)
		{
			return 1;
		}
	}
	
	if(found == 0)
	{
		return 0;
	}
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
	}
}

void readOp(FILE *fptr)
{
	int index = 0;
	while(!feof(fptr))
	{
		fscanf(fptr,"%s %s",optab[index].opc,optab[index].hex);
		index++;	
	}
}

void readLine(FILE *fptr)
{
	
	int i = 0;
	
	while(!feof(fptr))
	{
		char c;
		char s1[10]="",s2[10]="",s3[10]="";
		fscanf(fptr,"%s",s1);
		if((c = fgetc(fptr)) != '\n')
		{
			fscanf(fptr,"%s",s2);
			if((c = fgetc(fptr)) != '\n')
			{
				fscanf(fptr,"%s",s3);
			}
		}
		
		if(strcmp(s1,"")!=0 && strcmp(s2,"")!=0 && strcmp(s3,"")!=0)
		{
			strcpy(tab[i].lab,s1);
			strcpy(tab[i].opc,s2);
			strcpy(tab[i].ope,s3);
		}
		else if(strcmp(s1,"")!=0 && strcmp(s2,"")!=0 && strcmp(s3,"")==0)
		{
			strcpy(tab[i].opc,s1);
			strcpy(tab[i].ope,s2);
		}
		else if(strcmp(s1,"")!=0 && strcmp(s2,"")==0 && strcmp(s3,"")==0)
		{
			strcpy(tab[i].opc,s1);
		}
		
		printf("%s %s %s\n",tab[i].lab,tab[i].opc,tab[i].ope);
		i++;oi++;line++;
	}
}

void main()
{
int loca = 0x0000;
	FILE * fptr1,*fptr2,*fptr3,*fptr4;
	fptr1 = fopen("input.txt","r");
	fptr2 = fopen("optab.txt","r");
	fptr3 = fopen("inter.txt","w");
	
	fptr4 = fopen("symtab.txt","w");
	readOp(fptr2);
	readLine(fptr1);
	
	if(strcmp(tab[0].opc,"START")==0)
	{
		loca = tab[0].loc = strtol(tab[0].ope,NULL,16);
		
	}
	else
	{
		loca = tab[0].loc = 0x0000;
	}
	fprintf(fptr3,"%5X\t%-10s %-10s %-10s\n",tab[0].loc,tab[0].lab,tab[0].opc,tab[0].ope);
	
	int i = 1;
	
	while(strcmp(tab[i].opc,"END"))
	{
		fprintf(fptr3,"%5X\t%-10s %-10s %-10s\n",loca,tab[i].lab,tab[i].opc,tab[i].ope);
	
		if(strcmp(tab[i].lab,"")!=0) searchLabel(tab[i].lab,loca);
		
		if(searchOpcode(tab[i].opc) == 1)
		{
			loca = loca+0x0003;
		}
		else if(strcmp(tab[i].opc,"WORD") == 0)
		{
			loca = loca + 3;		
		}
		else if(strcmp(tab[i].opc,"RESW") == 0)
		{
			loca += atoi(tab[i].ope) * 3;		
		}
		else if(strcmp(tab[i].opc,"RESB") == 0)
		{
			loca += atoi(tab[i].ope);		
		}
		else if(strcmp(tab[i].opc,"BYTE")== 0)
		{
			loca += strlen(tab[i].ope) - 3;	
		}

		i++;
	}
	fprintf(fptr3,"%5X\t%-10s %-10s %-10s\n",loca,tab[i].lab,tab[i].opc,tab[i].ope);

	for(int j = 0; j < si; j++)
	{
		fprintf(fptr4,"%-7s\t%-5X\n",symtab[j].sym,symtab[j].hex);
	}
}




























