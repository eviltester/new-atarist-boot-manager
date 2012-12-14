#include "sets.h"
#include "edit.h"
#include "display.h"
#include "global.h"
#include "other.h"



void set_window_on_off_from_set( SET *set, VT52_WINDOW *vtwin)
{
	LinkedList *window_entries, *set_window, *other_window;

	if(vtwin->type==1)
	{
		window_entries=vtwin->entries;

		set_window=set->window_list;
		while( set_window!=NULL && strcmp(((WindowList *)set_window->node)->name,vtwin->name))
		{
/*			printf("\nIn set window and %s is not %s\n",((WindowList *)set_window->node)->name,vtwin->name);
*/			set_window=set_window->right;
		}

		if(set_window!=NULL)
		{
/*			printf("found set %s\n",((WindowList *)set_window->node)->name);
*/			set_progs_on_off_from_set(((WindowList *)set_window->node)->stuff,window_entries);
		}
		else
			set_progs_on_off_from_set(NULL,window_entries);

	}
	else
	if(vtwin->type==3)
	{
		window_entries=vtwin->entries;

		other_window=set->window_list;
		while( other_window!=NULL && strcmp(((WindowList *)other_window->node)->name,vtwin->name))
		{
/*			printf("\nIn set window and %s is not %s\n",((WindowList *)set_window->node)->name,vtwin->name);
*/			other_window=other_window->right;
		}

		if(other_window!=NULL)
		{
/*			printf("found set %s\n",((WindowList *)set_window->node)->name);
*/			vtwin->entries=((WindowList *)other_window->node)->stuff;
		}
		else
			vtwin->entries=NULL;

		set_cursors(vtwin);
	}		
/*	while(!Bconstat(2));
		Bconin(2);*/
}
		



/* this function acts on any progs window type */
void set_progs_on_off_from_set( LinkedList *set_entries, LinkedList *progs)
{
	FILELIST *fp;
	LinkedList *llp;

	llp=progs;

	if(set_entries!=NULL)
	{
/*printf("SET ENTRIES NOT NULL\n");*/
		while(llp!=NULL)
		{
			fp=(FILELIST *)llp->node;

/*printf("checking for %s",fp->name);*/
			if(prog_in_set(fp->name, set_entries))
			{
				fp->on_off=1;
				/*printf(" found\n");*/
			}
			else
			{
				/*printf(" not found\n");*/
				fp->on_off=0;
			}

			llp=llp->right;
/*		while(!Bconstat(2));
			Bconin(2);
*/		}
	}
	else
	{
/*printf("SET ENTRIES NULL\n");*/
		while(llp!=NULL)
		{
			fp=(FILELIST *)llp->node;
			fp->on_off=0;
			llp=llp->right;
		}
	}
}



void set_set_details_from_window( SET *set, VT52_WINDOW *vtwin)
{
	LinkedList *window_entries,  *set_window;
	WindowList *wlp;
	char *cp;

	if(vtwin->type==1)
	{
		window_entries=vtwin->entries;

		/* find the correct window list details & if it doesn't exist then
			create it and tag it on */

		set_window=set->window_list;
		while( set_window!=NULL && strcmp(((WindowList *)set_window->node)->name,vtwin->name))
			set_window=set_window->right;

		if(set_window!=NULL)
		{
			((WindowList *)set_window->node)->stuff=set_set_from_progs(((WindowList *)set_window->node)->stuff,window_entries);
		}
		else
		{
			wlp=(WindowList *)malloc(sizeof(WindowList));
			if(wlp==NULL)
			{
				fprintf(stderr,"error, couldn't allocate Window list space ER3\n");
				exit(1);
			}

			cp=(char *)malloc(strlen(vtwin->name)+1);
			strcpy(cp,vtwin->name);

			wlp->name=cp;
			wlp->stuff=NULL;

			set->window_list=append_LinkedList(set->window_list, (void *)wlp);
			wlp->stuff=set_set_from_progs(wlp->stuff,window_entries);

			
		}
	}
	else
	if(vtwin->type==3)
	{
		set_window=set->window_list;
		while( set_window!=NULL && strcmp(((WindowList *)set_window->node)->name,vtwin->name))
			set_window=set_window->right;

		if(set_window!=NULL)
		{
			((WindowList *)set_window->node)->stuff=vtwin->entries;
		}
		else
		{
			wlp=(WindowList *)malloc(sizeof(WindowList));
			if(wlp==NULL)
			{
				fprintf(stderr,"error, couldn't allocate Window list space ER3\n");
				exit(1);
			}

			cp=(char *)malloc(strlen(vtwin->name)+1);
			strcpy(cp,vtwin->name);

			wlp->name=cp;
			wlp->stuff=NULL;

			set->window_list=append_LinkedList(set->window_list, (void *)wlp);
			wlp->stuff=vtwin->entries;

			
		}
	}
}

LinkedList *set_set_from_progs( LinkedList *set, LinkedList *progs)

/*(SET *set, FILELIST *progs)*/
{
	FILELIST *fp;
	SMALLFILE *temp;
	LinkedList *sets_stuff,*ps,*templl;
	

	sets_stuff=set;
	ps=NULL;

	/* overwrite old ones, create new ones if needed and delete any extra */

	if(progs!=NULL)
	{

		while(progs!=NULL)
		{
			fp=(FILELIST *)progs->node;

			if(fp->on_off==1)
			{

				if(sets_stuff==NULL)	/* need to create a new node */
				{
					temp=(SMALLFILE *)malloc(sizeof(SMALLFILE));
					if(temp==NULL)
					{
						fprintf(stderr,"error: not enough memory for this set ERtX\n");
						exit(1);
					}
					else
					{
						templl=create_LinkedList();
						templl->left=NULL;
						temp->right=NULL;
						templl->node=(void *)temp;
						strcpy(temp->name,fp->name);

						/* append it to the list */
						if(ps==NULL)	/* this was the first one we created ! */
						{
							set=templl;
							sets_stuff=templl;
							ps=templl;
							sets_stuff=sets_stuff->right;
						}
						else
						{
							sets_stuff=templl;
							ps->right=sets_stuff;
							sets_stuff->left=ps;
							ps=sets_stuff;
							sets_stuff=sets_stuff->right;
						}
					}
				}
				else 	/* there is a slot here already */
				{
					strcpy(((SMALLFILE *)sets_stuff->node)->name,fp->name);
					ps=sets_stuff;
					sets_stuff=sets_stuff->right;
				}
			}

			progs=progs->right;
		}

		if(sets_stuff!=NULL)	/* there is some overflow */
		{
			templl=sets_stuff;

			((LinkedList *)sets_stuff->left)->right=NULL;

			
			while(templl)
			{
				temp=(SMALLFILE *)sets_stuff->node;
				free(temp);
				ps=templl->right;
				free(templl);
				templl=ps;
			}
		}

	return set;
	}
	else
		return NULL;
}


int prog_in_set( char *progname, LinkedList *sp)
{
	SMALLFILE *sfp;

	if(sp!=NULL)
	{

		while(sp!=NULL)
		{
			sfp=(SMALLFILE *)sp->node;

/*			printf("checking %s against %s\n",progname,sfp->name);*/
			if(!strcmp(progname,sfp->name))
				return 1;
			sp=sp->right;
/*			while(!Bconstat(2));
				Bconin(2);*/
		}
	}
	return 0;
}


SET *create_set( void )
{
/*	long k;*/
	SET *new_set;

	new_set=(SET *)malloc(sizeof(SET));

	if(new_set==NULL)
	{
		AT(0,0)
		REVERSE_VIDEO
		centre_text(" ERROR: NOT ENOUGH MEMORY FOR A NEW SET ",line_buffer,prog_defaults.columns,' ');
		printf("%s",line_buffer);
		NORMAL_VIDEO
/*		AT(bottom_title_row+1,0)
		REVERSE_VIDEO
		centre_text(" PRESS ANY KEY ",line_buffer,prog_defaults.columns,' ');
		printf("%s",line_buffer);
		NORMAL_VIDEO	
		while(!Bconstat(2))
			;
		Bconin(2);*/
		return NULL;
	}

	new_set->left=NULL;
/*	strcpy( new_set->set_name, "NEW SET" );*/
	new_set->set_name[0]='\0';
	new_set->progs=NULL;
	new_set->accs=NULL;
	new_set->others=NULL;
	new_set->right=NULL;
	new_set->window_list=NULL;
	return(new_set);
}


int valid_set_char( char ch )
{
		if(	isalnum(ch) || ispunct(ch) || isspace(ch) )
		return 1;

	return 0;
}

SET *copy_set( SET *from_set)
{
	SET *temp;
	char *pch1,*pch2;
	OTHER_FILES *pOtherFiles;
	SMALLFILE *pSmallfile;

	LinkedList *pLinkedList,*pFromLinkedList,*pWindowListStuff,*pFromWindowListStuff;
	WindowList *pWindowList,*pFromWindowList;

	if(from_set!=NULL)
	{
		/* create set */
		temp=/*(SET *)malloc(sizeof(SET))*/create_set();

		if(temp!=NULL)
		{
			strcpy(temp->set_name,from_set->set_name);
			pFromLinkedList=from_set->window_list;

			/* copy the windows listed */
			while(pFromLinkedList!=NULL)
			{
				pWindowList=(WindowList *)malloc(sizeof(WindowList));
				pFromWindowList=pFromLinkedList->node;
				if(pFromWindowList==NULL)
					return NULL;

				if(pWindowList!=NULL)
				{
					pch1=(char *)malloc(strlen(pFromWindowList->name)+1);
					strcpy(pch1,pFromWindowList->name);

					pFromWindowListStuff=pFromWindowList->stuff;
					pWindowList->name=pch1;
					pWindowList->stuff=NULL;

					while(pFromWindowListStuff!=NULL)
					{
						if(!strcmp(pch1,"OTHERS"))
						{
							pOtherFiles=(OTHER_FILES *)malloc(sizeof(OTHER_FILES));
							if(pOtherFiles==NULL)
								return NULL;
							pch2=(char *)malloc(strlen(((OTHER_FILES *)pFromWindowListStuff->node)->from)+1);
							if(pch2==NULL)
								return NULL;

							strcpy(pch2,((OTHER_FILES *)pFromWindowListStuff->node)->from);

							pOtherFiles->from=pch2;
							pch2=(char *)malloc(strlen(((OTHER_FILES *)pFromWindowListStuff->node)->to)+1);
							if(pch2==NULL)
								return NULL;

							strcpy(pch2,((OTHER_FILES *)pFromWindowListStuff->node)->to);
							pOtherFiles->to=pch2;
			
							pWindowList->stuff=append_LinkedList(pWindowList->stuff,pOtherFiles);
						}
						else
						{
							pSmallfile=(SMALLFILE *)malloc(sizeof(SMALLFILE));
							if(pSmallfile==NULL)
								return NULL;

							strcpy(pSmallfile->name,((SMALLFILE *)pFromWindowListStuff->node)->name);
							pWindowList->stuff=append_LinkedList(pWindowList->stuff,pSmallfile);
						}	
						pFromWindowListStuff=pFromWindowListStuff->right;
					}
				}
				else
					return NULL;

				temp->window_list=append_LinkedList(temp->window_list,pWindowList);
				pFromLinkedList=pFromLinkedList->right;
			}

		}
	}
	return temp;
}

void new_set_key( VT52_WINDOW *sets_window, screen *sp, SET *from_set )
{

	SET *temp,*save_curr;
	int save_logical,save_physical,save_first_entry,save_old,esc_edit;
	LinkedList *list;

	if(from_set==NULL)
		temp=create_set();
	else
		temp=copy_set(from_set);

	if(temp!=NULL)
	{
/*		total_sets++;*/
		sets_window->total_entries++;

		save_logical=sets_window->logical_cursor;
		save_physical=sets_window->physical_cursor;
		save_old=sets_window->old_cursor;
		save_first_entry=sets_window->first_entry_on_screen;
		save_curr=curr_set;

		sets_window->logical_cursor=sets_window->total_entries;
		sets_window->old_cursor=sets_window->old_cursor;
		curr_set=temp;

/*printf("In NEW SETS && entries is %d\n",sets_window->total_entries);
while(!Bconstat(2));
Bconin(2);
*/		if(sets_window->total_entries!=0)  /* no sets yet */
		{
/* not just 1 screen */
			if(sets_window->total_entries>sets_window->height)	
			{

/* this line is strange, when I remove it, the prog don't work*/
printf("");


					/* not on this screen */
				if((sets_window->total_entries-sets_window->first_entry_on_screen+1)>sets_window->height)
					{
							sets_window->first_entry_on_screen=sets_window->total_entries-sets_window->height+1;
							sets_window->physical_cursor=sets_window->height;


/*printf("In NEW SETS && not just on this screen\n");
printf("In NEW SETS && not just 1 screen");
while(!Bconstat(2));
Bconin(2);*/

					}
				else
					{		
/*printf("In NEW SETS && on this screen\n");
while(!Bconstat(2));
Bconin(2);*/
						sets_window->physical_cursor=sets_window->total_entries-sets_window->first_entry_on_screen+1;
					}
			}
			else
			{
/*printf("In NEW SETS && just 1 screen\n");
while(!Bconstat(2));
Bconin(2);*/
				sets_window->first_entry_on_screen=1;
				sets_window->physical_cursor=sets_window->total_entries;
			}
		}
		else
		{
/*printf("In NEW SETS && setting everything to 1\n");
while(!Bconstat(2));
Bconin(2);*/
			curr_set=temp;
			sets_window->old_cursor=1;
			sets_window->first_entry_on_screen=1;
			sets_window->logical_cursor=1;
			sets_window->physical_cursor=1;
			sets_window->total_entries=1;
		}

		sets_window->entries=append_LinkedList(sets_window->entries, temp);
		remove_cursor(sets_window,sets_window->physical_cursor,sp,NULL);
		print_window_frame(sets_window,1,sp,NULL);
		print_window_contents(sets_window,sp,NULL);
		draw_cursor(sets_window,sets_window->physical_cursor,sp,NULL);
		print_area(sp,sets_window->xpos,sets_window->ypos,sets_window->xpos+(sets_window->chars_per_col*sets_window->columns)+sets_window->columns+2,sets_window->ypos+sets_window->height+1);

		esc_edit=edit_at(sets_window->ypos+sets_window->physical_cursor,
					sets_window->xpos+2,
					12,
					12,
					curr_set->set_name,
					'-',
					1,
					valid_set_char,
					0,
					sp->height);	


/* delete_last entry in list */
		if(esc_edit || curr_set->set_name[0]=='\0')
		{
/*printf("In NEW SETS && curr_set->name is blank!\n");
while(!Bconstat(2));
Bconin(2);*/
			list=sets_window->entries;
			while(list!=NULL && list->right!=NULL)
			{
				list=list->right;
			}
			if(list!=NULL)
			{
				if(list->left!=NULL)
				{
					((LinkedList *)list->left)->right=NULL;
				}

				if(sets_window->entries==list)
					sets_window->entries=NULL;

				delete_LinkedList((void *)((SET *)list->node)->window_list,delete_window_list);
				free(list->node);
				free(list);
				sets_window->total_entries--;
				sets_window->old_cursor=save_old;
				sets_window->first_entry_on_screen=save_first_entry;
				sets_window->logical_cursor=save_logical;
				sets_window->physical_cursor=save_physical;
				curr_set=save_curr;
			}
				
		}

/*		sets_bottom_title();
		print_sets_in_window( sets );*/

		remove_cursor(sets_window,sets_window->physical_cursor,sp,NULL);
		print_window_frame(sets_window,1,sp,NULL);
		print_window_contents(sets_window,sp,NULL);
		draw_cursor(sets_window,sets_window->physical_cursor,sp,NULL);
		print_area(sp,sets_window->xpos,sets_window->ypos,sets_window->xpos+(sets_window->chars_per_col*sets_window->columns)+sets_window->columns+2,sets_window->ypos+sets_window->height+1);


/*		return(sets);*/
	}
/*	else
		return NULL;*/
}

void edit_set( VT52_WINDOW *vtwin, screen *sp )
{
	LinkedList *temp;
	int loop;

	temp=vtwin->entries;

	if(temp!=NULL)
	{
		loop=1;

		while(loop!=vtwin->logical_cursor)
		{
			loop++;
			temp=temp->right;
		}

		edit_at(vtwin->ypos+vtwin->physical_cursor,
					vtwin->xpos+2,
					12,
					12,
					((SET *)temp->node)->set_name,
					'-',
					1,
					valid_set_char,
					0,
					sp->height);	

		print_window_frame(vtwin,1,sp,NULL);
		print_window_contents(vtwin,sp,NULL);
		draw_cursor(vtwin,vtwin->physical_cursor,sp,NULL);
		print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->chars_per_col*vtwin->columns)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);

/*		sets_bottom_title();
		print_sets_in_window( sets );*/
	}
}

void delete_set( VT52_WINDOW *vtwin, screen *sp)
{
	SET *del_set;
	LinkedList *llp;
/*	SET *first_set;*/
	int loop;

if(vtwin->entries!=NULL)
{
/*	del_set=sets;*/
	llp=vtwin->entries;
	loop=1;

	while(loop!=vtwin->logical_cursor)
	{
		loop++;
		llp=llp->right;
	}
	
	del_set=(SET *)llp->node;

	if(curr_set==del_set)
	{
		if(llp->left==NULL)
		{
			if(llp->right!=NULL)
				curr_set=((LinkedList *)llp->right)->node;
			else
				curr_set=NULL;
		}
		else
			curr_set=((LinkedList *)llp->left)->node;

/*		set_progs_on_off_from_set(curr_set, prgs);
		set_accs_on_off_from_set(curr_set,accs);
		if(curr_set!=NULL)
			others=curr_set->others;
		else
			others=NULL;*/
	}

	vtwin->entries=dlte_set(vtwin->entries, llp);

	vtwin->total_entries--;
	if(vtwin->logical_cursor!=1)
		vtwin->logical_cursor--;


	if(vtwin->first_entry_on_screen>vtwin->total_entries)
	{
		if(vtwin->paging==SCROLLING)
			vtwin->first_entry_on_screen=vtwin->total_entries;
		else
			vtwin->first_entry_on_screen=vtwin->total_entries-vtwin->height+1;
	}		

	if(vtwin->logical_cursor<=vtwin->first_entry_on_screen)
	{
		if(vtwin->paging==SCROLLING)
			vtwin->first_entry_on_screen=vtwin->logical_cursor;
		else
		{
			vtwin->first_entry_on_screen=vtwin->logical_cursor-vtwin->height+1;
			if(vtwin->first_entry_on_screen<0)
			{
				if(vtwin->total_entries==0)		
					vtwin->first_entry_on_screen=0;
				else
					vtwin->first_entry_on_screen=1;
			}
		}
	}

	vtwin->old_cursor=vtwin->physical_cursor;						

	if(vtwin->total_entries==0)
	{
		vtwin->logical_cursor=0;
		vtwin->physical_cursor=0;
		vtwin->entries=NULL;
	}	
	else
		vtwin->physical_cursor=vtwin->logical_cursor-vtwin->first_entry_on_screen+1;


		print_window_frame(vtwin,1,sp,NULL);
		print_window_contents(vtwin,sp,NULL);
		draw_cursor(vtwin,vtwin->physical_cursor,sp,NULL);
		print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->chars_per_col*vtwin->columns)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);

/*	print_sets_in_window( sets );
	print_sets_title(logical_sets_cursor);
	remove_cursor(current_window,old_sets_cursor);	
	draw_cursor(current_window,sets_cursor);
	return(sets);*/

}
else
	{
	BELL
/*	return(NULL);*/
	}
}

void delete_smallfile(SMALLFILE *sfp)
{
	free(sfp);
}

void delete_window_list(WindowList *wlp)
{
	delete_LinkedList(wlp->stuff,delete_smallfile);
	free(wlp);
}

LinkedList *dlte_set( LinkedList *sets, LinkedList *del_set )
{

	LinkedList *first_set;
/*	int loop;*/

	if((SET *)sets->node==(SET *)del_set->node)
	{
		first_set=sets->right;
	}
	else
		first_set=sets;

	if(del_set->left!=NULL)
		((LinkedList *)del_set->left)->right=del_set->right;

	if(del_set->right!=NULL)
		((LinkedList *)del_set->right)->left=del_set->left;

	delete_LinkedList((void *)((SET *)del_set->node)->window_list,delete_window_list);
	free(del_set);
	return(first_set);

}





/******************************************8
******************************************8
FOLLOWING FUNCTIONS DELETED BUT HERE FOR REFERENCE
******************************************88
***************************************/




void sets_window( void )
{

/*
	int width;
	int loop;

	width=SETS_NAME_SIZE+ARROW_BORDER+ARROW_BORDER+LINE_BORDER+
					LINE_BORDER;

	/* accs title */

	centre_text("SETS",line_buffer,width,' ');
	AT(mid_window_title_row,sets_menu_col)
	printf("%s",line_buffer);

	centre_text("--",line_buffer,width,'-');
	AT(mid_window_title_row+1,sets_menu_col)
	line_buffer[0]='+';
	line_buffer[width-1]='+';
	printf("%s",line_buffer);

	for(loop=0;loop<width;loop++)
		line_buffer[loop]=' ';
	line_buffer[loop]='\0';

	line_buffer[0]='|';
	line_buffer[width-1]='|';

	for(loop=0;loop<prog_defaults.progs_win;loop++)
	{
		AT(mid_window_first_item_row+loop,sets_menu_col)
		printf("%s",line_buffer);
	}

	centre_text("--",line_buffer,width,'-');
	line_buffer[0]='+';
	line_buffer[width-1]='+';
	AT(mid_window_first_item_row+prog_defaults.progs_win,sets_menu_col)
	printf("%s",line_buffer);
*/
}


void print_sets_in_window( SET *sets )
{
/*
	SET *temp;
	int loop;

	temp=sets;

	loop=1; 
	while(temp!=NULL && loop<first_set_on_screen)
	{
		 loop++;
		temp=temp->right;
	}

	

	for(loop=0;loop<prog_defaults.progs_win;loop++)
	{

		if(temp!=NULL)
		{

			if(temp==curr_set)
			{
				AT(mid_window_first_item_row+loop, sets_menu_col+2)
				printf("            ");
				AT(mid_window_first_item_row+loop, sets_menu_col+2)
				REVERSE_VIDEO 
				printf("%s",temp->set_name); 
				NORMAL_VIDEO
			}
			else
			{
				AT(mid_window_first_item_row+loop, sets_menu_col+2)
				printf("            ");
				AT(mid_window_first_item_row+loop, sets_menu_col+2)
				printf("%s",temp->set_name); 
			}

		temp=temp->right;
		}
		else
		{
				AT(mid_window_first_item_row+loop, sets_menu_col+2)
				printf("            ");
		}
	}
*/
}

void print_sets_title( int logical_sets_cursor )
{

/*
	int width;
/*	int loop;*/
	char total[4];
/*	char current[4];*/

	width=SETS_NAME_SIZE+ARROW_BORDER+ARROW_BORDER+LINE_BORDER+
					LINE_BORDER;

	sprintf(total,"%d",total_sets);
	centre_text("--",line_buffer,width,'-');
	AT(mid_window_title_row+1,sets_menu_col)
	line_buffer[0]='+';
	line_buffer[width-1]='+';
	printf("%s",line_buffer);

	AT(mid_window_title_row+1,sets_menu_col+width-2-strlen(total))
	printf("%s",total);


	AT(mid_window_title_row+1,sets_menu_col+2)
	printf("%d",logical_sets_cursor);
*/

}


void sets_up_arrow( void )
{
/*
		int num_in_window;

		num_in_window=prog_defaults.progs_win;		


/* at first entry */
/* at middle entry in middle */
/* at middle entry at top of window */


	if(logical_sets_cursor<=1) /* at first entry */
					BELL
	else
	{
		if(sets_cursor<=1)
		{
			old_sets_cursor=sets_cursor;
			sets_cursor=1;


				if(prog_defaults.sets_paging==SCROLLING)
				{
					logical_sets_cursor--;
					first_set_on_screen--;
				}
				else
				{
					first_set_on_screen-=num_in_window;
					logical_sets_cursor--;
					if(logical_sets_cursor<num_in_window)
						sets_cursor=logical_sets_cursor;
					else
						sets_cursor=num_in_window;
				}

				if(first_set_on_screen<1)
					first_set_on_screen=1;
				if(logical_sets_cursor<1)
					logical_sets_cursor=1;

			print_sets_in_window( sets );

		}
		else
		{
			old_sets_cursor=sets_cursor;
			sets_cursor--;
			logical_sets_cursor--;
			
		}

		print_sets_title(logical_sets_cursor);
		remove_cursor(current_window,old_sets_cursor);	
		draw_cursor(current_window,sets_cursor);
	}
*/
}

void sets_down_arrow( void )
{
/*
		int num_in_window;

		num_in_window=prog_defaults.progs_win;		

/* last entry */
/* middle entry at end */
/* middle entry in middle */

	if(logical_sets_cursor>=total_sets)
					BELL
	else
	{
		if(sets_cursor>=num_in_window)
		{

				sets_cursor=num_in_window;
				old_sets_cursor=sets_cursor;

			if(prog_defaults.sets_paging==SCROLLING)
			{
				first_set_on_screen++;
				logical_sets_cursor++;

			}
			else
			{
				logical_sets_cursor++;
				first_set_on_screen+=num_in_window;
				sets_cursor=1;
			}

			if(first_set_on_screen>total_sets)
				first_set_on_screen=total_sets-num_in_window;
			if(first_set_on_screen<1)
				first_set_on_screen=1;

			if(logical_sets_cursor>total_sets)
				logical_sets_cursor=total_sets;

			print_sets_in_window( sets );

		}
		else
		{
			logical_sets_cursor++;
			old_sets_cursor=sets_cursor;
			sets_cursor++;

		}

		print_sets_title(logical_sets_cursor);
		remove_cursor(current_window,old_sets_cursor);	
		draw_cursor(current_window,sets_cursor);
	}
*/	
}


void select_set( void )
{
/*
	SET *sel_set;
/*	SET *first_set;*/
	int loop;

/*	AT(0,0)
	printf("SELECT 1");
*/

if(sets!=NULL)
{
	sel_set=sets;
	loop=1;

/*	AT(0,0)
	printf("SELECT 2");
*/
	while(loop!=logical_sets_cursor)
	{
		loop++;
		sel_set=sel_set->right;
/*	AT(0,0)
	printf("SELECT 2.%d",loop);
*/
	}

/*	AT(0,0)
	printf("SELECT 3     ");
*/
	if(curr_set!=sel_set)
	{

/*	AT(0,0)
	printf("SELECT 4");
*/
		set_set_from_progs(curr_set, prgs);
		set_set_from_accs( curr_set, accs);

		if(curr_set!=NULL)
			curr_set->others=others;

/*	AT(0,0)
	printf("SELECT 5");
*/
		curr_set=sel_set;
		set_other_counts();

/*	AT(0,0)
	printf("SELECT 6");
*/
		print_sets_in_window( sets );
/*	AT(0,0)
	printf("SELECT 7");
*/
		set_progs_on_off_from_set(curr_set, prgs);
/*	AT(0,0)
	printf("SELECT 8");
*/
		set_accs_on_off_from_set(curr_set,accs);
/*	AT(0,0)
	printf("SELECT 9");
*/

		if(curr_set!=NULL)
			others=curr_set->others;
		else
			others=NULL;

		set_other_counts();
		draw_old_window_title(4);
		print_accs(accs,curr_set,first_acc_on_screen);
/*	AT(0,0)
	printf("SELECT 10");
*/
		print_progs(prgs,curr_set,first_prog_on_screen);
		print_others_in_window(others);

	}
/*	AT(0,0)
	printf("SELECT 11");
*/
}
/*	AT(0,0)
	printf("SELECT 12");
*/
*/
}



int sets_main_loop( int *shift_key )
{

/*
/*		long k;*/
		int u,l,s;

	while(1)
	{

		while(!Bconstat(2))
		{
			/*possibly update the clock here*/;
		}
/*		k=Bconin(2);
		u=(int)(k>>16);
		l=(int)(k%256);
*/
		get_key(&s,&u,&l);


		if(u==0x48 && s!=2)
			sets_up_arrow();
		else
		{
			if(u==prog_defaults.new_key_high)
				{
				set_set_from_progs(curr_set, prgs);
				set_set_from_accs( curr_set, accs);
				if(curr_set!=NULL)
					curr_set->others=others;
				set_other_counts();
				sets=new_set_key(sets);
				set_progs_on_off_from_set(curr_set, prgs);
				set_accs_on_off_from_set(curr_set,accs);	
				if(curr_set!=NULL)
					others=curr_set->others;
				else
					others=NULL;
				set_other_counts();
				draw_old_window_title(4);
				print_accs(accs,curr_set,first_acc_on_screen);
				print_progs(prgs,curr_set,first_prog_on_screen);
				print_others_in_window(others);
				
				}
			else
			{
				if(u==prog_defaults.page_key_high || (u==0x50 && s==2))
					sets_paging();
				else
				{
					if(u==0x50 && s!=2)
					{
						sets_down_arrow();
					}
					else
					{
						if(u==prog_defaults.delete_key_high)
						{
							set_set_from_progs(curr_set, prgs);
							set_set_from_accs( curr_set, accs);
							if(curr_set!=NULL)
								curr_set->others=others;
							set_other_counts();
							sets=delete_set(sets,logical_sets_cursor);
							set_progs_on_off_from_set(curr_set, prgs);
							set_accs_on_off_from_set(curr_set,accs);	
							if(curr_set!=NULL)
								others=curr_set->others;
							else
								others=NULL;
							set_other_counts();
							draw_old_window_title(4);
							print_accs(accs,curr_set,first_acc_on_screen);
							print_progs(prgs,curr_set,first_prog_on_screen);
							print_others_in_window(others);
						}
						else
						{
							if(u==prog_defaults.toggle_key_high)
							{
								select_set();
							}
							else
							{
								if(u==0x1C)
								{
									edit_set();
									sets_bottom_title();
								}
								else
								{
									if(u==0x48 && s==2)
										sets_paging_up();
									else
									{
										*shift_key=s;
										return u;
									}
								}
							}
						}
					}
				}
			}
		}
	}
*/
}

void sets_bottom_title( void )
{
/*
	char temp[80];

	DISCARD_EOL

	if(prog_defaults.columns<=40)
		sprintf( temp,
				"^v SEL-%s NEW-%s DEL-%s ED-ENTER",
				prog_defaults.toggle_key_text,
				prog_defaults.new_key_text,
				prog_defaults.delete_key_text);
	else	
	sprintf(	temp,
				"CURS-^v SELECT-%s PAGE-%s NEW-%s DEL-%s EDIT-RETURN",
				prog_defaults.toggle_key_text,
				prog_defaults.page_key_text,
				prog_defaults.new_key_text,
				prog_defaults.delete_key_text);

	AT(bottom_title_row,0)
	REVERSE_VIDEO
	centre_text(temp,line_buffer,prog_defaults.columns,' ');
	printf("%s",line_buffer);

	if(prog_defaults.columns<=40)
		sprintf( temp,
				"SWTC-%s HLP-%s OPT-%s BYE-%s",
				prog_defaults.switch_key_text,
				prog_defaults.help_key_text,
				prog_defaults.options_key_text,
				prog_defaults.abort_key_text);
	else	
		sprintf( temp,
				"SWITCH-%s  HELP-%s  OPTIONS-%s EXIT-%s",
				prog_defaults.switch_key_text,
				prog_defaults.help_key_text,
				prog_defaults.options_key_text,
				prog_defaults.abort_key_text);

	AT(bottom_title_row+1,0)
	REVERSE_VIDEO
	centre_text(temp,line_buffer,prog_defaults.columns,' ');
	printf("%s",line_buffer);

	NORMAL_VIDEO
	WRAP_EOL
*/
}

void set_set_from_accs( SET *set, FILELIST *accs)
{
/*	FILELIST *fp;
	SMALLFILE *sfp;
	SMALLFILE *temp;

	fp=accs;

	if(set!=NULL)
	{
		sfp=set->accs;

		/* delete progs first */
		while(sfp!=NULL)
		{
			temp=sfp->right;
			free(sfp);
			sfp=temp;
		}
			
		set->accs=NULL;

		while(fp!=NULL)
		{
			if(fp->on_off==1)
			{
				temp=(SMALLFILE *)malloc(sizeof(SMALLFILE));
				if(temp==NULL)
				{
				AT(bottom_title_row,0)
				REVERSE_VIDEO
				centre_text(" ERROR: NOT ENOUGH MEMORY FOR THIS SET ",line_buffer,prog_defaults.columns,' ');
				printf("%s",line_buffer);
				NORMAL_VIDEO
				AT(bottom_title_row+1,0)
				REVERSE_VIDEO
				centre_text(" PRESS ANY KEY ",line_buffer,prog_defaults.columns,' ');
				printf("%s",line_buffer);
				NORMAL_VIDEO	
				while(!Bconstat(2))
					;
				Bconin(2);
				sets_bottom_title();
				}
				else
				{
					temp->left=NULL;
					temp->right=NULL;
					strcpy(temp->name,fp->name);
	
					set->accs=append_smallfile(temp,set->accs);
				}
			}

			fp=fp->right;
		}
	}
*/
}

SMALLFILE *append_smallfile( SMALLFILE *append, SMALLFILE *list)
{
/*	SMALLFILE *first;

	first=list;

	if(list==NULL)
	{
		return(append);
	}
	else
	{
		while(list->right!=NULL)
			list=list->right;

		list->right=append;
		append->left=list;

		return(first);
	}
*/
}

void set_accs_on_off_from_set( SET *set, FILELIST *accs)
{
/*	FILELIST *fp;

	fp=accs;

	if(set!=NULL)
	{
		while(fp!=NULL)
		{
			if(acc_in_set(fp->name, set))
				fp->on_off=1;
			else
				fp->on_off=0;

			fp=fp->right;
		}
	}
	else
	{
		while(fp!=NULL)
		{
			fp->on_off=0;
			fp=fp->right;
		}
	}
*/
}

int acc_in_set( char *accname, SET *sp)
{
/*
	SMALLFILE *sfp;

	if(sp!=NULL)
	{
		sfp=sp->accs;

		while(sfp!=NULL)
		{
			if(!strcmp(accname,sfp->name))
				return 1;
			sfp=sfp->right;
		}

	}

	return 0;
*/
}

SET *append_set_to_list( SET *to_add, SET *list)
{
/*	SET *first;

	first=list;

	if(list==NULL)
	{
		return(to_add);
	}
	else
	{
		while(list->right!=NULL)
			list=list->right;

		list->right=to_add;
		to_add->left=list;

		return(first);
	}
*/
}

void sets_paging()
{
/*		int num_in_window;
		int column;
		num_in_window=prog_defaults.progs_win;		
		column=1;

		if(first_set_on_screen+num_in_window<total_sets+1)
		{

				first_set_on_screen+=num_in_window;
				logical_sets_cursor+=num_in_window;

				if(logical_sets_cursor>total_sets)
					logical_sets_cursor=total_sets;

				old_sets_cursor=sets_cursor;
				sets_cursor=logical_sets_cursor-first_set_on_screen;
				sets_cursor++;

				print_sets_in_window( sets );
				print_sets_title(logical_sets_cursor);
				remove_cursor(current_window,old_sets_cursor);	
				draw_cursor(current_window,sets_cursor);
		}	
		else
			BELL
*/
}


void sets_paging_up()
{
/*		int num_in_window;
		int column;
		num_in_window=prog_defaults.progs_win;		
		column=1;

		if(first_set_on_screen>1)
		{

				first_set_on_screen-=num_in_window;
				logical_sets_cursor-=num_in_window;

				if(logical_sets_cursor<1)
					logical_sets_cursor=1;
		
				if(first_set_on_screen<1)
					first_set_on_screen=1;

				old_sets_cursor=sets_cursor;
				sets_cursor=logical_sets_cursor-first_set_on_screen;
				sets_cursor++;

				print_sets_in_window( sets );
				print_sets_title(logical_sets_cursor);
				remove_cursor(current_window,old_sets_cursor);	
				draw_cursor(current_window,sets_cursor);
		}	
		else
			BELL
*/
}
