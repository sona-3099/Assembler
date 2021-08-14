#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
int lc=0;
char stack[10][10];
int top=-1;
char reg[8][4]={"eax","ecx","edx","ebx","esp","ebp","esi","edi"};
char opc[11][5]={"add","sub","mul","mov","jmp","cmp","xor","inc","dec","and","or"};
struct symbtab
{
  char name[50],section[50],type[50],value[50],defined;
  int size,addr;
  struct symbtab *next;
};
struct symbtab *bucket[27];
void makeNullBuckets()
{
  for(int i=0;i<27;i++)
    {
      bucket[i]=NULL;
    }
}
void push(char *ele)
{
  strcpy(stack[++top],ele);
}
void pop()
{
  top--;
}
char *peek()
{
  return stack[top];
}
int isEmptyStack()
{
  if(top==-1)
    {
      return 1;
    }
  return 0;
}
int checkRegister(char *op)
{
  for(int i=0;i<8;i++)
    {
       if(strcmp(op,reg[i])==0)
	{
	  return 1;
	}
    }
  return 0;
}
int checkOpcode(char *op)
{
  for(int i=0;i<11;i++)
    {
      if(strcmp(op,opc[i])==0)
	{
	  return 1;
	}
    }
  return 0;
}
int hash(char c)
{
  return (c-97);
}
struct symbtab *getNewNode(char *symb,char *section,int addr,char *type,char *value,int size,char def)
{
  struct symbtab *new=(struct symbtab *)malloc(sizeof(struct symbtab));
  new->next=NULL;
  strcpy(new->name,symb);
  strcpy(new->section,section);
  new->addr=addr;
  strcpy(new->type,type);
  strcpy(new->value,value);
  new->size=size;
  new->defined=def;
  //  printf("Returning newNode\n");
  return new;
  
  
}
void updateSymbol(char *symb,int addr,char def)
{
  struct symbtab *temp;
  int index=hash(symb[0]);
  for(temp=bucket[index];temp!=NULL;temp=temp->next)
    {
      if(strcmp(temp->name,symb)==0)
	{
	  temp->addr=addr;
	  temp->defined=def;
	}
    }
}
int checkSymbol(char *symb)
{
  struct symbtab *temp;
  int index=hash(symb[0]);
  // printf("in checkSymbol\n");
  for(temp=bucket[index];temp!=NULL;temp=temp->next)
    {
      if(strcmp(temp->name,symb)==0)
	{
	  return 1;
	}
    }
  // printf("returning checkSymbol\n");
  return 0;
}
void insert(char *symb,char *section,int addr,char *type,char *value,int size,char def)
{
  struct symbtab *temp;
  int index=hash(symb[0]);
  // printf("Index:%d\n",index);
  if(checkSymbol(symb)==1)
    {
      updateSymbol(symb,addr,def);
    }
  else
    {
      if(bucket[index]==NULL)
	{
	  //	  printf("in case 1\n");
	  bucket[index]=getNewNode(symb,section,addr,type,value,size,def);
	}
      else
	{
	  temp=bucket[index];
	  bucket[index]=getNewNode(symb,section,addr,type,value,size,def);
	  bucket[index]->next=temp;
	}
    }
    
}

char *getType(char *word)
{
  if(strcmp("db",word)==0)
    {
      // printf("return fromtype\n");
           return "byte";
    }
  else if(strcmp("dw",word)==0)
    {
      //printf("return fromtype\n");
      return "word";
    }
  else if(strcmp("dd",word)==0)
    {
      return "dword";
    }
   else if(strcmp("resb",word)==0)
    {
      return "byte";
    }
   else if(strcmp("resw",word)==0)
    {
      return "word";
    }
   else if(strcmp("resd",word)==0)
    {
      return "dword";
    }
}
int getSizeBSS(char *word1,char *word2)
{
  int size;
  // printf("atoi:%d %s\n",atoi(word2),word1);
  if(strcmp(getType(word1),"byte")==0)
    {
      size=1*(atoi(word2));
      //printf("size:%d\n",size);
    }
  else if(strcmp(getType(word1),"word")==0)
    {
      size=(2*atoi(word2));
    }
  else if(strcmp(getType(word1),"dword")==0)
    {
      size=4*(atoi(word2));
    }
    return size;
  
}
int getSize(char *word1,char *word2)
{
  int i,size=0;
  for(i=0;word2[i]!='\0';i++)
    {
      if(word2[i]==',')
    {
      continue;
    }
      else if(strcmp(getType(word1),"byte")==0)
      {
	size=size+1;
      }
    else if(strcmp(getType(word1),"word")==0)
      {
	size=size+2;
      }
    else if(strcmp(getType(word1),"dword")==0)
      {
	size=size+4;
      }
    
    }
return (size/2);
}
void printBucket(struct symbtab *head)
{
  struct symbtab *temp;
  for(temp=head;temp!=NULL;temp=temp->next)
    {
      printf("%s\t%d\t%s\t%s\t%s\t%d\t%c\n",temp->name,temp->addr,temp->section,temp->type,temp->value,temp->size,temp->defined);
    }
}
void main()
{ 
  FILE *fp=NULL,*fp2=NULL;
  char line[100],fileName[50],word1[50],word2[50],word3[50],word4[50],*bin;
  int i,j,k,w1,w2,w3,w4,f1,f2,f3,f4,f11;
  makeNullBuckets();
  printf("Enter file name containing assembly code: ");
  scanf("%s",&fileName);
  fp=fopen(fileName,"r");
  if(fp==NULL)
    {
      printf("Unable to open file!!");
      exit(0);
    }
  while(fgets(line,100,fp)!=NULL)
    {
      w1=w2=w3=w4=0;
      f1=f2=f3=f4=f11=0;
      for(i=0;line[i]!='\n';i++)
	{
	  if(line[i]==9)
	    {
	      continue;
	    }
	 else if(line[i]=='.')
	    {
	      continue;
	    }
	 else if(line[i]=='[')
	    {
	      j=i+1;
	      while(line[j]!=']')
		{
		  j++;
		}
	      i=j;
	    }
	 else if(line[i]!=' '&&line[i]!=','&&line[i]!=':')
	   {
	     if(f1==0)
	       {
		 word1[w1++]=line[i];
	       }
	     else if(f1==1&&f2==0)
	       {
		 word2[w2++]=line[i];
	       }
	     else if(f1==1&&f2==1&&f3==0)
	       {
		 word3[w3++]=line[i];
	       }
	     else if(f1==1&&f2==1&&f3==1&&f4==0)
	       {
		 word4[w4++]=line[i];
	       }
	   }
	 else
	   {
	     if(line[i]==' '||line[i]==':')
	       {
		 if(line[i]==':')
		   {
		     word1[w1]='\0';
		     f1=f11=1;
		   }
		 else if(w2==0)
		   {
		     word1[w1]='\0';
		     f1=1;
		   }
		 else
		   {
		     word2[w2]='\0';
		     f2=1;
		   }
	       }
	     if(line[i]==',')
	       {
		 if(w3==0)
		   {
		     word2[w2]='\0';
		     f2=1;
		   }
		 else
		   {
		     if(strcmp(peek(),"data")==0)
		       {
			 word3[w3++]=line[i];
		       }
		     else
		       {
		     word3[w3]='\0';
		     f3=1;
		       }
		   }
	       }
	   }
	   
        }
      if(w1==0)
	{
	  continue;
	}
      else if(f1==1&&f2==0)
	{
	  if(w2!=0)
	    {
	      word2[w2]='\0';
	      f2=1;
	    }
	}
      else if(f1==1&&f2==1&&f3==0)
	{
	  if(w3!=0)
	    {
	      word3[w3]='\0';
	      f3=1;
	    }
	}
      else
	{
	  if(w4!=0)
	    {
	      word4[w4]='\0';
	    }
	}
      /*if(w1!=0)
	{
	  printf("%s1 ",word1);
	}
      if(w2!=0)
	{
	  printf("%s2 ",word2);
	}
      if(w3!=0)
	{
	  printf("%s3 ",word3);
	}
      if(w4!=0)
	{
	  printf("%s4 ",word4);
	}
	printf("\n");*/
      if(strcmp(word1,"section")==0)
	{
	  if(isEmptyStack()==0)
	    {
	      lc=0;
	      pop();
	      push(word2);
	      continue;
	    }
	  else
	    {
	      lc=0;
	      push(word2);
	      continue;
	    }
	  
	}
      if(strcmp(peek(),"data")==0)
	{
	  insert(word1,peek(),lc,getType(word2),word3,getSize(word2,word3),'D');
	  //  printf("inlast\n");
	  //printf("abc %s\n",getType(word2));
	  //	  printf("%s %s  %d %s %s %d D\n",word1,peek(),lc,getType(word2),word3,getSize(word2,word3));
	    lc=lc+getSize(word2,word3);
	  
	}
      else if(strcmp(peek(),"bss")==0)
	{
	   insert(word1,peek(),lc,getType(word2),"NA",getSizeBSS(word2,word3),'D');
	   //printf("%s %s %d  %s 0  size=%d D\n",word1,peek(),lc,getType(word2),getSizeBSS(word2,word3));
			  lc=lc+getSizeBSS(word2,word3);

	}
      else if(strcmp(peek(),"text")==0&&(strcmp(word1,"global")!=0))
	{
	  if(f11==1)
	    {
	      insert(word1,peek(),-1,"label","NA",0,'D');
	    }
	  else
	    {
	      if(w1!=0)
		{
		  if(checkOpcode(word1)==0&&checkRegister(word1)==0&&!isdigit(word1[0]))
		    {
		      insert(word1,peek(),-1,"label","NA",0,'U');
		    }
		}
		  else if(w2!=0)
		{
		  if(checkOpcode(word2)==0&&checkRegister(word2)==0&&!isdigit(word2[0]))
		    {
		      insert(word2,peek(),-1,"label","NA",0,'U');
		    }
		}
		  else if(w3!=0)
		{
		  if(checkOpcode(word3)==0&&checkRegister(word3)==0&&!isdigit(word3[0]))
		    {
		      insert(word3,peek(),-1,"label","NA",0,'U');
		    }
		}
	    }
	    
	  
	}
    }
  printf("Symbol Table is:\nName Address\tSection\tType\tValue \tsize defined/ND\n");
  for(i=0;i<27;i++)
    {
      printBucket(bucket[i]);
    }
  printf("\n");
   
}
