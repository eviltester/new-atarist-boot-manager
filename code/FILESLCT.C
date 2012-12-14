#include "fileslct.h"

char *drive_paths_array[32]={	NULL,NULL,NULL,NULL,NULL,
										NULL,NULL,NULL,NULL,NULL,
										NULL,NULL,NULL,NULL,NULL,		
										NULL,NULL,NULL,NULL,NULL,
										NULL,NULL,NULL,NULL,NULL,
										NULL,NULL,NULL,NULL,NULL,
										NULL,NULL
									};

/* things printable in display */
#define THE_NAME	0x01
#define THE_DATE	0x02
#define THE_TIME	0x04
#define THE_LENGTH	0x08

typedef struct{
					char name[15];
					short attributes;
					unsigned int date;
					unsigned int time;
					long		length;
					short in_list;
					short drive;
					}file_selector_file;


int as_much_as_poss_path( char *cp)
{
	short found_drive;
	int loop;
	long mounted_drives;
	char *other_cp,stored_char,*first_char;
	struct FILEINFO dta;

	first_char=cp;

	found_drive=0;
	mounted_drives=Drvmap();

	for(loop=0; loop<32; loop++)
	{
		if(mounted_drives&1 && (loop+'A')==*cp)	/*check a bit */
		{
			/* we have drive loop+'A' */
		
			found_drive=1;
		}
		mounted_drives=mounted_drives>>1;
	}

	if(found_drive==0)
		return 0;		/* bum path */

	cp++;
	if(*cp==':')
		cp++;
	else
		strcpy(cp,":\\");

	if(*cp=='\\')
		cp++;
	else
		strcpy(cp,"\\");

		Fsetdta(&dta);

check_path:
	other_cp=cp;
	while(*cp!='\\' && *cp!='\0')
		cp++;

	if(*cp=='\0')
	{

		if( !Fsfirst(first_char,0x10))
		{
	
				if(!(dta.attr&0x10))	/* is it a file */
				{
					 return 2;	/* to say that it is a file */
				}
				else
				{
					return 1;	/* to say its a valid directory path */
				}
		}
		else
		{
				/* it was valid up until this point so cut it here */
				*other_cp='\0';
				return -1;	/* we had to cut to get it valid */
		}
		

	}
	else
	{

		if( !Fsfirst(first_char,0x10))
		{
				if(dta.attr&0x10)	/* is it a directory */
				{
					cp++;	/* get next char */
					goto check_path;
				}
				else
				{
					/* major bummer, a non terminating file */
					/*	cut it and finish */
					*other_cp='\0';
					return -1;	/* we had to cut to get it valid */
				}
					
		}
		else
		{
				/* it was valid up until this point so cut it here */
				*other_cp='\0';
				return -1;	/* we had to cut to get it valid */
		}

	}

}



void print_header(int screen_width, char *line, char *purpose, char *curr_path, char *file_mask)
{
	int chars_left;
	char *ppath;
	int loop;

	DISCARD_EOL
	centre_text(purpose,line,screen_width,' ');
	AT(0,0)
	REVERSE_VIDEO
	printf("%s",line);
	NORMAL_VIDEO

	AT(0,0)
	REVERSE_VIDEO
	printf("Drive: %c",curr_path[0]);
	NORMAL_VIDEO

	AT(1,0)
	REVERSE_VIDEO
	sprintf(line,"Mask: %s   , Path: ",file_mask);

	chars_left=screen_width-strlen(line);

	if(chars_left>strlen(curr_path))
		strcat(line,curr_path);
	else
	{
		ppath=curr_path;
		for(loop=strlen(curr_path)-chars_left;loop<0;loop++)
			ppath++;
		ppath+=2;
		strcat(line,"..");
		strcat(line,ppath);
	}

	for(loop=strlen(line);loop<screen_width;loop++)
		line[loop]=' ';

	printf("%s",line);
	NORMAL_VIDEO
	WRAP_EOL
} 

void print_file_selector_bottom( char *line, int screen_width, int screen_height)
{
	DISCARD_EOL
	centre_text("v^<> , SPC/RTN select, C change drive",line,screen_width,' ');
	AT(screen_height-2,0)
	REVERSE_VIDEO
	printf("%s",line);
	NORMAL_VIDEO

	centre_text("ESC cancel, M change file mask, HELP help",line,screen_width,' ');
	AT(screen_height-1,0)
	REVERSE_VIDEO
	printf("%s",line);
	NORMAL_VIDEO
	WRAP_EOL
}

void strip_last_dir( char *path)
{
	char *cp;

	cp=path;
	while(*cp!='\0')
		cp++;

	while(*cp!='\\')
		cp--;

	cp--;

	while(*cp!='\\')
		cp--;

	cp++;
	*cp='\0';
}

int valid_extension_char(char ch)
{
	if(isupper(ch) || islower(ch) || isdigit(ch) || ch=='!' || ch=='@' ||
		ch=='#' || ch=='$' || ch=='%' || ch=='^' || ch=='&' || ch=='('
		|| ch==')' || ch=='+' || ch=='-' || ch=='=' || ch=='~' || ch=='`'
		|| ch=='"' || ch==',' || ch=='<' || ch=='>' || ch=='|' || ch=='['
		|| ch==']' || ch=='!' || ch=='_' || ch=='*' || ch=='?')
		return 1;
	else
		return 0;                   
}

int file_selector( int screen_width, int screen_height, 
							char *curr_drive, char *purpose, char *return_file_path)
{

	char emergency_path[FILENAME_MAX+1];
	char file_mask[15];
	char *curr_path,*temp_path;
	char *ppath;
	unsigned long mounted_drives;
	int loop;
	char *line;
	LinkedList *llp,*search_llp;
	file_selector_file *flp;
	int cols, col_width, logical_cursor, physical_cursor, first_entry;
	int chars_left;
	long k;
	int u,l,s,shift,stopped;
	int total_entries;
	int max_num_on_screen;
	short found;
	char ext[4];
		
	mounted_drives=Drvmap();


	for(loop=0; loop<32; loop++)
	{
		if(mounted_drives&1 && (loop+'A')==curr_drive[0])	/*check a bit */
		{
			/* we have drive loop+'A' */
		
			if(drive_paths_array[loop]==NULL)
			{
/*				temp_path=(char *)malloc(FILENAME_MAX+1);
				drive_paths_array[loop]=temp_path;
				if(temp_path!=NULL)
					sprintf(temp_path,"%c:\\",loop+'A');

				if(loop+'A'==curr_drive[0])
				{
					if(temp_path!=NULL)
					{
						curr_path=temp_path;
						printf("setting curr_path to temp %s\n",curr_path);
					}
					else
					{
						curr_path=drive_paths_array[0];
						printf("setting curr_path to first %s\n",curr_path);
					}
				}*/
				curr_path=emergency_path;
				sprintf(emergency_path,"%c:\\",curr_drive[0]);
				
			}
			else
			{
				curr_path=emergency_path;
				strcpy(curr_path,drive_paths_array[loop]);
			}
		}

		mounted_drives=mounted_drives>>1;

	}


/*	if(curr_path==NULL)
	{
		curr_path=emergency_path;
		strcpy(emergency_path,curr_drive);
	}			
*/

	strcpy(ext,"*");	      
	strcpy(file_mask,"*.*");

	CLEAR_SCREEN
	DISCARD_EOL

	AT(0,0)
/*	line=(char *)malloc(screen_width+2);*/
	line=line_buffer;


	print_header(screen_width, line, purpose, curr_path, file_mask);

	llp=read_files(file_mask,curr_path,0x10,NULL,&total_entries);


	llp=sort_files_by_type(llp);


	print_file_selector_bottom( line, screen_width, screen_height);

	
	col_calculation(	screen_width, THE_NAME, 
							 &col_width, &cols);

	max_num_on_screen=(screen_height-4)*cols;

	logical_cursor=1;
	physical_cursor=1;
	first_entry=1;

	display_files( 	first_entry, THE_NAME,
							cols, col_width, llp,
							screen_height);
	
	draw_flst_cursor(cols,col_width,physical_cursor);

	stopped=0;

	while(stopped==0)
	{
		while(!Bconstat(2));

		k=Bconin(2);
		shift=Kbshift(-1);
		shift=shift&0x0F;
		u=(int)(k>>16)&0xFF;
		l=(int)(k%256);
		s=(int)(k>>24)&0x0F;	/* we are only interested in Shift, Cntrl and Alt see page 306 for details */

		switch(u)
		{
		case 0x1C:	/* return */
		case 0x39:	/* space */
					search_llp=llp;
					for(loop=1;loop<logical_cursor;loop++)
						search_llp=search_llp->right;
					
					flp=(file_selector_file *)search_llp->node;
					
					if(flp->attributes&0x10)	/* its a directory */
					{
						if(flp->name[0]!='.')
						{
							strcat(curr_path,flp->name);
							strcat(curr_path,"\\");
						}
						else
						{
							if(flp->name[1]=='.')
								strip_last_dir(curr_path);
						}
							remove_flst_cursor(cols,col_width,physical_cursor);

							llp=read_files(file_mask,curr_path,0x10,llp,&total_entries);
							llp=sort_files_by_type(llp);
							print_header(screen_width, line, purpose, curr_path, file_mask);
							logical_cursor=1;
							physical_cursor=1;
							first_entry=1;
	
							display_files( 	first_entry, THE_NAME,
													cols, col_width, llp,
													screen_height);
		
							draw_flst_cursor(cols,col_width,physical_cursor);
						
					}
					else
					{
						/* save path */
							for(loop=0; loop<32; loop++)
							{
								if(mounted_drives&1 && (loop+'A')==curr_path[0])	/*check a bit */
								{
								/* we have drive loop+'A' */
		
									if(drive_paths_array[loop]==NULL)
									{
										temp_path=(char *)malloc(FILENAME_MAX+1);
										drive_paths_array[loop]=temp_path;
										if(temp_path!=NULL)
											strcpy(temp_path,curr_path);				
									}
									else
									{
										strcpy(drive_paths_array[loop],curr_path);
									}
		
									mounted_drives=mounted_drives>>1;
								}
							}
	
							/* tag on the filename */
							strcat(curr_path,flp->name);
							strcpy(return_file_path,curr_path);
							return 1;
					}
					break;

		case 0x32:	/* M */

						edit_at(1, 8, 3, 4, ext, ' ', 1, valid_extension_char, 1, screen_height);
						strcpy(file_mask,"*.");
						strcat(file_mask,ext);
						remove_flst_cursor(cols,col_width,physical_cursor);
						llp=read_files(file_mask,curr_path,0x10,llp,&total_entries);
						llp=sort_files_by_type(llp);
						print_header(screen_width, line, purpose, curr_path, file_mask);
						display_files( 	first_entry, THE_NAME,
												cols, col_width, llp,
												screen_height);
		
						draw_flst_cursor(cols,col_width,physical_cursor);
						print_file_selector_bottom( line, screen_width, screen_height);
						break;						

		case 0x2E:	/* C */
					DISCARD_EOL
					AT(screen_height-2,0)
					REVERSE_VIDEO
					centre_text("   ",line,screen_width,' ');
					printf("%s",line);
					NORMAL_VIDEO
					AT(screen_height-2,0)
					REVERSE_VIDEO
					printf("WHICH DRIVE: ");
					mounted_drives=Drvmap();
					chars_left=0;
					for(loop=0; loop<32; loop++)
					{
						if(mounted_drives&1)	/*check a bit */
						{
							/* we have drive loop+'A' */
							if(chars_left==0)
							{
								printf("%c",loop+'A');
								chars_left=1;
							}
							else
								printf(",%c",loop+'A');
						}
						mounted_drives=mounted_drives>>1;
					}
					NORMAL_VIDEO

					AT(screen_height-1,0)
					REVERSE_VIDEO
					centre_text("Press a valid drive key or ESC to cancel",line,screen_width,' ');
					printf("%s",line);
					NORMAL_VIDEO
				

want_a_valid_drive_key:
					while(!Bconstat(2));
					k=Bconin(2);
					u=(int)(k>>16)&0xFF;
					l=(int)(k%256);
					s=(int)(k>>24)&0x0F;	/* we are only interested in Shift, Cntrl and Alt see page 306 for details */
							

					/* check for valid key */
					mounted_drives=Drvmap();
					found=0;

					for(loop=0; loop<32 && found==0; loop++)
					{
						if(mounted_drives&1 && loop==(toupper(l)-'A'))	/*check a bit */
						{
							found=1;
						}
						mounted_drives=mounted_drives>>1;
					}

					if(found==1)
					{

						if(toupper(l)!=toupper(curr_path[0]))
						{
							/*change path*/
							/*should save old path into its slot */

							for(loop=0; loop<32; loop++)
							{
								if(mounted_drives&1 && (loop+'A')==curr_path[0])	/*check a bit */
								{
								/* we have drive loop+'A' */
		
									if(drive_paths_array[loop]==NULL)
									{
										temp_path=(char *)malloc(FILENAME_MAX+1);
										drive_paths_array[loop]=temp_path;
										if(temp_path!=NULL)
											strcpy(temp_path,curr_path);				
									}
									else
									{
										strcpy(drive_paths_array[loop],curr_path);
									}
		
									mounted_drives=mounted_drives>>1;
								}
							}
							curr_path=drive_paths_array[toupper(l)-'A'];
							if(curr_path==NULL)
							{
									curr_path=emergency_path;
								sprintf(curr_path,"%c:\\",toupper(l));
							}
	
							llp=read_files(file_mask,curr_path,0x10,llp,&total_entries);
							llp=sort_files_by_type(llp);
							print_header(screen_width, line, purpose, curr_path, file_mask);
							logical_cursor=1;
							physical_cursor=1;
							first_entry=1;
	
							display_files( 	first_entry, THE_NAME,
													cols, col_width, llp,
													screen_height);
		
							draw_flst_cursor(cols,col_width,physical_cursor);
	
						}
					}
					else
					{
						if(u!=0x01)
						{
							BELL
							goto want_a_valid_drive_key;
						}
					}
					print_file_selector_bottom( line, screen_width, screen_height);
					break;
		case 0x48:		/* up arrow */
					
						if(logical_cursor-cols>=1)
						{
							remove_flst_cursor(cols,col_width,physical_cursor);
							logical_cursor-=cols;
							physical_cursor-=cols;

							if(logical_cursor<first_entry)
							{
								first_entry=logical_cursor-max_num_on_screen;
								if(first_entry<1)
									first_entry=1;

								/*logical_cursor=first_entry+max_num_on_screen-1;*/
								physical_cursor=logical_cursor-first_entry+1;

								display_files( 	first_entry, THE_NAME,
														cols, col_width, llp,
														screen_height);
							}
							draw_flst_cursor(cols,col_width,physical_cursor);					
						}
					break;
		case 0x50:		/* down_arrow */
						if(logical_cursor+cols<=total_entries)
						{
							remove_flst_cursor(cols,col_width,physical_cursor);
							logical_cursor+=cols;
							physical_cursor+=cols;

							if(logical_cursor-first_entry>=max_num_on_screen)
							{
								first_entry=first_entry+max_num_on_screen;
								physical_cursor=logical_cursor-first_entry+1;

								display_files( 	first_entry, THE_NAME,
														cols, col_width, llp,
														screen_height);
							}
							draw_flst_cursor(cols,col_width,physical_cursor);					
						}
					break;
		case 0x4B:		/* left arrow */
						if(logical_cursor>1)
						{
							remove_flst_cursor(cols,col_width,physical_cursor);
							logical_cursor--;
							physical_cursor--;

							if(logical_cursor<first_entry)
							{


								first_entry=logical_cursor-max_num_on_screen;
								if(first_entry<1)
									first_entry=1;

								physical_cursor=logical_cursor-first_entry+1;

								display_files( 	first_entry, THE_NAME,
														cols, col_width, llp,
														screen_height);
							}
							draw_flst_cursor(cols,col_width,physical_cursor);					
						}
						break;
		case 0x4D:		/* right arrow */
						if(logical_cursor<total_entries)
						{
							remove_flst_cursor(cols,col_width,physical_cursor);
							logical_cursor++;
							physical_cursor++;

							if(logical_cursor-first_entry>=max_num_on_screen)
							{
								physical_cursor=1;
								first_entry=logical_cursor;
								display_files( 	first_entry, THE_NAME,
														cols, col_width, llp,
														screen_height);
							}
							draw_flst_cursor(cols,col_width,physical_cursor);					
						}
						break;

		case 0x01:		/* ESC */
					s=Kbshift(-1);
					s&=0x0F;

					if(s&0x01 || s&0x02 || s&0x03)
						stopped=2;
					else
						stopped=1;

					break;
		}
	}

	WRAP_EOL

	if(stopped==1)
	{
		AT(0,0)
		printf("returning %s",curr_path);
		strcpy(return_file_path,curr_path);
		return 1;
	}
	else
		return 0;

}


void col_calculation( int screen_width, short things_to_print, 
							 int *col_width, int *cols)
{

	*col_width=0;

	*col_width+=1;

	if(things_to_print&THE_NAME)
		*col_width+=15;

	*cols=screen_width/(*col_width);

	if( (( (*cols)*(*col_width))+1)>screen_width)
		(*cols)--;
}

void display_files( 	int first_entry, short things_to_print,
							int cols, int col_width, LinkedList *files,
							int screen_height)
{
	int loop, col_loop;
	LinkedList *llp;

	loop=1;
	llp=files;

	while(loop<first_entry && llp!=NULL)
	{
		loop++;
		llp=llp->right;
	}

	AT(2,0)
	DISCARD_EOL

	for(loop=2;loop<screen_height-2;loop++)
	{
		AT(loop,0)

/*		Bconout(2,27);
		Bconout(2,'K');*/

		CLEAR_EOL
		for(col_loop=0;col_loop<cols;col_loop++)
		{
			if(llp && ((file_selector_file *)llp->node)->in_list==1)
			{
				AT(loop,col_width*col_loop)
				if(((file_selector_file *)llp->node)->attributes&0x10)
					printf(" [] ");
				else
					printf("    ");
				printf("%s\n",((file_selector_file *)llp->node)->name);
				llp=llp->right;
			}
		}
	}

	WRAP_EOL


}

LinkedList *read_files(char *file_mask, char *curr_path, short attr, LinkedList *files, int *total_entries)
{
	file_selector_file *fl;
	file_selector_file *last_fl;
	LinkedList *head;
	LinkedList *llp;
/*	FILELIST *last;*/
	struct FILEINFO dta;
	char *filenamep1,*filenamep2;
	int path_len;
	char *path;
	int append_it;
	char filename_buffer[128+1];

	*total_entries=0;
	head=files;
	fl=NULL;
	last_fl=NULL;
	llp=head;
	
	if(llp!=NULL)
		fl=llp->node;

/*	last=NULL;*/

/*	path_len=strlen(curr_path)+strlen(file_mask)+1;
	path=(char *)malloc(path_len);
	if(path_len==NULL)
	{
		AT(0,0)
		printf("MEMORY ERROR\n");
		return head;
	}
*/
	path=filename_buffer;

	/* FIRST READ ALL THE DIRECTORIES */
	sprintf(path,curr_path);
	strcat(path,"*.*");


	Fsetdta(&dta);
	if( !Fsfirst(path,attr))
	{
		do
		{
	
		if(dta.attr&0x10)	/* is it a directory */
		  {		
			append_it=0;

			fl=NULL;

			if(llp!=NULL)
				fl=llp->node;

			if(fl==NULL)
			{
				append_it=1;
				fl=(file_selector_file *)malloc(sizeof(file_selector_file));
			}

				if(fl==NULL)
				{
					printf("ERROR: not enough memory to build file list \n");
					exit(1);
				}


				fl->attributes=0;
				fl->date=0;
				fl->time=0;
				fl->length=0;
				fl->in_list=0;
				fl->drive=0;
				fl->in_list=1;

				strcpy(fl->name,dta.name);
				fl->attributes=dta.attr;

				if(append_it)
				{			
					head=append_LinkedList( head, fl);
					append_it=0;
					last_fl=fl;
					fl=NULL;
				}
				else
				{
					llp=llp->right;
				}
				(*total_entries)++;
		  }
		}while(!Fsnext());
	}

	/* NOW READ THE FILES */
	sprintf(path,curr_path);
	strcat(path,file_mask);



	Fsetdta(&dta);
	if( !Fsfirst(path,attr))
	{
		do
		{
	
		if(!(dta.attr&0x10))	/* not directory */
		  {
			append_it=0;

			fl=NULL;

			if(llp!=NULL)
				fl=llp->node;

			if(fl==NULL)
			{
				append_it=1;
				fl=(file_selector_file *)malloc(sizeof(file_selector_file));
			}

				if(fl==NULL)
				{
					printf("ERROR: not enough memory to build file list \n");
					exit(1);
				}


				fl->attributes=0;
				fl->date=0;
				fl->time=0;
				fl->length=0;
				fl->in_list=0;
				fl->drive=0;
				fl->in_list=1;

				strcpy(fl->name,dta.name);
				fl->attributes=dta.attr;

				if(append_it)
				{			
					head=append_LinkedList( head, fl);
					append_it=0;
					last_fl=fl;
					fl=NULL;
				}
				else
				{
					llp=llp->right;
				}
				(*total_entries)++;

		  }
		}while(!Fsnext());
	}

	while(llp!=NULL)
	{
		((file_selector_file *)llp->node)->in_list=0;
		llp=llp->right;
	}

/*	free(path);*/
	return(head);
}

void sort_files_by_name(LinkedList *files)
{
}

LinkedList *sort_files_by_type(LinkedList *files)
{
	LinkedList *llp;
	LinkedList *llp2;
	LinkedList *head;
	int stopped;

	stopped=0;
	llp=files;
	head=files;

	/* find the first non directory */
		/* look for a directory after this and move the directory before this */
		/* if no directory found then quit */

sort_loop:

	while(llp!=NULL && ((file_selector_file *)llp->node)->in_list==1 && 
			((file_selector_file *)llp->node)->attributes&0x10)
		llp=llp->right;

	if(llp!=NULL && ((file_selector_file *)llp->node)->in_list==1)
	{

		llp2=llp->right;
		while(llp2!=NULL && ((file_selector_file *)llp2->node)->in_list==1 && 
			!(((file_selector_file *)llp2->node)->attributes&0x10))
			llp2=llp2->right;

		if(llp2!=NULL && ((file_selector_file *)llp2->node)->in_list==1)
		{
			if(llp2->left)
				((LinkedList *)llp2->left)->right=llp2->right;
			if(llp2->right)
				((LinkedList *)llp2->right)->left=llp2->left;
			if(llp->left)
				((LinkedList *)llp->left)->right=llp2;
			else
				head=llp2;

			llp2->left=llp->left;
			llp2->right=llp;
			llp->left=llp2;
			goto sort_loop;
		}
	}
	
	return head;
}

/*
main()
{
	char *ret;

	ret=file_selector(79,39,"C:\\");
	getchar();
}
*/








void remove_flst_cursor(int num_cols,int col_width, int cursor)
{
	if(cursor!=0)
	print_flst_cursor(num_cols,col_width,' ',' ',cursor);
}

void draw_flst_cursor(int num_cols,int col_width, int cursor)
{
	if(cursor!=0)
	print_flst_cursor(num_cols,col_width,'>','<', cursor);
}

void print_flst_cursor(int num_cols,int col_width,char l, char r, int cursor)
{
	int phys_col;
	int phys_curs;
	int x,y;

			/*phys_col=(cursor-1)/col_width;*/
			phys_col=(cursor-1)%num_cols;
/*			phys_curs=(cursor-1)%col_width;*/
			phys_curs=(cursor-1)/num_cols;
			y=phys_curs+2;
			x=phys_col*col_width;


			AT(y,x)
			Bconout(2,l);

			AT(y,x+col_width)
			Bconout(2,r);

}