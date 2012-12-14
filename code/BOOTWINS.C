#include "bootwins.h"
#include "vt52.h"
#include "autofold.h"
#include "filelist.h"

LinkedList *circular_windows_list;	
#define MEM_PRINT

/********************************
	A Progs win is one which only lists program names.
*********************************/

void progs_win_bottom_title( VT52_WINDOW *vtwin
#ifdef MEM_PRINT
, screen *sp
#endif
)
{
	
	char temp[50];

#ifndef MEM_PRINT
	DISCARD_EOL
#endif

	sprintf(	temp,
				"CURSOR:[^v<>] PAGE: s[^v<>] TOGGLE: SPACE");

#ifdef MEM_PRINT
	screen_at(sp,/*sp->height-1*/auto_details.rows-2,0);
	set_reverse_mode(sp);
	centre_text(temp,line_buffer,/*sp->width-1*/auto_details.columns,' ');
	print_string(sp,line_buffer);
#else
	AT(auto_details.rows-2,0)
	REVERSE_VIDEO
	centre_text(temp,line_buffer,auto_details.columns/*sp->width-1*/,' ');
	printf("%s",line_buffer);
#endif

	if(auto_details.columns/*sp->width*/<=40)
		sprintf( temp,
				"SWITCH: TAB,HELP: HELP,OPTNS: aO,BYE: ESC");
	else	
		sprintf( temp,
				"SWITCH: TAB, HELP: HELP, OPTIONS: aO, EXIT: ESC");

#ifdef MEM_PRINT
	screen_at(sp,auto_details.rows-1,0);
	set_reverse_mode(sp);
	centre_text(temp,line_buffer,prog_defaults.columns,' ');
	print_string(sp,line_buffer);
	set_normal_mode(sp);
#else
	AT(auto_details.rows-1,0)
	REVERSE_VIDEO
	centre_text(temp,line_buffer,prog_defaults.columns,' ');
	printf("%s",line_buffer);
	NORMAL_VIDEO
	WRAP_EOL
#endif

}

prog_win_extra_info *create_prog_win_extra_info( void )
{

	prog_win_extra_info *tmp;

	if((tmp=(prog_win_extra_info *)malloc(sizeof(prog_win_extra_info)))==NULL)
	{
		fprintf(stderr,"ERROR, couldnot allocate space for prog win info\n");
		exit(1);
	}

	tmp->good_extension[0]='\0';
	tmp->bad_extension[0]='\0';
	tmp->search_extension[0]='\0';
	tmp->from_path[0]='\0';
	tmp->to_path[0]='\0';
	tmp->action_type=0;
	
	return tmp;
}


VT52_WINDOW *create_prog_win( void )
{

	VT52_WINDOW *win;

	win=create_vt52window();

	win->type=1;
	win->can_resize_horizontal=1;
	win->can_resize_vertical=1;
	win->chars_per_col=8;

	win->extra_info=create_prog_win_extra_info();
	win->print=print_prog_win_contents;
	win->other_keys=prog_win_other_keys;
	win->print_bottom=progs_win_bottom_title;
	win->focus=prog_win_in_focus;
	win->unfocus=prog_win_out_focus;

	return win;
}

void print_prog_win_contents( VT52_WINDOW *vtwin 
#ifdef MEM_PRINT
,screen *sp
#endif
,int *area)
{

	LinkedList *temp;
	int count;
	int row_loop, col_loop;
	int progs_left_char;


	temp=vtwin->entries;
	count=1;
	progs_left_char=vtwin->xpos+2;

/*AT(10,22)
	printf(" in print contents");
*/
if(area==NULL || (area[1]<=vtwin->ypos+1 && area[3]>=vtwin->ypos+1))
{
	while(count<vtwin->first_entry_on_screen && temp!=NULL)
	{
		temp=temp->right;
		count++;
	}

	for(col_loop=0; col_loop<vtwin->columns; col_loop++)
	{
		for(row_loop=0; row_loop<vtwin->height; row_loop++)
		{

		if(area==NULL || (area[1]<=vtwin->ypos+row_loop+1 && area[3] >=vtwin->ypos+row_loop+1))
		{
			if(sp!=NULL)
				screen_at(sp,vtwin->ypos+row_loop+1, vtwin->xpos+2+(col_loop*9));
			else
				AT(vtwin->ypos+row_loop+1, vtwin->xpos+2+(col_loop*9))

		}

			if(temp!=NULL)
			{
			if(area==NULL || (area[1]<=vtwin->ypos+row_loop+1 && area[3] >=vtwin->ypos+row_loop+1))
			{

				if(((FILELIST *)temp->node)->on_off==1) /*in_set(temp->name,curr_set)*/
				{
				if(sp!=NULL)
				{
					print_string(sp,"        ");
					screen_at(sp,vtwin->ypos+1+row_loop, vtwin->xpos+2+(col_loop*9));
					set_reverse_mode(sp);
					print_string(sp,((FILELIST *)temp->node)->name); 
					set_normal_mode(sp);
				}
				else
				{
					printf("        ");
					AT(vtwin->ypos+1+row_loop, vtwin->xpos+2+(col_loop*9))
					REVERSE_VIDEO printf("%s",((FILELIST *)temp->node)->name); NORMAL_VIDEO
				}
				}
				else
				{
					if(sp!=NULL)
					{
						print_string(sp,"        ");
						screen_at(sp,vtwin->ypos+1+row_loop, vtwin->xpos+2+(col_loop*9));
						print_string(sp,((FILELIST *)temp->node)->name);
					}
					else
					{
						printf("        ");
						AT(vtwin->ypos+1+row_loop, vtwin->xpos+2+(col_loop*9))
						printf("%s",((FILELIST *)temp->node)->name);
					}
				}
			}
				temp=temp->right;
			}
			else
			{
			if(area==NULL || (area[1]<=vtwin->ypos+row_loop+1 && area[3] >=vtwin->ypos+row_loop+1))
			{
				if(sp!=NULL)
					print_string(sp,"        ");	
				else
					printf("        ");	

			/*	row_loop=vtwin->height;
				col_loop=vtwin->columns;*/
			}
			}
		}
	}
}
/*AT(10,22)
	printf("out print contents");
*/
}

void print_specific_win_contents( VT52_WINDOW *vtwin 
#ifdef MEM_PRINT
,screen *sp
#endif
)
{

	LinkedList *temp;
	int count;
	int phys_curs;
	int phys_col;
	int y;
	int x;
	int cursor;

	cursor=vtwin->physical_cursor;
	phys_col=(cursor-1)/vtwin->height;
	phys_curs=(cursor-1)%vtwin->height;
	y=phys_curs+vtwin->ypos+1;
	x=vtwin->xpos+1+(phys_col*9)+1;
	temp=vtwin->entries;
	count=1;


/*	progs_left_char=vtwin->xpos+2;
*/

	while(count<vtwin->logical_cursor && temp!=NULL)
	{
		temp=temp->right;
		count++;
	}

#ifdef MEM_PRINT
	if(sp!=NULL)
		screen_at(sp,y, x);
	else
		AT(y, x)
#else
	AT(y, x)
#endif

	if(temp!=NULL)
	{
		if(((FILELIST *)temp->node)->on_off==1) /*in_set(temp->name,curr_set)*/
		{
#ifdef MEM_PRINT
		if(sp!=NULL)
		{
			print_string(sp,"        ");
			screen_at(sp,y, x);
			set_reverse_mode(sp); 
			print_string(sp,((FILELIST *)temp->node)->name); 
			set_normal_mode(sp);
		}
		else
		{
			printf("        ");
			AT(y, x)
			REVERSE_VIDEO printf("%s",((FILELIST *)temp->node)->name); NORMAL_VIDEO
		}
#else
			printf("        ");
			AT(y, x)
			REVERSE_VIDEO printf("%s",((FILELIST *)temp->node)->name); NORMAL_VIDEO
#endif
		}
		else
		{
#ifdef MEM_PRINT
		if(sp!=NULL)
		{
			print_string(sp,"        ");
			screen_at(sp,y, x);
			print_string(sp,((FILELIST *)temp->node)->name);
		}
		else
		{
			printf("        ");
			AT(y, x)
			printf("%s",((FILELIST *)temp->node)->name);
		}
#else
			printf("        ");
			AT(y, x)
			printf("%s",((FILELIST *)temp->node)->name);
#endif
		}
	}
}

void prog_win_toggle_key( VT52_WINDOW *vtwin 
#ifdef MEM_PRINT
,screen *sp
#endif
)
{

	LinkedList *llp,*llp2;
/*	FILELIST *flp;*/
	int loop;

	loop=1;
	llp=vtwin->entries;

		
	while(llp!=NULL && loop!=vtwin->logical_cursor)
	{
		loop++;
		llp=llp->right;
	}

	if(llp!=NULL)
	{
		if(((prog_win_extra_info *)vtwin->extra_info)->action_type==1 && ((FILELIST *)llp->node)->on_off==0)
		{
			llp2=vtwin->entries;

			while(llp2!=NULL)
			{
				((FILELIST *)llp2->node)->on_off=0;
				llp2=llp2->right;
			}
		}

		if(((FILELIST *)llp->node)->on_off==1)
			((FILELIST *)llp->node)->on_off=0;
		else
			((FILELIST *)llp->node)->on_off=1;

		if(((prog_win_extra_info *)vtwin->extra_info)->action_type==0)
#ifdef MEM_PRINT
		{
			print_specific_win_contents(vtwin,sp);
			print_specific_win_contents(vtwin,NULL);
		}
#else
			print_specific_win_contents(vtwin);
#endif

	}

	if(((prog_win_extra_info *)vtwin->extra_info)->action_type==1)
#ifdef MEM_PRINT
		print_prog_win_contents(vtwin,sp,NULL);
#else
		print_prog_win_contents(vtwin,NULL);
#endif

	
}

long prog_win_other_keys( VT52_WINDOW *vtwin, long k
#ifdef MEM_PRINT
,screen *sp
#endif
)
{
	int u,l,s;


	u=(int)(k>>16)&0xff;
	l=(int)(k%256);
	s=(int)(k>>24)&0x0f;	/* we are only interested in Shift, Cntrl and Alt see page 306 for details */

/*	AT(20,10)
	printf("k %ld, u %d, l %d, s %d",k,u,l,s);
*/
	if(u==57)
	{
		prog_win_toggle_key( vtwin 
#ifdef MEM_PRINT
,sp
#endif
);
		return -1;
	}
	else
		return k;
}

void prog_win_in_focus(VT52_WINDOW *vtwin)
{
	vtwin->on_top=1;
}

void prog_win_out_focus(VT52_WINDOW *vtwin)
{
	vtwin->on_top=0;
}

void set_win_in_focus(VT52_WINDOW *vtwin)
{
	vtwin->on_top=1;
}

void set_win_out_focus(VT52_WINDOW *vtwin)
{
	vtwin->on_top=0;
}


void sets_win_bottom_title( VT52_WINDOW *vtwin
#ifdef MEM_PRINT
, screen *sp
#endif
)
{
	
	char temp[50];

#ifndef MEM_PRINT
	DISCARD_EOL
#endif

	sprintf(	temp,
				"CURSOR:[^v<>] PAGE: s[^v<>] TOGGLE: SPACE");

#ifdef MEM_PRINT
	screen_at(sp,auto_details.rows-2,0);
	set_reverse_mode(sp);
	centre_text(temp,line_buffer,auto_details.columns,' ');
	print_string(sp,line_buffer);
#else
	AT(auto_details.rows-2,0)
	REVERSE_VIDEO
	centre_text(temp,line_buffer,auto_details.columns,' ');
	printf("%s",line_buffer);
#endif

	if(auto_details.columns<=40)
		sprintf( temp,
				"SWITCH: TAB,HELP: HELP,OPTNS: aO,BYE: ESC");
	else	
		sprintf( temp,
				"SWITCH: TAB, HELP: HELP, OPTIONS: aO, EXIT: ESC");

#ifdef MEM_PRINT
	screen_at(sp,auto_details.rows-1,0);
	set_reverse_mode(sp);
	centre_text(temp,line_buffer,prog_defaults.columns,' ');
	print_string(sp,line_buffer);
	set_normal_mode(sp);
#else
	AT(auto_details.rows-1,0)
	REVERSE_VIDEO
	centre_text(temp,line_buffer,prog_defaults.columns,' ');
	printf("%s",line_buffer);
	NORMAL_VIDEO
	WRAP_EOL
#endif

}

/*
sets_win_extra_info *create_sets_win_extra_info( void )
{

}
*/

void print_ordered_windows(LinkedList *windows, VT52_WINDOW *top_win, screen *sp)
{

	LinkedList *lp;

	lp=windows;

	while((VT52_WINDOW *)lp->node!=top_win)
		lp=lp->right;

	lp=lp->right;

	do{
			print_window_frame(lp->node,1, sp,NULL);
			print_window_contents(lp->node, sp,NULL);
/*			draw_cursor(lp->node,((VT52_WINDOW *)lp->node)->physical_cursor, sp,NULL);
*/
			lp=lp->right;
		
	}
	while(lp->node!=top_win);

	print_window_frame(lp->node,1, sp,NULL);
	print_window_contents(lp->node, sp,NULL);
	draw_cursor(lp->node,((VT52_WINDOW *)lp->node)->physical_cursor, sp,NULL);
		
	print_window_bottom_help(lp->node, sp);

}

VT52_WINDOW *create_sets_win( void )
{

	VT52_WINDOW *win;

	win=create_vt52window();

	win->type=2;
	win->can_resize_horizontal=0;
	win->can_resize_vertical=1;
	win->chars_per_col=17;
	win->extra_info=NULL;
	win->print=print_sets_win_contents;
	win->other_keys=sets_win_other_keys;
	win->print_bottom=sets_win_bottom_title;
	win->focus=set_win_in_focus;
	win->unfocus=set_win_out_focus;

	return win;
}

void print_sets_win_contents( VT52_WINDOW *vtwin 
#ifdef MEM_PRINT
,screen *sp
#endif
,int *area)
{

	LinkedList *temp;
	int count;
	int row_loop, col_loop;
	int progs_left_char;
	char function_key_string[10];

	temp=vtwin->entries;
	count=1;
	progs_left_char=vtwin->xpos+2;

/*AT(10,22)
	printf(" in print contents");
*/
if(area==NULL || (area[1]<=vtwin->ypos+1 && area[3]>=vtwin->ypos+1))
{
	while(count<vtwin->first_entry_on_screen && temp!=NULL)
	{
		temp=temp->right;
		count++;
	}

	for(col_loop=0; col_loop<vtwin->columns; col_loop++)
	{
		for(row_loop=0; row_loop<vtwin->height; row_loop++)
		{

		if(area==NULL || (area[1]<=vtwin->ypos+row_loop+1 && area[3] >=vtwin->ypos+row_loop+1))
		{
		if(sp!=NULL)
			screen_at(sp,vtwin->ypos+row_loop+1, vtwin->xpos+2+(col_loop*9));
		else
			AT(vtwin->ypos+row_loop+1, vtwin->xpos+2+(col_loop*9))
	
		}

			if(temp!=NULL)
			{
			if(area==NULL || (area[1]<=vtwin->ypos+row_loop+1 && area[3] >=vtwin->ypos+row_loop+1))
			{

				if((SET *)temp->node==curr_set) /*in_set(temp->name,curr_set)*/
				{
				if(sp!=NULL)
				{
					print_string(sp,"                 ");
					screen_at(sp,vtwin->ypos+1+row_loop, vtwin->xpos+2+(col_loop*9));
					set_reverse_mode(sp);
					print_string(sp,((SET *)temp->node)->set_name); 
					set_normal_mode(sp);
				}
				else
				{
					printf("                 ");
					AT(vtwin->ypos+1+row_loop, vtwin->xpos+2+(col_loop*9))
					REVERSE_VIDEO printf("%s",((SET *)temp->node)->set_name); NORMAL_VIDEO
				}
				}
				else
				{
					if(sp!=NULL)
					{
						print_string(sp,"                 ");
						screen_at(sp,vtwin->ypos+1+row_loop, vtwin->xpos+2+(col_loop*9));
						print_string(sp,((SET *)temp->node)->set_name);
					}
					else
					{
						printf("                 ");
						AT(vtwin->ypos+1+row_loop, vtwin->xpos+2+(col_loop*9))
						printf("%s",((SET *)temp->node)->set_name);
					}
				}

			if(count<=40)
			{
				if(count<=10)
				{
					function_key_string[0]=' ';
				}
				else
				{
					if(count<=20)
						function_key_string[0]='s';
					else
					{
						if(count<=30)
							function_key_string[0]='c';
						else
						{
							if(count<=40)
								function_key_string[0]='a';
						}
					}
				}
				
				function_key_string[1]='F';
				if((count%10)!=0)
					sprintf(function_key_string+2,"%d",count%10);
				else
					sprintf(function_key_string+2,"10");

				if(sp!=NULL)
				{
					screen_at(sp,vtwin->ypos+1+row_loop, vtwin->xpos+2+(col_loop*9)+13);
					print_string(sp,function_key_string);
				}
				else
				{									
					AT(vtwin->ypos+1+row_loop, vtwin->xpos+2+(col_loop*9)+13)
					printf("%s",function_key_string);
				}
				
			}
					

			}
				temp=temp->right;
				count++;
			}
			else
			{
			if(area==NULL || (area[1]<=vtwin->ypos+row_loop+1 && area[3] >=vtwin->ypos+row_loop+1))
			{
				if(sp!=NULL)
					print_string(sp,"                 ");	
				else
					printf("                 ");	

			/*	row_loop=vtwin->height;
				col_loop=vtwin->columns;*/
			}
			}
		}
	}
}
/*AT(10,22)
	printf("out print contents");
*/
}

void print_specific_set_win_contents( VT52_WINDOW *vtwin 
#ifdef MEM_PRINT
,screen *sp
#endif
)
{

	LinkedList *temp;
	int count;
	int phys_curs;
	int phys_col;
	int y;
	int x;
	int cursor;

	cursor=vtwin->physical_cursor;
	phys_col=(cursor-1)/vtwin->height;
	phys_curs=(cursor-1)%vtwin->height;
	y=phys_curs+vtwin->ypos+1;
	x=vtwin->xpos+1+(phys_col*9)+1;
	temp=vtwin->entries;
	count=1;


/*	progs_left_char=vtwin->xpos+2;
*/

	while(count<vtwin->logical_cursor && temp!=NULL)
	{
		temp=temp->right;
		count++;
	}

#ifdef MEM_PRINT
	if(sp!=NULL)
		screen_at(sp,y, x);
	else
		AT(y, x)
#else
	AT(y, x)
#endif

	if(temp!=NULL)
	{
		if((SET *)temp->node==curr_set) /*in_set(temp->name,curr_set)*/
		{
#ifdef MEM_PRINT
		if(sp!=NULL)
		{
			print_string(sp,"                     ");
			screen_at(sp,y, x);
			set_reverse_mode(sp); 
			print_string(sp,((SET *)temp->node)->set_name); 
			set_normal_mode(sp);
		}
		else
		{
			printf("                     ");
			AT(y, x)
			REVERSE_VIDEO printf("%s",((SET *)temp->node)->set_name); NORMAL_VIDEO
		}
#else
			printf("                     ");
			AT(y, x)
			REVERSE_VIDEO printf("%s",((SET *)temp->node)->set_name); NORMAL_VIDEO
#endif
		}
		else
		{
#ifdef MEM_PRINT
		if(sp!=NULL)
		{
			print_string(sp,"                     ");
			screen_at(sp,y, x);
			print_string(sp,((SET *)temp->node)->set_name);
		}
		else
		{
			printf("                     ");
			AT(y, x)
			printf("%s",((SET *)temp->node)->set_name);
		}
#else
			printf("                     ");
			AT(y, x)
			printf("%s",((SET *)temp->node)->set_name);
#endif
		}
	}
}

void set_win_toggle_key( VT52_WINDOW *vtwin 
#ifdef MEM_PRINT
,screen *sp
#endif
)
{
/*
	LinkedList *llp,*llp2;
/*	FILELIST *flp;*/
	int loop;

	loop=1;
	llp=vtwin->entries;

print_screen(sp);

while(!Bconstat(2));
	Bconin(2);
		
	while(llp!=NULL && loop!=vtwin->logical_cursor)
	{
		loop++;
		llp=llp->right;
	}

	if(llp!=NULL)
	{
		if(((prog_win_extra_info *)vtwin->extra_info)->action_type==1 && ((FILELIST *)llp->node)->on_off==0)
		{
			llp2=vtwin->entries;

			while(llp2!=NULL)
			{
				((FILELIST *)llp2->node)->on_off=0;
				llp2=llp2->right;
			}
		}

		if(((FILELIST *)llp->node)->on_off==1)
			((FILELIST *)llp->node)->on_off=0;
		else
			((FILELIST *)llp->node)->on_off=1;

		if(((prog_win_extra_info *)vtwin->extra_info)->action_type==0)
#ifdef MEM_PRINT
		{
			print_specific_win_contents(vtwin,sp);
			print_specific_win_contents(vtwin,NULL);
		}
#else
			print_specific_win_contents(vtwin);
#endif

	}

	if(((prog_win_extra_info *)vtwin->extra_info)->action_type==1)
#ifdef MEM_PRINT
		print_prog_win_contents(vtwin,sp,NULL);
#else
		print_prog_win_contents(vtwin,NULL);
#endif
*/
	
}

void save_windows_to_set( VT52_WINDOW *vtwin )
{
	LinkedList *curr_list;

	if(curr_set!=NULL)
	{
		curr_list=circular_windows_list;
		while(curr_list->node!=vtwin)
			curr_list=curr_list->right;
		curr_list=curr_list->right;

		do{
				set_set_details_from_window( curr_set, curr_list->node);
				curr_list=curr_list->right;
			}while(curr_list!=NULL && curr_list->node!=vtwin);
	}
}

void set_curr_set_from_window( VT52_WINDOW *vtwin)
{
	LinkedList *pset;
	SET *cset;
	int count;


	pset=vtwin->entries;
	count=1;
	if(pset!=NULL)
	{
		count=1;
		while(count!=vtwin->logical_cursor)
		{
			pset=pset->right;
			count++;
		}
		cset=(SET *)pset->node;
		curr_set=cset;
	}
}

LinkedList *set_windows_from_set(VT52_WINDOW *vtwin)
{
	LinkedList *curr_list;

	if(vtwin->entries!=NULL)
	{
		curr_list=circular_windows_list;	/* find the sets window in the list */
		while(curr_list->node!=vtwin)
			curr_list=curr_list->right;

		curr_list=curr_list->right;		/* bump past it */

		/*loop around the windows setting the details to the set */
		do{
			set_window_on_off_from_set( curr_set, curr_list->node);
			curr_list=curr_list->right;
		}while(curr_list!=NULL && curr_list->node!=vtwin);
		/* until we hit the end of the list of windows or we hit the sets
			window */
	}
	return curr_list;
}

long sets_win_other_keys( VT52_WINDOW *vtwin, long k
#ifdef MEM_PRINT
,screen *sp
#endif
)
{
	int u,l,s;
	int temp_int,temp_int2;
	LinkedList *curr_list;
	SET *old_curr_set;
	
	u=(int)(k>>16)&0xff;
	l=(int)(k%256);
	s=(int)(k>>24)&0x0f;	/* we are only interested in Shift, Cntrl and Alt see page 306 for details */

	curr_list=circular_windows_list;

	if(u==57)	/* space */
	{
		if(curr_list!=NULL)
		{

				save_windows_to_set(vtwin);
				set_curr_set_from_window(vtwin);
				curr_list=set_windows_from_set(vtwin);

				print_ordered_windows(curr_list,vtwin,sp);
				print_area(sp,0,0,sp->width-1, sp->height-1);

		}

		return -1;
	}
	else
	{
		if(u==82)		/* insert */
		{

				save_windows_to_set(vtwin);

			if(s==2)
				old_curr_set=curr_set;
			else
				old_curr_set=NULL;

				new_set_key(vtwin,sp,old_curr_set);
				curr_list=set_windows_from_set(vtwin);

				print_ordered_windows(curr_list,vtwin,sp);
				print_area(sp,0,0,sp->width-1, sp->height-1);
				return -1;
			
		}
		else
		{
			if(u==0x1C)	/* enter */
			{
				edit_set(vtwin,sp);
				return -1;
			}
			else
			{
				if(u==83) /* delete */
				{

					save_windows_to_set(vtwin);
					delete_set(vtwin,sp);
					curr_list=set_windows_from_set(vtwin);

					print_ordered_windows(curr_list,vtwin,sp);
					print_area(sp,0,0,sp->width-1, sp->height-1);
					return -1;
					
				}
				else
				{
					if((u>=59 && u<=68 && (s==0 || s==4 || s==8))
						|| (u>=84 && u<=94 && s==2))
					{
							if(u>=59 && u<=68)
								temp_int=u-58;
							else
								temp_int=u-83;

							switch(s)
							{
							case 0:
								temp_int2=0;
								break;
							case 2:
								temp_int2=10;
								break;
							case 4:
								temp_int2=20;
								break;
							case 8:
								temp_int2=30;
								break;
							}

							temp_int=temp_int+temp_int2;

							if(temp_int<=vtwin->total_entries)
							{
							/*if we reset then do it now otherwise */
							save_windows_to_set(vtwin);
							vtwin->logical_cursor=temp_int;
							set_curr_set_from_window(vtwin);
							curr_list=set_windows_from_set(vtwin);

							if(vtwin->logical_cursor>vtwin->height+vtwin->first_entry_on_screen-1
								|| vtwin->logical_cursor<vtwin->first_entry_on_screen)
							{
								vtwin->first_entry_on_screen=vtwin->logical_cursor;
								vtwin->physical_cursor=1;
							}
							else
								vtwin->physical_cursor=vtwin->logical_cursor-vtwin->first_entry_on_screen+1;

							print_ordered_windows(curr_list,vtwin,sp);
							print_area(sp,0,0,sp->width-1, sp->height-1);
							}
							return -1;

					}
					else	
						return k;
				}
			}
		}
	}
}


void other_win_bottom_title( VT52_WINDOW *vtwin
#ifdef MEM_PRINT
, screen *sp
#endif
)
{
	
	char temp[50];

#ifndef MEM_PRINT
	DISCARD_EOL
#endif

	sprintf(	temp,
				"CURSOR:[^v<>] PAGE: s[^v<>] TOGGLE: SPACE");

#ifdef MEM_PRINT
	screen_at(sp,auto_details.rows-2,0);
	set_reverse_mode(sp);
	centre_text(temp,line_buffer,auto_details.columns,' ');
	print_string(sp,line_buffer);
#else
	AT(auto_details.rows-2,0)
	REVERSE_VIDEO
	centre_text(temp,line_buffer,auto_details.columns,' ');
	printf("%s",line_buffer);
#endif

	if(auto_details.columns<=40)
		sprintf( temp,
				"SWITCH: TAB,HELP: HELP,OPTNS: aO,BYE: ESC");
	else	
		sprintf( temp,
				"SWITCH: TAB, HELP: HELP, OPTIONS: aO, EXIT: ESC");

#ifdef MEM_PRINT
	screen_at(sp,auto_details.rows-1,0);
	set_reverse_mode(sp);
	centre_text(temp,line_buffer,prog_defaults.columns,' ');
	print_string(sp,line_buffer);
	set_normal_mode(sp);
#else
	AT(auto_details.rows-1,0)
	REVERSE_VIDEO
	centre_text(temp,line_buffer,prog_defaults.columns,' ');
	printf("%s",line_buffer);
	NORMAL_VIDEO
	WRAP_EOL
#endif

}

VT52_WINDOW *create_other_win( void )
{

	VT52_WINDOW *win;
	char *name;

	win=create_vt52window();
	name=(char *)malloc(7);

	strcpy(name,"OTHERS");
	win->name=name;

	win->type=3;
	win->can_resize_horizontal=0;
	win->can_resize_vertical=1;
	win->chars_per_col=auto_details.columns-4;

	win->extra_info=NULL /*create_prog_win_extra_info()*/;
	win->print=print_other_win_contents;
	win->other_keys=other_win_other_keys;
	win->print_bottom=other_win_bottom_title;
	win->focus=other_win_in_focus;
	win->unfocus=other_win_out_focus;

	return win;
}

void other_win_in_focus(VT52_WINDOW *vtwin)
{
	vtwin->on_top=1;
}

void other_win_out_focus(VT52_WINDOW *vtwin)
{
	vtwin->on_top=0;
}

void print_other_win_contents( VT52_WINDOW *vtwin, screen *sp, int *area )
{
	LinkedList *llp;
	OTHER_FILES *temp;
	int loop,loop2;
	char *half_buffer;
	int half_size;


	half_size=((auto_details.columns-6)/2)+1;
	half_buffer=(char *)malloc(half_size);
	/*DISCARD_EOL*/

/*	temp=others;*/
	llp=vtwin->entries;


	loop=1; 
	while(llp!=NULL && loop<vtwin->first_entry_on_screen)
	{
		loop++;
		llp=llp->right;
	}

	

	for(loop=0;loop<vtwin->height;loop++)
	{

		if(llp!=NULL)
		{


			if(sp==NULL)
			{
				AT(vtwin->ypos+1+loop, 2)
				for(loop2=2;loop2<auto_details.columns;loop2++)
					Bconout(2,' ');
			}
			else
			{
				screen_at(sp,vtwin->ypos+1+loop, 2);
				for(loop2=2;loop2<auto_details.columns;loop2++)
					print_char(sp,' ');
			}

/*				CLEAR_EOL*/

/*				AT(other_window_first_item_row+loop, prog_defaults.columns-1);
				Bconout(2,'+');
*/
			if(sp==NULL)
			{
				AT(vtwin->ypos+1+loop,auto_details.columns-1)
				if(vtwin->on_top)
					Bconout(2,'|');
				else
					Bconout(2,'!');
			}
			else
			{
				screen_at(sp,vtwin->ypos+1+loop,auto_details.columns-1);	
				if(vtwin->on_top)
					print_char(sp,'|');
				else
					print_char(sp,'!');
			}

				temp=llp->node;

				if(temp->to!=NULL && half_buffer!=NULL)
				{
					loop2=0;
					while(loop2<half_size && temp->to[loop2]!='\0')
					{
						half_buffer[loop2]=temp->to[loop2];
						loop2++;
					}
					half_buffer[loop2]='\0';

					if(sp==NULL)
					{
						AT(vtwin->ypos+1+loop, 2)
						printf("%s",half_buffer);
					}
					else
					{
						screen_at(sp,vtwin->ypos+1+loop, 2);
						print_string(sp,half_buffer);
					}
				}

				if(temp->from!=NULL && half_buffer!=NULL)
				{
					loop2=0;
					while(loop2<half_size && temp->from[loop2]!='\0')
					{
						half_buffer[loop2]=temp->from[loop2];
						loop2++;
					}
					half_buffer[loop2]='\0';
				
					if(sp==NULL)
					{
						AT(vtwin->ypos+1+loop, 2+half_size+1)
						printf("%s",half_buffer);
					}
					else
					{
						screen_at(sp,vtwin->ypos+1+loop, 2+half_size+1);
						print_string(sp,half_buffer);
					}
				}

			llp=llp->right;
		}
		else
		{
			if(sp==NULL)
			{
				AT(vtwin->ypos+1+loop, 2)
				for(loop2=2;loop2<auto_details.columns;loop2++)
					Bconout(2,' ');
			}
			else
			{
				screen_at(sp,vtwin->ypos+1+loop, 2);
				for(loop2=2;loop2<auto_details.columns;loop2++)
					print_char(sp,' ');
			}



			if(sp==NULL)
			{
				AT(vtwin->ypos+1+loop,auto_details.columns-1)
				if(vtwin->on_top)
					Bconout(2,'|');
				else
					Bconout(2,'!');
			}
			else
			{
				screen_at(sp,vtwin->ypos+1+loop,auto_details.columns-1);	
				if(vtwin->on_top)
					print_char(sp,'|');
				else
					print_char(sp,'!');
			}
		}
	}

/*	WRAP_EOL*/
}



long other_win_other_keys( VT52_WINDOW *vtwin, long k
#ifdef MEM_PRINT
,screen *sp
#endif
)
{
	int u,l,s;
	LinkedList *curr_list;
	
	u=(int)(k>>16)&0xff;
	l=(int)(k%256);
	s=(int)(k>>24)&0x0f;	/* we are only interested in Shift, Cntrl and Alt see page 306 for details */

	curr_list=circular_windows_list;


/*	if(u==57)	/* space */
	{
		if(curr_list!=NULL)
		{

				save_windows_to_set(vtwin);
				set_curr_set_from_window(vtwin);
				curr_list=set_windows_from_set(vtwin);

				print_ordered_windows(curr_list,vtwin,sp);
				print_area(sp,0,0,sp->width-1, sp->height-1);

		}

		return -1;
	}
	else
	{*/
		if(u==82)		/* insert */
		{

/*				save_windows_to_set(vtwin);*/
				new_other_key(vtwin,sp);
/*				curr_list=set_windows_from_set(vtwin);

				print_ordered_windows(curr_list,vtwin,sp);
				print_area(sp,0,0,sp->width-1, sp->height-1);
*/			
		}
		else
		{
			if(u==0x1C)	/* enter */
			{
				edit_other(vtwin,sp);
			}
			else
			{
				if(u==83) /* delete */
				{

/*					save_windows_to_set(vtwin);*/
					delete_other(vtwin,sp);
/*					curr_list=set_windows_from_set(vtwin);

					print_ordered_windows(curr_list,vtwin,sp);
					print_area(sp,0,0,sp->width-1, sp->height-1);
*/					
				}
				else
					return k;
			}
		}
/*	}*/


	return k;
}

/*
void new_other_window_key( VT52_WINDOW *vtwin, screen *sp)
{
	int half_size;
	OTHER_FILES *temp;

	temp=create_other();
	half_size=((auto_details.columns-6)/2)+1;

	if(temp!=NULL)
	{
		vtwin->total_entries++;
		vtwin->logical_cursor=vtwin->total_entries;
		vtwin->old_cursor=vtwin->physical_cursor;
		curr_other=temp;

		if(vtwin->logical_cursor!=1)  /* no sets yet */
		{
			if(vtwin->total_entries>vtwin->height)	/* not just 1 screen */
			{
					/* not on this screen */
				if((vtwin->total_entries-vtwin->first_entry_on_screen+1)>vtwin->height)
					{
						if(vtwin->paging==SCROLLING)
						{
							vtwin->first_entry_on_screen=vtwin->total_entries-vtwin->height+1;
							vtwin->physical_cursor=vtwin->height;
						}
						else
						{
							vtwin->first_entry_on_screen=vtwin->total_entries;
							vtwin->physical_cursor=1;
						}
					}
				else
					{		
						vtwin->physical_cursor=vtwin->total_entries-vtwin->first_entry_on_screen+1;
					}
			}
			else
			{
				vtwin->first_entry_on_screen=1;
				vtwin->physical_cursor=vtwin->total_entries;
			}
		}
		else
		{
			curr_other=temp;
			vtwin->old_cursor=1;
			vtwin->first_entry_on_screen=1;
			vtwin->logical_cursor=1;
			vtwin->physical_cursor=1;
			vtwin->total_entries=1;
		}

		print_window_frame(vtwin,1, temp_screen,NULL);
		print_window_contents(vtwin, temp_screen,NULL);
		remove_cursor(vtwin,vtwin->old_cursor, temp_screen,NULL);
		draw_cursor(vtwin,vtwin->physical_cursor, temp_screen,NULL);
		vtwin->entries=append_LinkedList(vtwin->entries,temp);

/*		print_others_in_window( others );
		print_others_title(logical_other_cursor);
		remove_cursor(current_window,old_other_cursor);	
		draw_cursor(current_window,other_cursor);
*/
		temp->to=(char *)malloc(FILENAME_MAX+1);
		temp->from=(char *)malloc(FILENAME_MAX+1);
		temp->to[0]='\0';
		temp->from[0]='\0';

		edit_at(vtwin->ypos+1+vtwin->physical_cursor-1,
					2,
					FILENAME_MAX,
					half_size,
					temp->to,
					'-',
					0,
					valid_to_char,
					0);	

		edit_at(vtwin->ypos+1+vtwin->physical_cursor-1,
					half_size+2+1,
					FILENAME_MAX,
					half_size,
					temp->from,
					'-',
					0,
					valid_from_char,
					0);	

/*		if(temp->from[0]=='\0' || temp->to[0]=='\0')
			others=delete_other(others,logical_other_cursor);
*/

		print_window_frame(vtwin,1, temp_screen,NULL);
		print_window_contents(vtwin, temp_screen,NULL);
		remove_cursor(vtwin,vtwin->old_cursor, temp_screen,NULL);
		draw_cursor(vtwin,vtwin->physical_cursor, temp_screen,NULL);
/*		other_bottom_title();
		print_others_in_window( others );
		AT(other_window_first_item_row+other_cursor-1,prog_defaults.columns-2)
		Bconout(2,'<');
		return(others);*/
	}
/*	else
		return NULL;*/
}
*/


LinkedList *set_default_windows(char *curr_path, SET *curr_set, LinkedList *sets)
{

	VT52_WINDOW *temp_win;
	char drive_only[4];
	char temp_ext[15];
	LinkedList *circular_windows,*pLinkedList;
	char *pch;
	

	drive_only[0]=curr_path[0];
	drive_only[1]=curr_path[1];
	drive_only[2]=curr_path[2];
	drive_only[3]='\0';
	
	circular_windows=NULL;

	temp_win=create_prog_win();	/*create_vt52window();*/
	pch=(char *)malloc(strlen("AUTO")+1);
	strcpy(pch,"AUTO");
	temp_win->name=pch;	
	strcpy(((prog_win_extra_info *)temp_win->extra_info)->good_extension,"PRG");
	strcpy(((prog_win_extra_info *)temp_win->extra_info)->bad_extension,"PRX");
	strcpy(((prog_win_extra_info *)temp_win->extra_info)->search_extension,"PR?");
/*	strcpy(((prog_win_extra_info *)temp_win->extra_info)->from_path,"C:\\AUTO\\");*/
	sprintf(((prog_win_extra_info *)temp_win->extra_info)->from_path,"%sAUTO\\",drive_only);
	((prog_win_extra_info *)temp_win->extra_info)->action_type=0;

	strcpy(temp_ext,"*.");
	strcat(temp_ext,((prog_win_extra_info *)temp_win->extra_info)->search_extension);
	temp_win->entries = build_list(((prog_win_extra_info *)temp_win->extra_info)->from_path,temp_ext,0);

	temp_win->height=6;
	temp_win->columns=1;
	temp_win->xpos=3;
	temp_win->ypos=1;
	temp_win->on_top=0;

	circular_windows=append_CircularList(circular_windows,temp_win);

	temp_win=create_prog_win();	/*create_vt52window();*/
	pch=(char *)malloc(strlen("ACCS")+1);
	strcpy(pch,"ACCS");
	temp_win->name=pch;	
	strcpy(((prog_win_extra_info *)temp_win->extra_info)->good_extension,"ACC");
	strcpy(((prog_win_extra_info *)temp_win->extra_info)->bad_extension,"ACX");
	strcpy(((prog_win_extra_info *)temp_win->extra_info)->search_extension,"AC?");
/*	strcpy(((prog_win_extra_info *)temp_win->extra_info)->from_path,"C:\\");*/
	sprintf(((prog_win_extra_info *)temp_win->extra_info)->from_path,"%s",drive_only);
	((prog_win_extra_info *)temp_win->extra_info)->action_type=0;

	strcpy(temp_ext,"*.");
	strcat(temp_ext,((prog_win_extra_info *)temp_win->extra_info)->search_extension);
	temp_win->entries=build_list(((prog_win_extra_info *)temp_win->extra_info)->from_path,temp_ext,0);

	temp_win->height=5;
	temp_win->columns=1;
	temp_win->xpos=3;
	temp_win->ypos=10;

	circular_windows=append_CircularList(circular_windows,temp_win);

	temp_win=create_sets_win();	/*create_vt52window();*/
	pch=(char *)malloc(strlen("SETS")+1);
	strcpy(pch,"SETS");
	temp_win->name=pch;	
	temp_win->on_top=1;
	temp_win->entries=sets;

	temp_win->height=13;
	temp_win->columns=1;
	temp_win->xpos=15;
	temp_win->ypos=1;
	circular_windows=append_CircularList(circular_windows,temp_win);

	temp_win=create_other_win();	/*create_vt52window();*/
	temp_win->height=4;
	temp_win->xpos=0;
	temp_win->ypos=17;
	circular_windows=append_CircularList(circular_windows,temp_win);


	pLinkedList=circular_windows;

	do{
		temp_win=(VT52_WINDOW *)pLinkedList->node;	
		set_window_on_off_from_set(curr_set,temp_win);
		set_cursors( temp_win );
		pLinkedList=pLinkedList->right;
		}while(pLinkedList!=circular_windows);

	return circular_windows;		
}