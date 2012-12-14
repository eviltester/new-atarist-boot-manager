#include "fileform.h"
#include "sets.h"
#include "other.h"

#define COMMENT			1
#define UNEXPECTED_END	2
#define BLOCK				3
#define OTHER				4
#define SET_BLOCK			5
#define AUTO_BLOCK		6	/* not needed */
#define ACCS_BLOCK		7	/* not needed */
#define OTHERS_BLOCK		8
#define END_BLOCK			9
#define WINDOW_BLOCK		10

char block_name[20];	/* global for block name */

int get_token( FILE *fp , char *ch )
{
	char read_ch;

	read_ch=fgetc(fp);
	*ch=read_ch;

/*printf("TOKEN 1\n");		
*/
	while((read_ch==' ' || read_ch=='\n') && read_ch!=EOF)
	{
		read_ch=fgetc(fp);
		*ch=read_ch;
	}
		
	if(read_ch==EOF)
	{
		/*printf("UNEXPECTED END\n");
*/		return UNEXPECTED_END;
	}

	switch(read_ch)
	{
		case '*':
/*printf("TOKEN 2\n");		
*/			return(COMMENT);
			break;
		case '%':
/*printf("TOKEN 3\n");		
*/			return(BLOCK);
			break;
		default:
/*printf("TOKEN 4\n");		
*/			return(OTHER);
			break;
	}
}
		
void skip_to_eol( FILE *fp)
{
	char ch;

	ch=fgetc(fp);
/*printf("SKIP 1\n");		
*/
	while(ch!='\n' && ch!=EOF)
		ch=fgetc(fp);
/*printf("SKIP 1\n");		
*/}

int get_block_type(FILE *fp)
{
/*	char block_name[20];*/
	int count;
	char ch;
	int ok;

	count=0;
	block_name[0]='\0';
	ok=1;
/*printf("BLOCK 1\n");		
*/
	while(ok)
	{

		ch=fgetc(fp);
/*printf("BLOCK 2\n");		
*/
		if(ch!=EOF && ch!='\n' && count<19)
		{
			if(ch!=' ')
			{
/*printf("BLOCK 3\n");		
*/				block_name[count]=ch;
				count++;
				block_name[count+1]='\0';
			}
		}
		else
			ok=0;
	}

	if(count==19)
	{
/*printf("BLOCK 4\n");		
*/		printf("WARNING: BLOCK NAME TOO BIG, %s  IGNORED\n",block_name);
		skip_to_eol(fp);
		return(0);
	}

	if(count==0)
	{
/*printf("BLOCK 5\n");		
*/		printf("WARNING: BLOCK NAME BLANK, BLOCK IGNORED\n");
		return(0);
	}

/*printf("BLOCK 6\n");		
*/
	if(!strcmp("SET",block_name))
		return(SET_BLOCK);

/*printf("BLOCK 7\n");		
*//* 	if(!strcmp("AUTO",block_name))
		return(AUTO_BLOCK);
*/
/*printf("BLOCK 8\n");		
*//*	if(!strcmp("ACCS",block_name))
		return(ACCS_BLOCK);
*/
/*printf("BLOCK 9\n");		
*/
	if(!strcmp("OTHERS",block_name))
		return(OTHERS_BLOCK);

/*printf("BLOCK 10\n");		
getchar();
*/	if(!strcmp("END",block_name))
		return(END_BLOCK);

/*	printf("WARNING: UNKNOWN BLOCK NAME %s\n",block_name);
	return(0);
*/
	return(WINDOW_BLOCK);
}

char *read_name(FILE *fp, char first_char)
{
	char line[301];
	char *return_char;
	int count=0;

	line[count]=first_char;
	line[count+1]='\0';

		first_char=fgetc(fp);
/*printf("READ NAME 1\n");		
*/
	while(!feof(fp) && first_char!='\n' && count<300)
	{
		count++;
		line[count]=first_char;
		line[count+1]='\0';
	
		first_char=fgetc(fp);
/*printf("READ NAME 2\n");		
	*/}

	if(count==300)
	{
/*printf("READ NAME 3\n");		
*/		skip_to_eol(fp);

		return NULL;
	}

/*printf("READ NAME 4\n");		
*/	return_char=(char *)malloc(strlen(line)+1);
	strcpy(return_char,line);
/*printf("READ NAME 5\n");		
*/
	return return_char;
}


LinkedList *load_sets( FILE *fp)
/*SET *load_sets( FILE *fp )*/
{
	int token;
	char ch;
	int current_block=0;
	int block_type;
	char *name;
	LinkedList /*SET*/ *first_set,*tl;
	SET *temp;
	SET *curr_set;
	SMALLFILE *sfp;
	WindowList *window_list;
	char *cp;

	first_set=NULL;
	curr_set=NULL;
	name=NULL;
	curr_other=NULL;
	window_list=NULL;

	while(!feof(fp))
	{
		token=get_token(fp, &ch);

/*printf("LOAD 1\n");		
*/		switch(token)
		{
			case COMMENT:
/*printf("LOAD 2\n");		
*/					skip_to_eol(fp);
					break;
			case BLOCK:
/*printf("LOAD 3\n");		
*/					block_type=get_block_type(fp);
					switch(block_type)
					{
						case 0:
							break;
						case SET_BLOCK:
							temp=create_set();

							if(temp==NULL)
								return(first_set);

/*							if(first_set==NULL)
							{
								first_set=temp;
								curr_set=temp;
							}
							else
							{
								curr_set=temp;
								first_set=append_set_to_list(temp,first_set);
							}
*/
							first_set=append_LinkedList(first_set,temp);
							curr_set=temp;
							current_block=block_type;
							break;
						case END_BLOCK:
							return(first_set);
							break;
	
						default:
/*printf("LOAD 4\n");		
*/							current_block=block_type;
							window_list=NULL;
							break;
					}
					break;
			case OTHER:
/*printf("LOAD 5\n");		
	*/			switch(current_block)
				{
				case SET_BLOCK:
/*printf("LOAD 6\n");		
*/						name=read_name(fp,ch);
						if(curr_set==NULL)
						printf("CURR SET NULL\n");
						if(name==NULL)
						printf("NAME NULL\n");
						strcpy(curr_set->set_name,name);
/*printf("LOAD 6.1\n");
*/						free(name);
						break;
/*				case AUTO_BLOCK:

						name=read_name(fp,ch);
						sfp=(SMALLFILE *)malloc(sizeof(SMALLFILE));
						if(sfp!=NULL)
						{
							sfp->left=NULL;
							sfp->right=NULL;
							strcpy(sfp->name,name);
						}
						free(name);
						curr_set->progs=append_smallfile(sfp,curr_set->progs);
						break;
				case ACCS_BLOCK:

						name=read_name(fp,ch);
						sfp=(SMALLFILE *)malloc(sizeof(SMALLFILE));
						if(sfp!=NULL)
						{
							sfp->left=NULL;
							sfp->right=NULL;
							strcpy(sfp->name,name);
						}
						free(name);
						curr_set->accs=append_smallfile(sfp,curr_set->accs);
						break;
*/
				case OTHERS_BLOCK:
/*printf("LOAD 9\n");		
*/
						if(window_list==NULL)
						{
							cp=(char *)malloc(strlen(block_name)+1);
							if(cp==NULL)
								return(first_set);
							strcpy(cp,block_name);

							window_list=(WindowList *)malloc(sizeof(WindowList));
							if(window_list==NULL)
								return(first_set);
							window_list->name=cp;
							window_list->stuff=NULL;
							curr_set->window_list=append_LinkedList(curr_set->window_list,window_list);
						}


						name=read_name(fp,ch);
						if(curr_other==NULL)
						{
							curr_other=create_other();
							if(curr_other)
							{
							curr_other->to=(char *)malloc(FILENAME_MAX+1);
							strcpy(curr_other->to,name);
							}
						}
						else
						{
							curr_other->from=(char *)malloc(FILENAME_MAX+1);
							strcpy(curr_other->from,name);
							window_list->stuff=append_LinkedList(window_list->stuff,curr_other);

/*							curr_set->others=append_other_to_list(curr_other,curr_set->others);*/	
							curr_other=NULL;
						}
																		
						free(name);
						break;
/*printf("LOAD 9\n");		
*//*						name=read_name(fp,ch);
						if(curr_other==NULL)
						{
							curr_other=create_other();
							if(curr_other)
							{
							curr_other->to=(char *)malloc(FILENAME_MAX+1);
							strcpy(curr_other->to,name);
							}
						}
						else
						{
							curr_other->from=(char *)malloc(FILENAME_MAX+1);
							strcpy(curr_other->from,name);
							curr_set->others=append_other_to_list(curr_other,curr_set->others);	
							curr_other=NULL;
						}
																		
						free(name);
						break;*/
				case WINDOW_BLOCK:

						if(window_list==NULL)
						{
							cp=(char *)malloc(strlen(block_name)+1);
							if(cp==NULL)
								return(first_set);
							strcpy(cp,block_name);

							window_list=(WindowList *)malloc(sizeof(WindowList));
							if(window_list==NULL)
								return(first_set);
							window_list->name=cp;
							window_list->stuff=NULL;
							curr_set->window_list=append_LinkedList(curr_set->window_list,(void *)window_list);
						}

						name=read_name(fp,ch);
						sfp=(SMALLFILE *)malloc(sizeof(SMALLFILE));
						strcpy(sfp->name,name);				
						window_list->stuff=append_LinkedList(window_list->stuff,sfp);
						free(name);

/*	printf("\nLIST IS ");
	tl=window_list->stuff;
	while(tl)
	{
		printf(" %s ",((SMALLFILE *)tl->node)->name);
		tl=tl->right;
	}
	printf("\n");*/

/*						sfp=(SMALLFILE *)malloc(sizeof(SMALLFILE));
						if(sfp!=NULL)
						{
							sfp->left=NULL;
							sfp->right=NULL;
							strcpy(sfp->name,name);
						}
						free(name);*/

/*						curr_set->accs=append_smallfile(sfp,curr_set->accs);*/
						break;

				default:
						printf("ERROR NO BLOCK\n");
						name=read_name(fp,ch);
						free(name);
						break;
					
				}
				break;
	
		}
	}
}

void save_sets( FILE *fp, LinkedList *sets )
{

	SET *first_set;

	LinkedList *window_contents;
	OTHER_FILES *ofp;
	LinkedList	*sets_windows;


	while(sets!=NULL)
	{
		/* write the set out */

		first_set=(SET *)sets->node;
		fprintf(fp,"%SET\n");
		fprintf(fp,"%s\n",first_set->set_name);

		sets_windows=(LinkedList *)sets->window_list;
		while(sets_windows!=NULL)
		{
			fprintf(fp,"%");
			fprintf(fp,"%s\n",sets_windows->name);

			/* find out what type of window this is*/

			/* if its a prog window then ....*/

			window_contents=sets_windows->stuff;
			while(window_contents!=NULL)
			{
				fprintf(fp,"    %s\n",((SMALLFILE *)window_contents->node)->name);
				window_contents=window_contents->right;
			}

			/* if its an other window then */
			/* insert other window processing details here */

			sets_windows=sets_windows->right;
		}				

		sets=sets->right;
	}
		
/*		sfp=first_set->progs;
		if(sfp)
			fprintf(fp,"%AUTO\n");
		while(sfp!=NULL)
		{
			fprintf(fp,"    %s\n",sfp->name);
			sfp=sfp->right;
		}

		sfp=first_set->accs;
		if(sfp)
			fprintf(fp,"%ACCS\n");
		while(sfp!=NULL)
		{
			fprintf(fp,"    %s\n",sfp->name);
			sfp=sfp->right;
		}

		ofp=first_set->others;
		if(ofp)
			fprintf(fp,"%OTHERS\n");
		while(ofp!=NULL)
		{
			fprintf(fp,"%s\n",ofp->to);
			fprintf(fp,"%s\n",ofp->from);
			ofp=ofp->right;
		}
*/

		first_set=first_set->right;

	}

		fprintf(fp,"%END\n");

}

/*void save_sets( FILE *fp )
{

	SET *first_set;
	SMALLFILE *sfp;
	OTHER_FILES *ofp;

	first_set=sets;

	while(first_set!=NULL)
	{
		/* write the set out */

		fprintf(fp,"%SET\n");
		fprintf(fp,"%s\n",first_set->set_name);

		sfp=first_set->progs;
		if(sfp)
			fprintf(fp,"%AUTO\n");
		while(sfp!=NULL)
		{
			fprintf(fp,"    %s\n",sfp->name);
			sfp=sfp->right;
		}

		sfp=first_set->accs;
		if(sfp)
			fprintf(fp,"%ACCS\n");
		while(sfp!=NULL)
		{
			fprintf(fp,"    %s\n",sfp->name);
			sfp=sfp->right;
		}

		ofp=first_set->others;
		if(ofp)
			fprintf(fp,"%OTHERS\n");
		while(ofp!=NULL)
		{
			fprintf(fp,"%s\n",ofp->to);
			fprintf(fp,"%s\n",ofp->from);
			ofp=ofp->right;
		}

		first_set=first_set->right;

	}

		fprintf(fp,"%END\n");

}
*/