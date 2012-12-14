#include "vtwindow.h"
#include "vt52.h"
#include "screen.h"
/*#include <vdi.h>*/

#define MEM_PRINT


/*#define GRAPHIC_WINDOW*/

#ifdef GRAPHIC_WINDOW
#include "autofold.h"
#endif

short dirty_rectangle1[4];
short dirty_rectangle2[4];


char temp_name[]="TEMP";
void null_function( VT52_WINDOW *vtwin)
{
	;
}

long long_function( VT52_WINDOW *vtwin)
{
	return 0;
}

VT52_WINDOW *create_vt52window( void )
{
	VT52_WINDOW *tmp;

	if((tmp=(VT52_WINDOW *) malloc(sizeof(VT52_WINDOW)))==NULL)
	{	
		fprintf(stderr,"Could not allocate space for VT52 Window!!\n");
		exit(1);
	}

	tmp->first_entry_on_screen=0;
	tmp->logical_cursor=0;
	tmp->physical_cursor=0;
	tmp->old_cursor=0;
	tmp->on_top=0;
	tmp->name=temp_name;				
	tmp->height=1;			
	tmp->columns=1;			
	tmp->chars_per_col=16;	
	tmp->titles_at_top=1;	
	tmp->name_at_top=1;	
	tmp->titles_on_off=1;	
	tmp->paging=1;		
	tmp->xpos=0;		
	tmp->ypos=0;		
	tmp->total_entries=0;
	tmp->type=0;		
	tmp->entries=NULL;

	tmp->can_resize_horizontal=1;
	tmp->can_resize_vertical=1;
	tmp->is_moveable=1;
	tmp->is_scrollable_left=1;
	tmp->is_scrollable_up=1;
	
	tmp->help=null_function;		/* called when help key pressed */		
/*	tmp->toggle=null_function;*/		/* called when toggle key pressed */
/*	tmp->select=null_function;*/		/* called when select key pressed */
	tmp->part_print=null_function;	/* called to print part */
	tmp->print=null_function;		/* called when printing window */
	tmp->focus=null_function;		/* called when window is in focus */
	tmp->unfocus=null_function;	/* called when window goes out of focus */
	tmp->other_keys=long_function;	/* called if unkown key is pressed */

	tmp->extra_info=NULL;
	tmp->max_selectable=0;
	tmp->no_key_help1=NULL;
	tmp->no_key_help2=NULL;
	tmp->shift_key_help1=NULL;
	tmp->shift_key_help2=NULL;
	tmp->cntrl_key_help1=NULL;
	tmp->cntrl_key_help2=NULL;
	tmp->alt_key_help1=NULL;
	tmp->alt_key_help2=NULL;
	tmp->alt_shift_key_help1=NULL;
	tmp->alt_shift_key_help2=NULL;
	tmp->cntrl_shift_key_help1=NULL;
	tmp->cntrl_shift_key_help2=NULL;

	return tmp;
}

void delete_vt52window( VT52_WINDOW *vtwin )
{
/*
	delete_LinkedList(vtwin->entries, function);
*/
	if(vtwin->extra_info)
		free(vtwin->extra_info);

	free(vtwin);
}

void print_window_numbers( VT52_WINDOW *vtwin 
#ifdef MEM_PRINT
, screen *sp
#endif
,int *area
)
{
	int x1,y1,x2,y2;
	char number[10];
	int print_flag;

	if(area==NULL)
		print_flag=1;
	else
		print_flag=0;

	if(vtwin->titles_on_off==1)
	{
		x1=	vtwin->xpos+1;

		sprintf(number,"%d",vtwin->total_entries);
		x2=	vtwin->xpos+1+
				(vtwin->columns*vtwin->chars_per_col)+
				vtwin->columns+1-
				strlen(number);

		if(vtwin->titles_at_top==1)
		{
			if(area!=NULL && (area[1]<=vtwin->ypos && area[3]>=vtwin->ypos))
				print_flag=1;
				
			y1=	vtwin->ypos;
			y2=	y1;	
		}
		else
		{		
			if(area!=NULL && (area[1]<=vtwin->ypos && area[3]>=vtwin->ypos+vtwin->height+1))
				print_flag=1;

			y1=	vtwin->ypos+vtwin->height+1;
			y2=	y1;
		}

#ifdef MEM_PRINT
	if(sp!=NULL)
	{
		if(print_flag)
		{
			screen_at(sp,y1,x1);
			print_int(sp,vtwin->logical_cursor);
			screen_at(sp,y2,x2);
			print_int(sp,vtwin->total_entries);
		}
	}
	else
	{
		if(print_flag)
		{
			AT(y1,x1)
			printf("%d",vtwin->logical_cursor);
			AT(y2,x2)
			printf("%d",vtwin->total_entries);
		}
	}
#else
	if(print_flag)
	{
		AT(y1,x1)
		printf("%d",vtwin->logical_cursor);
		AT(y2,x2)
		printf("%d",vtwin->total_entries);
	}
#endif
	}

}			

void print_window_name( VT52_WINDOW *vtwin 
#ifdef MEM_PRINT
, screen *sp
#endif
, int *area
)
{
	int x1,y1;
	
if(area==NULL || (area[1]<=vtwin->ypos && area[3]>=vtwin->ypos))
{

	if(vtwin->name!=NULL)
	{

		x1=	((((vtwin->chars_per_col*vtwin->columns)+vtwin->columns+1)/2)-
				(strlen(vtwin->name)/2))+
				vtwin->xpos+1;
		y1=	vtwin->ypos;

		if(vtwin->on_top)
		{
#ifdef MEM_PRINT
		if(sp!=NULL)
		{
			screen_at(sp,y1,x1);
			set_reverse_mode(sp);
			print_string(sp,vtwin->name);
			set_normal_mode(sp);
		}
		else
		{
			AT(y1,x1)
			REVERSE_VIDEO
			printf("%s",vtwin->name);
			NORMAL_VIDEO
		}
#else
			AT(y1,x1)
			REVERSE_VIDEO
			printf("%s",vtwin->name);
			NORMAL_VIDEO
#endif
		}
		else
		{
#ifdef MEM_PRINT
		if(sp!=NULL)
		{
			screen_at(sp,y1,x1);
			print_string(sp,vtwin->name);
		}
		else
		{
			AT(y1,x1)
			printf("%s",vtwin->name);
		}
#else
			AT(y1,x1)
			printf("%s",vtwin->name);
#endif
		}
	}
}
}

void print_window_title_bar( VT52_WINDOW *vtwin 
#ifdef MEM_PRINT
, screen *sp
#endif
,int *area
)
{
	print_window_bar( vtwin, 1
#ifdef MEM_PRINT
, sp
, area
);
#else
,area);
#endif
}

void print_window_bottom_bar( VT52_WINDOW *vtwin
#ifdef MEM_PRINT
, screen *sp
#endif
,int *area
)
{
	print_window_bar( vtwin, 0
#ifdef MEM_PRINT
, sp, area);
#else
,area);
#endif

}

void print_window_bar( VT52_WINDOW *vtwin, int top 
#ifdef MEM_PRINT
, screen *sp
#endif
, int *area
)
{
	int x1,y1;
	int loop,loop_end;

	if(top)
	{
		y1=vtwin->ypos;
	}
	else
	{
		y1=vtwin->ypos+vtwin->height+1;
	}

if(area==NULL || (area[1]<=y1 && area[3]>=y1))
{
	x1=vtwin->xpos;

#ifdef MEM_PRINT
	if(sp!=NULL)
	{
	
	screen_at(sp,y1,x1);
	print_char(sp,'+');
	}
	else
	{
	DISCARD_EOL
	AT(y1,x1)
	Bconout(2,'+');
	}
#else
	DISCARD_EOL
	AT(y1,x1)
	Bconout(2,'+');
#endif
	loop_end=(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+1;

	for(loop=0;loop<loop_end;loop++)
	{
		if(vtwin->on_top)
#ifdef MEM_PRINT
	if(sp!=NULL)
	{	
		print_char(sp,'=');
	}
	else
	{
			Bconout(2,'=');
	}
#else
			Bconout(2,'=');
#endif
		else
#ifdef MEM_PRINT
	if(sp!=NULL)
	{
			print_char(sp,'-');
	}
	else
	{
			Bconout(2,'-');
	}
#else
			Bconout(2,'-');
#endif

	}

#ifdef MEM_PRINT
	if(sp!=NULL)
	{
			print_char(sp,'+');
	}
	else
	{
			Bconout(2,'+');
			WRAP_EOL
	}
#else
			Bconout(2,'+');
			WRAP_EOL
#endif
}
}

void print_window_sides( VT52_WINDOW *vtwin , int clear_contents
#ifdef MEM_PRINT
, screen *sp
#endif
, int *area
)
{
	int x1,y1,loop1,loop2,loop_end2;

	loop_end2=(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+1;
	x1=vtwin->xpos;
	y1=vtwin->ypos+1;

	if(area==NULL || (area[1]<=vtwin->ypos && area[3]>=vtwin->ypos))
	{ 	

		for(loop1=0;loop1<vtwin->height;loop1++)
		{
			#ifndef MEM_PRINT
			DISCARD_EOL
			AT(y1+loop1,x1)
		
			if(area==NULL || (area[1]<=y1+loop1 && area[3]>=y1+loop1))
			{
				if(vtwin->on_top)
				Bconout(2,'|');
			else
				Bconout(2,'!');
			#endif

			#ifdef MEM_PRINT
			if(sp!=NULL)
			{
				screen_at(sp,y1+loop1,x1);
				if(vtwin->on_top)
					print_char(sp,'|');
				else
					print_char(sp,'!');
			}
			else
			{
				DISCARD_EOL
				AT(y1+loop1,x1)
		
				if(vtwin->on_top)
					Bconout(2,'|');
				else
					Bconout(2,'!');
			}
			#endif

			if(clear_contents)
			{
				for(loop2=0;loop2<loop_end2;loop2++)
				{
				#ifdef MEM_PRINT
						if(sp!=NULL)
							print_char(sp,' ');
						else
							Bconout(2,' ');
				#else
							Bconout(2,' ');
				#endif
				}
			}
	

			#ifdef MEM_PRINT
			if(sp!=NULL)
			{
		
				screen_at(sp,y1+loop1,x1+loop_end2+1);
		
				if(vtwin->on_top)
					print_char(sp,'|');
				else
					print_char(sp,'!');
			}
			else
			{
				AT(y1+loop1,x1+loop_end2+1)

				if(vtwin->on_top)
					Bconout(2,'|');
				else
					Bconout(2,'!');
	
				WRAP_EOL
			}
			#else
				AT(y1+loop1,x1+loop_end2+1)

				if(vtwin->on_top)
					Bconout(2,'|');
				else
					Bconout(2,'!');
	
				WRAP_EOL
			#endif
		}
	}
}


void print_window_frame( VT52_WINDOW *vtwin, int clear_contents 
#ifdef MEM_PRINT
, screen *sp
#endif
,int *area
)
{

#ifdef GRAPHIC_WINDOW
	short pxy_array[10];
#endif

#ifndef GRAPHIC_WINDOW
/*AT(20,20)
printf("test1");
			while(!Bconstat(2));
			Bconin(2);
*/	

	print_window_title_bar(vtwin
#ifdef MEM_PRINT
, sp
#endif
,area);

/*AT(20,20)
printf("test2");
			while(!Bconstat(2));
			Bconin(2);
*/	
	print_window_sides(vtwin, clear_contents
#ifdef MEM_PRINT
, sp
#endif
,area);

/*AT(20,20)
printf("test3");
			while(!Bconstat(2));
			Bconin(2);
*/	
	print_window_bottom_bar(vtwin
#ifdef MEM_PRINT
, sp
#endif
,area);

/*AT(20,20)
printf("test4");
			while(!Bconstat(2));
			Bconin(2);
*/
#else
/*x1*/	pxy_array[0]=((vtwin->xpos*auto_details.char_pixel_width)-(auto_details.char_pixel_width/2))-2;
/*y1*/	pxy_array[1]=((vtwin->ypos*auto_details.char_pixel_height)-(auto_details.char_pixel_height/2))-2;
/*x2*/	pxy_array[2]=(((vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns)+1)*auto_details.char_pixel_width)-(auto_details.char_pixel_width/2)+2;
/*y1*/	pxy_array[3]=pxy_array[1];
/*x2*/	pxy_array[4]=pxy_array[2];
/*y2*/	pxy_array[5]=((vtwin->ypos+vtwin->height+1)*auto_details.char_pixel_height)-(auto_details.char_pixel_height/2)+2;
/*x1*/	pxy_array[6]=pxy_array[0];
/*y2*/	pxy_array[7]=pxy_array[5];
/*x1*/	pxy_array[8]=pxy_array[0];
/*y1*/	pxy_array[9]=pxy_array[1];

/*	printf("%d, %d, %d, %d\n",pxy_array[0],pxy_array[1],pxy_array[2],pxy_array[3]);
	v_bar(auto_details.handle,pxy_array);
*/
	v_pline(auto_details.handle,10,pxy_array);
#endif	
	print_window_numbers(vtwin
#ifdef MEM_PRINT
, sp
#endif
,area);
/*AT(20,20)
printf("test5");
			while(!Bconstat(2));
			Bconin(2);
*/	
	print_window_name(vtwin
#ifdef MEM_PRINT
, sp
#endif
,area);
/*AT(20,20)
printf("test6");
			while(!Bconstat(2));
			Bconin(2);
*/
}

void print_window_contents( VT52_WINDOW *vtwin 
#ifdef MEM_PRINT
,screen *sp
#endif
,int *area)
{
	if(vtwin->print)
	{
		(vtwin->print)(vtwin
#ifdef MEM_PRINT
,sp
#endif
,area);
	}
}

void print_window_bottom_help( VT52_WINDOW *vtwin
#ifdef MEM_PRINT
, screen *sp
#endif
)
{
	if(vtwin->print_bottom)
	{
		(vtwin->print_bottom)(vtwin
#ifdef MEM_PRINT
,sp
#endif
);
	}
}

void print_cursor( VT52_WINDOW *vtwin, char l, char r, int cursor
#ifdef MEM_PRINT
, screen *sp
#endif
, int *area)
{
	int phys_curs;
	int phys_col;
	int y;
	int x;

	phys_col=(cursor-1)/vtwin->height;
	phys_curs=(cursor-1)%vtwin->height;
	y=phys_curs+vtwin->ypos+1;
	x=vtwin->xpos+1+(phys_col*9);

#ifdef MEM_PRINT
if(sp!=NULL)
{
	if(area==NULL || (area[1]<=y && area[3]>=y))
	{
	screen_at(sp,y,x);
	print_char(sp,l);

	screen_at(sp,y,x+vtwin->chars_per_col+1);
	print_char(sp,r);

	print_area(sp,x,y,x,y);
	print_area(sp,x+vtwin->chars_per_col+1,y,x+vtwin->chars_per_col+1,y);
	}
}
else
{
	if(area==NULL || (area[1]<=y && area[3]>=y))
	{
	AT(y,x)
	Bconout(2,l);

	AT(y,x+vtwin->chars_per_col+1)
	Bconout(2,r);
	}
}
#else
	if(area==NULL || (area[1]<=y && area[3]>=y))
	{

	AT(y,x)
	Bconout(2,l);

	AT(y,x+vtwin->chars_per_col+1)
	Bconout(2,r);
	}
#endif
}

void remove_cursor(VT52_WINDOW *window, int cursor
#ifdef MEM_PRINT
, screen *sp
#endif
, int *area
)
{
	if(cursor!=0)
		print_cursor(window,' ',' ',cursor
#ifdef MEM_PRINT
,sp
#endif
, area);
}

void draw_cursor(VT52_WINDOW *window, int cursor
#ifdef MEM_PRINT
, screen *sp
#endif
, int *area
)
{
	if(cursor!=0)
		print_cursor(window,'>','<', cursor
#ifdef MEM_PRINT
,sp
#endif
, area);
}


void set_total_entries( VT52_WINDOW *vtwin )
{
	LinkedList *llp;
	int count;

	llp=vtwin->entries;
	count=0;

	while(llp!=NULL)
	{
		count++;
		llp=llp->right;
	}

	vtwin->total_entries=count;
}

void set_cursors( VT52_WINDOW *vtwin)
{

	set_total_entries(vtwin);

	if(vtwin->total_entries>0)
	{
		vtwin->first_entry_on_screen=1;
		vtwin->logical_cursor=1;				/*position in list of entires*/
		vtwin->physical_cursor=1;				/* position on screen */
		vtwin->old_cursor=1;
	}
}

void vt52window_up_arrow( VT52_WINDOW *vtwin
#ifdef MEM_PRINT
, screen *sp
#endif
)
{
		int num_in_window;

		num_in_window=vtwin->height*vtwin->columns;		


/* at first entry */
/* at middle entry in middle */
/* at middle entry at top of window */


	if(vtwin->logical_cursor<=1) /* at first entry */
					BELL
	else
	{
		if(vtwin->physical_cursor<=1 && vtwin->is_scrollable_up==1) /* need to page and can */
		{
			vtwin->old_cursor=vtwin->physical_cursor;
			vtwin->physical_cursor=1;


				if(vtwin->paging==0)
				{
					vtwin->logical_cursor--;
					vtwin->first_entry_on_screen--;
				}
				else
				{
					vtwin->first_entry_on_screen-=num_in_window;
					vtwin->logical_cursor--;
					if(vtwin->logical_cursor<num_in_window)
						vtwin->physical_cursor=vtwin->logical_cursor;
					else
						vtwin->physical_cursor=num_in_window;
				}

				if(vtwin->first_entry_on_screen<1)
					vtwin->first_entry_on_screen=1;
				if(vtwin->logical_cursor<1)
					vtwin->logical_cursor=1;

			print_window_contents(vtwin
#ifdef MEM_PRINT
,sp
#endif
, NULL);
#ifdef MEM_PRINT
	print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#endif

		}
		else
		{
			if(vtwin->physical_cursor<=1) /* need to page and can't */
			{
				BELL
				vtwin->physical_cursor=1;
			}
			else
			{
				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->physical_cursor--;
				vtwin->logical_cursor--;
			}				
		}

#ifdef MEM_PRINT
		print_window_title_bar(vtwin,sp, NULL);
		print_window_title_bar(vtwin,NULL, NULL);
		print_window_name(vtwin,sp, NULL);
		print_window_name(vtwin,NULL, NULL);
		print_window_numbers(vtwin,sp, NULL);
		print_window_numbers(vtwin,NULL, NULL);
		remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
/*		remove_cursor(vtwin,vtwin->old_cursor,NULL, NULL);
*/		draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
/*		draw_cursor(vtwin,vtwin->physical_cursor,NULL, NULL);
*/
#else
		print_window_title_bar(vtwin, NULL);
		print_window_name(vtwin, NULL);
		print_window_numbers(vtwin, NULL);
		remove_cursor(vtwin,vtwin->old_cursor, NULL);
		draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif


	}

}

void vt52window_down_arrow( VT52_WINDOW *vtwin
#ifdef MEM_PRINT
, screen *sp
#endif
)
{
		int num_in_window;

		num_in_window=vtwin->height*vtwin->columns;		

/* last entry */
/* middle entry at end */
/* middle entry in middle */

	if(vtwin->logical_cursor>=vtwin->total_entries)
					BELL
	else
	{
		if(vtwin->physical_cursor>=num_in_window && vtwin->is_scrollable_up==1)
		{

				vtwin->physical_cursor=num_in_window;
				vtwin->old_cursor=vtwin->physical_cursor;

			if(vtwin->paging==0)
			{
				vtwin->first_entry_on_screen++;
				vtwin->logical_cursor++;
			}
			else
			{
				vtwin->logical_cursor++;
				vtwin->first_entry_on_screen+=num_in_window;
				vtwin->physical_cursor=1;
			}

			if(vtwin->first_entry_on_screen>vtwin->total_entries)
				vtwin->first_entry_on_screen=vtwin->total_entries-num_in_window;
			if(vtwin->first_entry_on_screen<1)
				vtwin->first_entry_on_screen=1;

			if(vtwin->logical_cursor>vtwin->total_entries)
				vtwin->logical_cursor=vtwin->total_entries;

#ifdef MEM_PRINT
			print_window_contents(vtwin,sp, NULL);
			print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
			print_window_contents(vtwin, NULL);
#endif


		}
		else
		{
			if(vtwin->physical_cursor>=num_in_window) /* need to scroll and can't */
			{
				BELL
				vtwin->physical_cursor=num_in_window;
			}
			else
			{
				vtwin->logical_cursor++;
				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->physical_cursor++;
			}
		}

#ifdef MEM_PRINT
		print_window_title_bar(vtwin,sp, NULL);
		print_window_title_bar(vtwin,NULL, NULL);
		print_window_name(vtwin,sp, NULL);
		print_window_name(vtwin,NULL, NULL);
		print_window_numbers(vtwin,sp, NULL);
		print_window_numbers(vtwin,NULL, NULL);
		remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
/*		remove_cursor(vtwin,vtwin->old_cursor,NULL, NULL);
*/		draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
/*		draw_cursor(vtwin,vtwin->physical_cursor,NULL, NULL);
*/
#else
		print_window_title_bar(vtwin, NULL);
		print_window_name(vtwin, NULL);
		print_window_numbers(vtwin, NULL);
		remove_cursor(vtwin,vtwin->old_cursor, NULL);
		draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif
	}	
}

void vt52window_right_arrow( VT52_WINDOW *vtwin
#ifdef MEM_PRINT
, screen *sp
#endif
)
{
		int num_in_window;
		int flag;
		int column;
		num_in_window=vtwin->height*vtwin->columns;		
		column=(vtwin->physical_cursor-1)/vtwin->height;

		flag=0;

/* middle column */
/* middle column with blank to the right */
/* last column */


	if(column+1>=vtwin->columns && vtwin->is_scrollable_left) /* need to page && can */
	{
		if(vtwin->first_entry_on_screen+vtwin->physical_cursor+vtwin->height<=vtwin->total_entries+1)
		{
			if(vtwin->physical_cursor+vtwin->height>num_in_window)
			{
				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->first_entry_on_screen+=vtwin->height;
				vtwin->logical_cursor+=vtwin->height;
			}
			else
			{
				vtwin->logical_cursor+=vtwin->height;
				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->physical_cursor+=vtwin->height;
			}

#ifdef MEM_PRINT
			print_window_contents(vtwin,sp, NULL);
			print_window_title_bar(vtwin,sp, NULL);
			print_window_name(vtwin,sp, NULL);
			print_window_numbers(vtwin,sp, NULL);
			remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
			draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
			print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
			print_window_contents(vtwin, NULL);
			print_window_title_bar(vtwin, NULL);
			print_window_name(vtwin, NULL);
			print_window_numbers(vtwin, NULL);
			remove_cursor(vtwin,vtwin->old_cursor, NULL);
			draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif
		}	
		else
		{
			if(vtwin->logical_cursor<vtwin->total_entries)
			{
				if(((vtwin->logical_cursor-1)/vtwin->height)<
					((vtwin->total_entries-1)/vtwin->height))
				{
					vtwin->first_entry_on_screen+=vtwin->height;
					flag=1;
				}
				vtwin->logical_cursor=vtwin->total_entries;
				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->physical_cursor=vtwin->total_entries-
						vtwin->first_entry_on_screen+1;

#ifdef MEM_PRINT
			if(flag==1)	
			{
				print_window_contents(vtwin,sp, NULL);
				print_window_title_bar(vtwin,sp, NULL);
				print_window_name(vtwin,sp, NULL);
				print_window_numbers(vtwin,sp, NULL);
				remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
				draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
				print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
			}
			else
			{
				print_window_title_bar(vtwin,sp, NULL);
				print_window_title_bar(vtwin,NULL, NULL);
				print_window_name(vtwin,sp, NULL);
				print_window_name(vtwin,NULL, NULL);
				print_window_numbers(vtwin,sp, NULL);
				print_window_numbers(vtwin,NULL, NULL);
				remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
/*				remove_cursor(vtwin,vtwin->old_cursor,NULL, NULL);
*/
				draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
/*				draw_cursor(vtwin,vtwin->physical_cursor,NULL, NULL);
*/			}
#else
			if(flag==1)
				print_window_contents(vtwin, NULL);
			print_window_title_bar(vtwin, NULL);
			print_window_name(vtwin, NULL);
			print_window_numbers(vtwin, NULL);
			remove_cursor(vtwin,vtwin->old_cursor, NULL);
			draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif

			}
			else
				BELL
		}
	}
	else
	{
		if(column+1>=vtwin->columns) /* need to page and can't */
			BELL
		else
		{
		if(vtwin->first_entry_on_screen+vtwin->physical_cursor+vtwin->height<=vtwin->total_entries+1)
		{

				vtwin->logical_cursor+=vtwin->height;
				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->physical_cursor+=vtwin->height;



#ifdef MEM_PRINT
		print_window_title_bar(vtwin,sp, NULL);
		print_window_title_bar(vtwin,NULL, NULL);
		print_window_name(vtwin,sp, NULL);
		print_window_name(vtwin,NULL, NULL);
		print_window_numbers(vtwin,sp, NULL);
		print_window_numbers(vtwin,NULL, NULL);
		remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
/*		remove_cursor(vtwin,vtwin->old_cursor,NULL, NULL);
*/		draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
/*		draw_cursor(vtwin,vtwin->physical_cursor,NULL, NULL);
*/
#else
		print_window_title_bar(vtwin, NULL);
		print_window_name(vtwin, NULL);
		print_window_numbers(vtwin, NULL);
		remove_cursor(vtwin,vtwin->old_cursor, NULL);
		draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif

		}	
		else
		{
			if(vtwin->logical_cursor<vtwin->total_entries /*&&
				!(vtwin->first_entry_on_screen+vtwin->physical_cursor+vtwin->height<vtwin->total_entries)
				((vtwin->logical_cursor-1)/vtwin->height)==((vtwin->total_entries-1)/vtwin->height)*/)
			{
				vtwin->logical_cursor=vtwin->total_entries;
				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->physical_cursor=vtwin->total_entries-
						vtwin->first_entry_on_screen+1;


#ifdef MEM_PRINT
		print_window_title_bar(vtwin,sp, NULL);
		print_window_title_bar(vtwin,NULL, NULL);
		print_window_name(vtwin,sp, NULL);
		print_window_name(vtwin,NULL, NULL);
		print_window_numbers(vtwin,sp, NULL);
		print_window_numbers(vtwin,NULL, NULL);
		remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
/*		remove_cursor(vtwin,vtwin->old_cursor,NULL, NULL);
*/		draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
/*		draw_cursor(vtwin,vtwin->physical_cursor,NULL, NULL);
*/
#else
		print_window_title_bar(vtwin, NULL);
		print_window_name(vtwin, NULL);
		print_window_numbers(vtwin, NULL);
		remove_cursor(vtwin,vtwin->old_cursor, NULL);
		draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif


			}
			else
				BELL
		}
		}
	}

}

void vt52window_left_arrow( VT52_WINDOW *vtwin 
#ifdef MEM_PRINT
, screen *sp
#endif
)
{
		int num_in_window;
		int column;
		num_in_window=vtwin->height*vtwin->columns;		
		column=(vtwin->physical_cursor-1)/vtwin->height;


/* middle column */
/* middle column with blank to the right */
/* last column */

	if(column==0 && vtwin->is_scrollable_left) /* need to page */
	{
		if(vtwin->first_entry_on_screen-vtwin->height>0)
		{


				vtwin->logical_cursor-=vtwin->height;
				vtwin->first_entry_on_screen-=vtwin->height;
				vtwin->old_cursor=vtwin->physical_cursor;


#ifdef MEM_PRINT
			print_window_contents(vtwin,sp, NULL);
			print_window_title_bar(vtwin,sp, NULL);
			print_window_name(vtwin,sp, NULL);
			print_window_numbers(vtwin,sp, NULL);
			remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
			draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
			print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
			print_window_contents(vtwin, NULL);
			print_window_title_bar(vtwin, NULL);
			print_window_name(vtwin, NULL);
			print_window_numbers(vtwin, NULL);
			remove_cursor(vtwin,vtwin->old_cursor, NULL);
			draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif

		}	
		else
			BELL
	}
	else
	{
		if(column==0)
			BELL
		else
		{

				vtwin->logical_cursor-=vtwin->height;
				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->physical_cursor-=vtwin->height;


#ifdef MEM_PRINT
		print_window_title_bar(vtwin,sp, NULL);
		print_window_title_bar(vtwin,NULL, NULL);
		print_window_name(vtwin,sp, NULL);
		print_window_name(vtwin,NULL, NULL);
		print_window_numbers(vtwin,sp, NULL);
		print_window_numbers(vtwin,NULL, NULL);
		remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
/*		remove_cursor(vtwin,vtwin->old_cursor,NULL, NULL);
*/		draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
/*		draw_cursor(vtwin,vtwin->physical_cursor,NULL, NULL);
*/
/*		print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
*/
#else
		print_window_title_bar(vtwin, NULL);
		print_window_name(vtwin, NULL);
		print_window_numbers(vtwin, NULL);
		remove_cursor(vtwin,vtwin->old_cursor, NULL);
		draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif


		}
	}
}

void vt52window_paging(VT52_WINDOW *vtwin
#ifdef MEM_PRINT
, screen *sp
#endif
)
{
		int num_in_window;
		int column;
		num_in_window=vtwin->height*vtwin->columns;		
		column=(vtwin->physical_cursor-1)/vtwin->height;

	if(vtwin->is_scrollable_up)
	{
		if(vtwin->first_entry_on_screen+num_in_window<vtwin->total_entries+1)
		{

				vtwin->first_entry_on_screen+=num_in_window;
				vtwin->logical_cursor+=num_in_window;

				if(vtwin->logical_cursor>vtwin->total_entries)
					vtwin->logical_cursor=vtwin->total_entries;

				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->physical_cursor=vtwin->logical_cursor-vtwin->first_entry_on_screen;
				vtwin->physical_cursor++;

#ifdef MEM_PRINT
			print_window_contents(vtwin,sp, NULL);
			print_window_title_bar(vtwin,sp, NULL);
			print_window_name(vtwin,sp, NULL);
			print_window_numbers(vtwin,sp, NULL);
			remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
			draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
			print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
			print_window_contents(vtwin, NULL);
			print_window_title_bar(vtwin, NULL);
			print_window_name(vtwin, NULL);
			print_window_numbers(vtwin, NULL);
			remove_cursor(vtwin,vtwin->old_cursor, NULL);
			draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif

		}	
		else
		{
			if(vtwin->logical_cursor!=vtwin->total_entries)
			{
				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->physical_cursor=vtwin->total_entries-vtwin->first_entry_on_screen+1;
				vtwin->logical_cursor=vtwin->total_entries;

#ifdef MEM_PRINT
		print_window_title_bar(vtwin,sp, NULL);
		print_window_title_bar(vtwin,NULL, NULL);
		print_window_name(vtwin,sp, NULL);
		print_window_name(vtwin,NULL, NULL);
		print_window_numbers(vtwin,sp, NULL);
		print_window_numbers(vtwin,NULL, NULL);
		remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
/*		remove_cursor(vtwin,vtwin->old_cursor,NULL, NULL);
*/		draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
/*		draw_cursor(vtwin,vtwin->physical_cursor,NULL, NULL);
*/
#else
		print_window_title_bar(vtwin, NULL);
		print_window_name(vtwin, NULL);
		print_window_numbers(vtwin, NULL);
		remove_cursor(vtwin,vtwin->old_cursor, NULL);
		draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif

			}
			else
				BELL
		}
	}
	else
		BELL

}

void vt52window_paging_up( VT52_WINDOW *vtwin 

#ifdef MEM_PRINT
, screen *sp
#endif
)
{
		int num_in_window;
		int column;
		num_in_window=vtwin->height*vtwin->columns;		
		column=(vtwin->physical_cursor-1)/vtwin->height;

	if(vtwin->is_scrollable_up)
	{
		if(vtwin->first_entry_on_screen>1)
		{

				vtwin->first_entry_on_screen-=num_in_window;
				vtwin->logical_cursor-=num_in_window;

				if(vtwin->logical_cursor<1)
					vtwin->logical_cursor=1;
		
				if(vtwin->first_entry_on_screen<1)
					vtwin->first_entry_on_screen=1;

				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->physical_cursor=vtwin->logical_cursor-vtwin->first_entry_on_screen;
				vtwin->physical_cursor++;

#ifdef MEM_PRINT
			print_window_contents(vtwin,sp, NULL);
			print_window_title_bar(vtwin,sp, NULL);
			print_window_name(vtwin,sp, NULL);
			print_window_numbers(vtwin,sp, NULL);
			remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
			draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
			print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
			print_window_contents(vtwin, NULL);
			print_window_title_bar(vtwin, NULL);
			print_window_name(vtwin, NULL);
			print_window_numbers(vtwin, NULL);
			remove_cursor(vtwin,vtwin->old_cursor, NULL);
			draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif


		}	
		else
		{
			if(vtwin->logical_cursor!=1)
			{
				vtwin->old_cursor=vtwin->physical_cursor;
				vtwin->physical_cursor=1;
				vtwin->logical_cursor=1;

#ifdef MEM_PRINT
		print_window_title_bar(vtwin,sp, NULL);
		print_window_title_bar(vtwin,NULL, NULL);
		print_window_name(vtwin,sp, NULL);
		print_window_name(vtwin,NULL, NULL);
		print_window_numbers(vtwin,sp, NULL);
		print_window_numbers(vtwin,NULL, NULL);
		remove_cursor(vtwin,vtwin->old_cursor,sp, NULL);
/*		remove_cursor(vtwin,vtwin->old_cursor,NULL, NULL);
*/		draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
/*		draw_cursor(vtwin,vtwin->physical_cursor,NULL, NULL);
*/
#else
		print_window_title_bar(vtwin, NULL);
		print_window_name(vtwin, NULL);
		print_window_numbers(vtwin, NULL);
		remove_cursor(vtwin,vtwin->old_cursor, NULL);
		draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif

			}
			else
				BELL
		}
	}
	else
		BELL
}

void change_window_cols( VT52_WINDOW *vtwin, int change, int screen_width )
{
	int old_col;

	old_col=vtwin->columns;
	vtwin->columns=change+vtwin->columns;

	/* check that window still fits the screen and that it has at least 1 col*/
	if(!(((vtwin->columns*vtwin->chars_per_col)+vtwin->columns+1+vtwin->xpos+1)<screen_width
		&& vtwin->columns>0))
	{
		vtwin->columns=old_col;
		BELL
	}
	else
	{
		if(change<0)
		{
			dirty_rectangle2[0]=0;
			dirty_rectangle2[1]=0;
			dirty_rectangle2[2]=0;
			dirty_rectangle2[3]=0;
		
			dirty_rectangle1[0]=(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+1+vtwin->xpos+1;
			dirty_rectangle1[1]=vtwin->ypos;
			dirty_rectangle1[2]=dirty_rectangle1[0]+vtwin->chars_per_col+2;
			dirty_rectangle1[3]=vtwin->ypos+vtwin->height+1;
		}
	}
}

void change_window_height( VT52_WINDOW *vtwin, int change, int screen_height )
{
	int old_height;

	old_height=vtwin->height;
	vtwin->height=change+vtwin->height;

	/* check that window still fits the screen and that it has at least 1 col*/
	if(!(((vtwin->height+vtwin->ypos+1)<(screen_height-2))
		&& vtwin->height>0))
	{
		vtwin->height=old_height;
		BELL
	}
	else
	{
		if(change<0)
		{
			dirty_rectangle2[0]=0;
			dirty_rectangle2[1]=0;
			dirty_rectangle2[2]=0;
			dirty_rectangle2[3]=0;
		
			dirty_rectangle1[0]=vtwin->xpos;
			dirty_rectangle1[1]=vtwin->ypos+vtwin->height+2;
			dirty_rectangle1[2]=(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+1+vtwin->xpos+1;
			dirty_rectangle1[3]=vtwin->ypos+vtwin->height+2;
		}
	}
}

void move_window( VT52_WINDOW *vtwin, 
						int xchange, int ychange, 
						int screen_width, int screen_height )
{
	int newx;
	int newy;
	int window_len;
	int window_height;

	newx=vtwin->xpos+xchange;
	newy=vtwin->ypos+ychange;
	window_len=(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2;
	window_height=vtwin->height+1;
	
	if(newx>=0 && newx+window_len<screen_width &&
		newy>0 && newy+window_height<screen_height-2)
	{

		if(xchange!=0)
		{
			if(xchange<0)
			{
				dirty_rectangle2[0]=vtwin->xpos+window_len+xchange;
				dirty_rectangle2[1]=vtwin->ypos;
				dirty_rectangle2[2]=vtwin->xpos+window_len;
				dirty_rectangle2[3]=vtwin->ypos+window_height;
			}
			else
			{
				dirty_rectangle2[0]=vtwin->xpos;
				dirty_rectangle2[1]=vtwin->ypos;
				dirty_rectangle2[2]=vtwin->xpos+xchange;
				dirty_rectangle2[3]=vtwin->ypos+window_height;
			}
		}

		if(ychange!=0)
		{		
			if(ychange<0)
			{		
				dirty_rectangle1[0]=vtwin->xpos;
				dirty_rectangle1[1]=vtwin->ypos+window_height+ychange;
				dirty_rectangle1[2]=vtwin->xpos+window_len;
				dirty_rectangle1[3]=vtwin->ypos+window_height;
			}
			else
			{
				dirty_rectangle1[0]=vtwin->xpos;
				dirty_rectangle1[1]=vtwin->ypos;
				dirty_rectangle1[2]=vtwin->xpos+window_len;
				dirty_rectangle1[3]=vtwin->ypos+ychange;
			}
		}

		vtwin->xpos=newx;
		vtwin->ypos=newy;

	}
	else
		BELL

}	
	

void reset_cursors( VT52_WINDOW *vtwin )
{
		int num_in_window;
		int column;
		num_in_window=vtwin->height*vtwin->columns;		
		column=(vtwin->physical_cursor-1)/vtwin->height;

		if(vtwin->logical_cursor>vtwin->first_entry_on_screen+num_in_window)
		{
			vtwin->logical_cursor=vtwin->logical_cursor-vtwin->height;
			if(vtwin->logical_cursor<1)
				vtwin->logical_cursor=1;

			vtwin->physical_cursor=	vtwin->logical_cursor-
											vtwin->first_entry_on_screen+1;
		}
}

void reset_cursors2( VT52_WINDOW *vtwin )
{
		int num_in_window;
		int column;
		num_in_window=vtwin->height*vtwin->columns;		
		column=(vtwin->physical_cursor-1)/vtwin->height;

		if(vtwin->logical_cursor>=vtwin->first_entry_on_screen+num_in_window)
		{
			vtwin->logical_cursor=vtwin->first_entry_on_screen+num_in_window-1;
			if(vtwin->logical_cursor<1)
				vtwin->logical_cursor=1;

			vtwin->physical_cursor=	vtwin->logical_cursor-
											vtwin->first_entry_on_screen+1;
		}
}




void print_area_under_frame(VT52_WINDOW *vtwin, screen *sp)
{ 

	print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos);
	print_area(sp,vtwin->xpos,vtwin->ypos+1,vtwin->xpos,vtwin->ypos+vtwin->height);
	print_area(sp,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height);
	print_area(sp,vtwin->xpos,vtwin->ypos+vtwin->height+1,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
}

void print_dotted_frame(VT52_WINDOW *vtwin)
{
	int loop;

	AT(vtwin->ypos,vtwin->xpos)
	for(loop=0;loop<((vtwin->columns*(vtwin->chars_per_col+1))+3);loop++)
		Bconout(2,'\'');

	for(loop=0;loop<vtwin->height;loop++)
	{
		AT(vtwin->ypos+loop+1,vtwin->xpos)
		Bconout(2,'\'');
		AT(vtwin->ypos+loop+1,vtwin->xpos+((vtwin->columns*(vtwin->chars_per_col+1))+2))
		Bconout(2,'\'');
	}

	AT(vtwin->ypos+vtwin->height+1,vtwin->xpos)
	for(loop=0;loop<((vtwin->columns*(vtwin->chars_per_col+1))+3);loop++)
		Bconout(2,'\'');
}



#define UP_ARROW		0x48
#define DOWN_ARROW	0x50
#define LEFT_ARROW	0x4B
#define RIGHT_ARROW	0x4D
#define LEFT_SHIFT	2
#define RIGHT_SHIFT	1
#define CONTROL		4
#define ALT				8
#define CONTROL_LEFT	0x73
#define CONTROL_RIGHT 0x74



int control_changed;
int alt_tabbed;
LinkedList *alt_tabbing_list;

void set_up_window_name_frame(int *length, int *x, int *y, LinkedList *windows, screen *sp)
{
	LinkedList *head;
	int longest_count;
	int current_count;

	head=windows;

	if(head==NULL)
	{
		*length=0;
		*x=0;
		*y=0;
	}

	current_count=0;
	longest_count=0;

	do{
		
		current_count=strlen(((VT52_WINDOW *)windows->node)->name);
		if(current_count>longest_count)
			longest_count=current_count;

		windows=windows->right;

	}
	while(windows!=head);

	longest_count+=4;

	*length=longest_count;
	*x=(sp->width/2)-(longest_count/2);
	*y=(sp->height/2)-2;

}

void print_window_name_frame(int x, int y, char *name, int length)
{
	int loop;
	int little_len;

	AT(y,x)
	REVERSE_VIDEO
	for(loop=0;loop<length;loop++)
		Bconout(2,'+');

	NORMAL_VIDEO

	AT(y+1,x)
	REVERSE_VIDEO
	Bconout(2,'+');
	NORMAL_VIDEO
	
	for(loop=0;loop<length-2;loop++)
		Bconout(2,' ');
	REVERSE_VIDEO
	Bconout(2,'+');
	NORMAL_VIDEO

	AT(y+2,x)
	REVERSE_VIDEO

	for(loop=0;loop<length;loop++)
		Bconout(2,'+');
	NORMAL_VIDEO

	little_len=strlen(name);
	
	little_len=length-little_len;
	little_len-=4;
	little_len=(little_len/2)/*+(little_len%2)*/;

	AT(y+1,x+2+little_len)
	printf("%s",name);
}

int global_s;

long window_key( 	VT52_WINDOW *vtwin, 
						int screen_width, int screen_height,
						int pixels_per_char, int pixels_per_height,
						int handle, long k,screen *sp)
{
/*		long k;*/
		int s,u,l;
/*		short blitxyarray[8];
		MFDB src,dest;
*/

/*	while(1)
	{

		while(!Bconstat(2))
		{
			;
		}
*/
/*		k=Bconin(2);*/
		u=(int)(k>>16)&0xff;
		l=(int)(k%256);
		s=(int)(k>>24)&0x0f;	/* we are only interested in Shift, Cntrl and Alt see page 306 for details */

		s=global_s;	/* this fixes some strange bug in the auto stuff! */

AT(0,0)
printf("\n s = %d, Con = %d, can = %d, u = %d down = %d\n",s,CONTROL,vtwin->can_resize_horizontal,u,DOWN_ARROW);


		switch(u)
		{
		case UP_ARROW:
							switch(s)
							{
							case CONTROL:
									if(vtwin->can_resize_vertical==1)
										{
/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
 											remove_cursor(vtwin, vtwin->physical_cursor,sp, NULL);
#else
 											remove_cursor(vtwin, vtwin->physical_cursor, NULL);
#endif
*/

											control_changed=1;
											print_area_under_frame(vtwin,sp);
											change_window_height(vtwin, -1, screen_height);
											reset_cursors2( vtwin );
											print_dotted_frame(vtwin);

/*	PUT THIS BACK IN TO REDRAW WHEN MOVING
#ifdef MEM_PRINT
											print_window_frame(vtwin,1,sp, NULL);
											print_window_contents(vtwin,sp, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
											print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
											print_window_frame(vtwin,1, NULL);
											print_window_contents(vtwin, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
#endif
*/
										}
									else
											BELL
									break;
							case CONTROL+LEFT_SHIFT:
							case CONTROL+RIGHT_SHIFT:
									if(vtwin->is_moveable==1)
									{
/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
										remove_cursor(vtwin, vtwin->physical_cursor,sp, NULL);
#else
										remove_cursor(vtwin, vtwin->physical_cursor, NULL);
#endif
*/
/*										blitxyarray[0]=vtwin->xpos*pixels_per_char;
										blitxyarray[1]=vtwin->ypos*pixels_per_height;
										blitxyarray[2]=((vtwin->xpos*vtwin->columns)+vtwin->columns+1)*pixels_per_char+pixels_per_char-1;
										blitxyarray[3]=(vtwin->ypos*vtwin->height+1)*pixels_per_height+pixels_per_height-1;
*/

/* restore area under current window frame*/
											print_area_under_frame(vtwin,sp);

											control_changed=1;
										move_window(vtwin,/*x*/0, /*y*/-1, screen_width, screen_height);
/* draw window frame */
											print_dotted_frame(vtwin);

/*										blitxyarray[4]=vtwin->xpos*pixels_per_char;
										blitxyarray[5]=vtwin->ypos*pixels_per_height;
										blitxyarray[6]=((vtwin->xpos*vtwin->columns)+vtwin->columns+1)*pixels_per_char+pixels_per_char-1;
										blitxyarray[7]=(vtwin->ypos*vtwin->height+1)*pixels_per_height+pixels_per_height-1;
										src.fd_addr=NULL;
										dest.fd_addr=NULL;

*/
/*	PUT THIS BACK IN TO REDRAW WHEN MOVING 
#ifdef MEM_PRINT
										print_window_frame(vtwin,1,sp, NULL);
										print_window_contents(vtwin,sp, NULL);
#else
										print_window_frame(vtwin,1, NULL);
										print_window_contents(vtwin, NULL);
#endif
*/

/*										vro_cpyfm(handle,S_ONLY,blitxyarray,&src,&dest);*/

/* PUT THIS BACK IN TO REDRAW WHEN MOVING
#ifdef MEM_PRINT
										draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
										print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
										draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif
*/
									}
									else
										BELL
									break;
							case LEFT_SHIFT:
							case RIGHT_SHIFT:
#ifdef MEM_PRINT
										vt52window_paging_up( vtwin ,sp);
#else
										vt52window_paging_up( vtwin );

#endif
/*
#ifdef MEM_PRINT
	print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#endif
*/										break;
							default:
#ifdef MEM_PRINT
										vt52window_up_arrow( vtwin, sp );
#else
										vt52window_up_arrow( vtwin);
#endif

/*
#ifdef MEM_PRINT
	print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#endif
*/
										break;
							}
							break;
		case DOWN_ARROW:
							switch(s)
							{
							case CONTROL:
									if(vtwin->can_resize_vertical==1)
										{
/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
 											remove_cursor(vtwin, vtwin->physical_cursor, sp, NULL);
#else
 											remove_cursor(vtwin, vtwin->physical_cursor, NULL);
#endif
*/
											control_changed=1;
											print_area_under_frame(vtwin,sp);
											change_window_height(vtwin, 1, screen_height );
											reset_cursors2( vtwin );
											print_dotted_frame(vtwin);

/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
											print_window_frame(vtwin,1,sp, NULL);
											print_window_contents(vtwin,sp, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
											print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
											print_window_frame(vtwin,1, NULL);
											print_window_contents(vtwin, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif
*/
										}
									break;
							case CONTROL+LEFT_SHIFT:
							case CONTROL+RIGHT_SHIFT:
									if(vtwin->is_moveable==1)
									{
/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
										remove_cursor(vtwin, vtwin->physical_cursor,sp, NULL);
#else
										remove_cursor(vtwin, vtwin->physical_cursor, NULL);
#endif
*/

										control_changed=1;
										print_area_under_frame(vtwin,sp);
										move_window(vtwin,/*x*/0, /*y*/1, screen_width, screen_height);
										print_dotted_frame(vtwin);

/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
										print_window_frame(vtwin,1,sp, NULL);
										print_window_contents(vtwin,sp, NULL);
										draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
										print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
										print_window_frame(vtwin,1, NULL);
										print_window_contents(vtwin, NULL);
										draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif
*/
									}
									else
										BELL
									break;
							case LEFT_SHIFT:
							case RIGHT_SHIFT:
#ifdef MEM_PRINT
										vt52window_paging( vtwin,sp);
#else
										vt52window_paging( vtwin);
#endif
/*
#ifdef MEM_PRINT
	print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#endif
*/										break;
							default:
#ifdef MEM_PRINT
										vt52window_down_arrow( vtwin, sp);
#else
										vt52window_down_arrow( vtwin );
#endif

/*
#ifdef MEM_PRINT
	print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#endif
*/										break;
								}
							break;
		case LEFT_ARROW:
							switch(s)
							{
							case LEFT_SHIFT:
							case RIGHT_SHIFT:
#ifdef MEM_PRINT
										vt52window_paging_up( vtwin,sp);
#else
										vt52window_paging_up( vtwin);
#endif
/*#ifdef MEM_PRINT
	print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#endif
*/										break;
							default:
#ifdef MEM_PRINT
										vt52window_left_arrow( vtwin,sp);
#else
										vt52window_left_arrow( vtwin);
#endif

/*#ifdef MEM_PRINT
	print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#endif
*/										break;
							}
							break;
		case RIGHT_ARROW:
							switch(s)
							{
							case LEFT_SHIFT:
							case RIGHT_SHIFT:
#ifdef MEM_PRINT
										vt52window_paging( vtwin,sp);
#else
										vt52window_paging( vtwin);
#endif
/*#ifdef MEM_PRINT
	print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#endif
*/										break;
							default:
#ifdef MEM_PRINT
										vt52window_right_arrow( vtwin,sp);
#else
										vt52window_right_arrow( vtwin);
#endif
/*#ifdef MEM_PRINT
	print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#endif
*/										break;
							}
							break;
		case CONTROL_RIGHT:
									if(s==CONTROL && vtwin->can_resize_horizontal==1)
										{
/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
 											remove_cursor(vtwin, vtwin->physical_cursor,sp, NULL);
#else
 											remove_cursor(vtwin, vtwin->physical_cursor, NULL);
#endif
*/
											control_changed=1;
											print_area_under_frame(vtwin,sp);
											change_window_cols(vtwin, 1, screen_width );
											reset_cursors( vtwin );
											print_dotted_frame(vtwin);

/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
											print_window_frame(vtwin,1,sp, NULL);
											print_window_contents(vtwin,sp, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
											print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
											print_window_frame(vtwin,1, NULL);
											print_window_contents(vtwin, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif
*/
										}
									else
									{
										if((s==CONTROL+LEFT_SHIFT || s==CONTROL+RIGHT_SHIFT) &&vtwin->is_moveable==1)
										{
/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
											remove_cursor(vtwin, vtwin->physical_cursor,sp, NULL);
#else
											remove_cursor(vtwin, vtwin->physical_cursor, NULL);
#endif
*/
											control_changed=1;
											print_area_under_frame(vtwin,sp);
											move_window(vtwin,/*x*/1, /*y*/0, screen_width, screen_height);
											print_dotted_frame(vtwin);

/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
											print_window_frame(vtwin,1,sp, NULL);
											print_window_contents(vtwin,sp, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
											print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
											print_window_frame(vtwin,1, NULL);
											print_window_contents(vtwin, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif
*/
										}
										else
											BELL

									}
									break;
		case CONTROL_LEFT:
									if(s==CONTROL && vtwin->can_resize_horizontal==1)
										{
/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
											remove_cursor(vtwin, vtwin->physical_cursor,sp, NULL);
#else
											remove_cursor(vtwin, vtwin->physical_cursor, NULL);
#endif
*/

											control_changed=1;
											print_area_under_frame(vtwin,sp);
											change_window_cols(vtwin, -1, screen_width );
											reset_cursors( vtwin );
											print_dotted_frame(vtwin);

/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
											print_window_frame(vtwin,1,sp, NULL);
											print_window_contents(vtwin,sp, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
											print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
											print_window_frame(vtwin,1, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif
*/

										}
									else
									{
										if((s==CONTROL+LEFT_SHIFT || s==CONTROL+RIGHT_SHIFT) &&vtwin->is_moveable==1)
										{
/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
											remove_cursor(vtwin, vtwin->physical_cursor,sp, NULL);
#else
											remove_cursor(vtwin, vtwin->physical_cursor, NULL);
#endif
*/
											control_changed=1;
											print_area_under_frame(vtwin,sp);
											move_window(vtwin,/*x*/-1, /*y*/0, screen_width, screen_height);
											print_dotted_frame(vtwin);

/* PUT THIS BACK IN TO REDRAW WHEN MOVING */
/*
#ifdef MEM_PRINT
											print_window_frame(vtwin,1,sp, NULL);
											print_window_contents(vtwin,sp, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor,sp, NULL);
											print_area(sp,vtwin->xpos,vtwin->ypos,vtwin->xpos+(vtwin->columns*vtwin->chars_per_col)+vtwin->columns+2,vtwin->ypos+vtwin->height+1);
#else
											print_window_frame(vtwin,1, NULL);
											print_window_contents(vtwin, NULL);
											draw_cursor(vtwin,vtwin->physical_cursor, NULL);
#endif
*/

									}
										else
											BELL
									}
									break;
		default:
				AT(21,10)
				printf("before k %ld, u %d, l %d, s %d",k,u,l,s);
				return (vtwin->other_keys)(vtwin,k,sp);
				break;
		}
/*	}*/

	return -1;
}


long window_loop( VT52_WINDOW **curr_win, 
						int screen_width, int screen_height,
						int pixels_per_char, 
						int pixels_per_height,
						int handle,
						screen *sp)
{
	extern LinkedList *circular_windows_list;

	long return_code=-1, key_status, shift_status, old_shift_status;
	short mode, old_mode, changed_mode, masked_old_state, masked_new_state, change_in_state;
	int control_on;
	long k;
	int win_len,win_x,win_y;
	int u,l,s;
	VT52_WINDOW *vtwin;

	vtwin=*curr_win;

	control_on=0;
	control_changed=0;
	alt_tabbed=0;
	alt_tabbing_list=circular_windows_list;
	shift_status=Kbshift(-1);
	shift_status&=0xF0;
	old_shift_status=shift_status;
	Kbshift(shift_status);

	set_up_window_name_frame(&win_len, &win_x, &win_y, alt_tabbing_list,sp);
	while(strcmp(((VT52_WINDOW *)alt_tabbing_list->node)->name,vtwin->name))
		alt_tabbing_list=alt_tabbing_list->right;

/*printf("len = %d, x %d, y %d\n",win_len,win_x, win_y);
getchar();
*/

	while(return_code==-1)
	{

	do{
		while(((key_status=Bconstat(2))==0) && ((shift_status=Kbshift(-1))==old_shift_status))
		{
			/*possibly update the clock here*/;
		}

/*		BELL*/

		if(key_status==0)	/* shift change */
		{
/*			mode=shift_status;
			mode&=0x0F;	/* only interested in cntrl alt and shift */
			old_mode=old_shift_status;
			old_mode&=0x0F;
			changed_mode=mode^old_mode;	/* XOR the modes to find out 
														which bit has changed */

			/* bits are 0 right shift, 1 left shift, 2 cntrl, 3 alt */
			switch(changed_mode)
			{
			case 0x00:
					AT(sp->height-2,10)
					printf(" NOTHING AT ALL ");
					break;
						
			case 0x01:	
			case 0x02:
			case 0x03:	/* only shifts */
					
					if(old_mode&0x01 || old_mode&0x02 || old_mode &0x03)
					{
						if(mode&
						break;

					AT(sp->height-2,10)
					printf(" SHIFT HELP ON ");
					break;
			}		
			
		}
*/
		masked_old_state=old_shift_status&0x0F;
		masked_new_state=shift_status&0x0F;
		changed_mode=masked_old_state^masked_new_state;	/* XOR the modes to find out 
														which bit has changed */
		if(changed_mode&0x04)					
			control_on^=0x01;
		if(control_on==0 && control_changed)
		{
			/*need to redraw the screen here*/
			clear_screen(sp);
			print_ordered_windows(circular_windows_list,vtwin,sp);
			print_area(sp,0,0,sp->width-1, sp->height-1);
			control_changed=0;
		}

		if(changed_mode&0x08 && alt_tabbed==1)
		{
			/* get rid of little window*/
			print_area(sp,win_x,win_y,win_x+win_len,win_y+3);

			/* should set current_window and refocus, provided alt_tabbing _list->node
				is not the current_window */
			if(*curr_win!=alt_tabbing_list->node)
			{
				(vtwin->unfocus)(vtwin);
				print_window_frame(vtwin,1,sp,NULL);
				print_window_contents(vtwin,sp,NULL);
				remove_cursor(vtwin,vtwin->physical_cursor,sp,NULL);
				/* printed to memory, now print to screen*/
				print_window_frame(vtwin,0,NULL,NULL);

				*curr_win=alt_tabbing_list->node;
				vtwin=*curr_win;

				(vtwin->focus)(vtwin);

				print_window_frame(vtwin,1,sp,NULL);
				print_window_contents(vtwin,sp,NULL);
				draw_cursor(vtwin,vtwin->physical_cursor,sp,NULL);
				/* printed to memory, now print to screen*/
				print_window_frame(vtwin,1,NULL,NULL);
				print_window_contents(vtwin,NULL,NULL);


				/*print_window_bottom_help(vtwin, sp);*/

				/*print_area(sp,0,0,sp->width-1, sp->height-1);*/
			}
			alt_tabbed=0;
		}		

		global_s=masked_new_state;	/* for some reason shifts are not recoginsed */
											/* in the autofolder anymore! */

		switch(masked_new_state)
		{
			case 0x00:	/* NOTHING PRESSED*/
					AT(sp->height-2,10)
					printf(" NO SHIFT KEY DOWN HELP ");
					break;
				
			case 0x01:	/*  RIGHT SHIFT */
			case 0x02:	/*  LEFT SHIFT */
			case 0x03:	/* BOTH SHIFT */
					AT(sp->height-2,10)
					printf("ANY SHIFT KEY DOWN HELP ");
					break;
		
			case 0x04:	/* CNTRL */
					AT(sp->height-2,10)
					printf(" CNTRL SHIFT KEY DOWN HELP ");
					break;
	
			case 0x05:	/*  CNTRL & RIGHT SHIFT */
			case 0x06:	/*  CNTRL & LEFT SHIFT */
			case 0x07:	/*  CNTRL & BOTH SHIFT */
					AT(sp->height-2,10)
					printf(" CNTRL&SHIFT KEY DOWN HELP ");
					break;

			case 0x08:	/* ALT */
					AT(sp->height-2,10)
					printf(" ALT SHIFT KEY DOWN HELP ");
					break;

			case 0x09:	/* ALT & RIGHT SHIFT */
			case 0x0A:	/* ALT & LEFT SHIFT */
			case 0x0B:	/* ALT & BOTH SHIFT */
					AT(sp->height-2,10)
					printf(" ALT&SHIFT KEY DOWN HELP ");
					break;

			case 0x0C:	/* ALT & CNTRL */
			case 0x0D:	/* ALT & CNTRL & SHIFT */
			case 0x0E:	/* ALT & CNTRL & SHIFT */
			case 0x0F:	/* ALT & CNTRL & SHIFT */
					break;
		
		}

	}

	old_shift_status=shift_status;	

	}while(key_status==0);	


		k=Bconin(2);

		u=(int)(k>>16)&0xff;
		l=(int)(k%256);
		s=(int)(k>>24)&0x0f;	/* we are only interested in Shift, Cntrl and Alt see page 306 for details */

		switch(u)
		{
			case 15:	/* TAB */
						if(masked_new_state==0x09 || 
							masked_new_state==0x0A ||
							masked_new_state==0x0B)
						{
							alt_tabbed=1;
							alt_tabbing_list=alt_tabbing_list->left;
							print_window_name_frame(win_x, win_y,((VT52_WINDOW *)alt_tabbing_list->node)->name,win_len);
							
						}
						else
						{
							if(masked_new_state==0x08)
							{
								alt_tabbed=1;
								alt_tabbing_list=alt_tabbing_list->right;
								print_window_name_frame(win_x, win_y,((VT52_WINDOW *)alt_tabbing_list->node)->name,win_len);
							}
							else
							{
							/*	return_code=k;*/
							(vtwin->unfocus)(vtwin);
							print_window_frame(vtwin,1,sp,NULL);
							print_window_contents(vtwin,sp,NULL);
							remove_cursor(vtwin,vtwin->physical_cursor,sp,NULL);
							/* printed to memory, now print to screen*/
							print_window_frame(vtwin,0,NULL,NULL);

							if(masked_new_state==0x01 || 
								masked_new_state==0x02 || 
								masked_new_state==0x03) 
							{
								alt_tabbing_list=alt_tabbing_list->left;
								vtwin=(VT52_WINDOW *)alt_tabbing_list->node;
							}
							else
							{
								alt_tabbing_list=alt_tabbing_list->right;
								vtwin=(VT52_WINDOW *)alt_tabbing_list->node;
							}
							(vtwin->focus)(vtwin);

							print_window_frame(vtwin,1,sp,NULL);
							print_window_contents(vtwin,sp,NULL);
							draw_cursor(vtwin,vtwin->physical_cursor,sp,NULL);
							/*print_window_bottom_help(vtwin, sp);*/
							/* printed to memory, now print to screen*/
							print_window_frame(vtwin,1,NULL,NULL);
							print_window_contents(vtwin,NULL,NULL);

							/*print_area(sp,0,0,sp->width-1,sp->height-1);*/
							NORMAL_VIDEO
							}
						}
					

						break;
			default:
					return_code=window_key(vtwin, 
										screen_width, screen_height,
										pixels_per_char, pixels_per_height,
										handle, k,sp);

			
		}

	}

	if(return_code==-1)
		return k;
	else
		return return_code;
}

void print_window( VT52_WINDOW *c)
{

	if(c)
	{

		printf("WINDOW:	first %d, logical %d, phys %d, old %d\n",
					c->first_entry_on_screen,
					c->logical_cursor,				/*position in list of entires*/
					c->physical_cursor,				/* position on screen */
					c->old_cursor);

		printf("WINDOW:	top %d, name %s, height %d, cols %d\n",
					c->on_top,				/* is window on top ? */					
					c->name,				/* window name */

					c->height,				/* height in lines for window */
												/* does not include titles */
					c->columns);			/* width in columns does not 
													include space between entries or
													window walls */

		printf("WINDOW:	chars %d, titles %d, name %d, page %d\n",
					c->chars_per_col,	/* number of characters in each column */

					c->titles_at_top,	/* boolean value for pos counts */
					c->name_at_top,		/* boolean value for name pos */
					c->titles_on_off,	/* boolean value for pos counts */
					c->paging);				/* boolean value for page scrolling */

		printf("WINDOW:	X %d, Y %d, total %d, type %d\n",					
					c->xpos,		/* xpos of top left corner */
					c->ypos,		/* ypos of top left corner */

					c->total_entries,	/* total_number of entries */
			
					c->type);		/* window type - 1,PROG, 2,SETS, 3,OTHER */

		printf("WINDOW:	hor %d, ver %d, move %d, left %d, up %d\n",
					c->can_resize_horizontal,
					c->can_resize_vertical,
					c->is_moveable,
					c->is_scrollable_left,
					c->is_scrollable_up);
/*
					LinkedList *entries;	/* list of window entries */
	
					/* pointer to functions each window must implement */
					VFP help;		/* called when help key pressed */		
/*					VFP toggle;*/		/* called when toggle key pressed */
/*					VFP select;*/		/* called when select key pressed */
					VFP print_bottom; /* print text at bottom of screen */
					VFP part_print;	/* print part of the window */
					VFP print;		/* called when printing window contents */
					VFP focus;		/* called when window is in focus */
					VFP unfocus;	/* called when window goes out of focus */
					LFP other_keys;	/* called if unknown key is pressed */

					void *extra_info;	/* extra information depending upon */
*/											/* the window type */
	}
	else
		printf("WINDOW is NULL\n");
}