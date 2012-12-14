#include "vt52.h"
#include "progs.h"
#include "display.h"
#include "filelist.h"
#include "edit.h"
#include "defaults.h"
#include "sets.h"
#include "accs.h"
#include "fileform.h"
#include "options.h"
#include "other.h"
#include "autofold.h"
#include "vtwindow.h"
#include "bootwins.h"
#include "screen.h"
#include "fileslct.h"

/*
	some variables to hold colours if we decide to change them
	and to hold the old resolution if we decide to change it.
	i.e change it from low res to med res, then change it back 
	again
*/
short col1;
short col2;
short col3;
int old_resolution;

#define MAIN_BODY_MISSING

#define TESTING

/*
void update_time()
{
	size_t temp_sizet;
	time_t temp_time;
	struct tm *tmpoint;

	time(&temp_time);
	tmpoint=gmtime(&temp_time);

	temp_sizet= strftime(altime,9,"%X",tmpoint);
}

void update_date()
{
	size_t temp_sizet;
	time_t temp_time;
	struct tm *tmpoint;

	time(&temp_time);
	tmpoint=gmtime(&temp_time);

	temp_sizet= strftime(aldate,9,"%x",tmpoint);
}
*/

				
										

/*
int main_loop( void )
{

		long k;
		int u;
		int l;
 		int s;

/*		int num_in_window;
		void (*draw_func)();
		void (*title_func)();
		void *print_thing;*/
	
	k=0;
	u=0;
	l=0;
	s=0;

	while(u!=prog_defaults.abort_key_high)
	{
	


		if(u==prog_defaults.options_key_high 		/*OPTIONS SCREEN*/
			)
			{
				options_screen();
				draw_screen();
			}		

		if(u==prog_defaults.help_key_high 		/*HELP SCREEN*/
			)
			{
				help_screen();
				draw_screen();
			}	

		if(u==prog_defaults.switch_key_high
			)
			{
				old_window=current_window;
				current_window++;
				if(current_window>4)
					current_window=1;

				draw_old_window_title(old_window);
				switch(old_window)
				{
				case 1:
					remove_cursor(old_window,progs_cursor);
					break;	
				case 2:
					remove_cursor(old_window,accs_cursor);
					break;	
				case 3:
					remove_cursor(old_window,sets_cursor);
					break;	
				case 4:
					remove_cursor(old_window,other_cursor);
					break;	
				}
				switch(current_window)
				{
				case AUTO:
					draw_cursor(current_window,progs_cursor);
					progs_bottom_title();
					break;	
				case ACCS:
					draw_cursor(current_window,accs_cursor);
					accs_bottom_title();
					break;	
				case SETS:
					draw_cursor(current_window,sets_cursor);
					sets_bottom_title();
					break;	
				case OTHER:
					draw_cursor(current_window,other_cursor);
					other_bottom_title();
					break;	
				}
				draw_new_window_title(current_window);

			}

			switch(current_window)
			{
			case AUTO:
					u=progs_main_loop( &s);
					break;	
				case ACCS:
					u=accs_main_loop( &s);
					break;	
				case SETS:
					u=sets_main_loop( &s);
					break;	
				case OTHER:
					u=other_main_loop( &s);
					break;	
			}

	}

/*		set_set_from_progs(curr_set, prgs);
		set_set_from_accs( curr_set, accs);
*/		if(curr_set!=NULL)
			curr_set->others=others;

/*		printf("\n %d \n",s); */
	if(s==2)
		return 0;
	else 
		return 1;
}
*/

#define LEFT_SHIFT	2
#define RIGHT_SHIFT	1
#define CONTROL		4
#define ALT				8


int main_loop2(LinkedList *window_list, screen *sp)
{
	long k;
	int u,s,l;
	VT52_WINDOW *curr_win;

	k=0;
	u=0;
	s=0;
	l=0;

	curr_win=(VT52_WINDOW *)window_list->node;

	/* print the window */
	print_window_frame(curr_win,1,sp,NULL);
	print_window_contents(curr_win,sp,NULL);
	draw_cursor(curr_win,curr_win->physical_cursor,sp,NULL);
	print_window_bottom_help(curr_win, sp);
	print_area(sp,curr_win->xpos,curr_win->ypos,curr_win->xpos+(curr_win->chars_per_col*curr_win->columns)+curr_win->columns+2,curr_win->ypos+curr_win->height+1);



	/*loop until escape*/
	while(u!=1 && l !=27)
	{

		k=window_loop(&curr_win, auto_details.columns, auto_details.rows, auto_details.char_pixel_width , auto_details.char_pixel_height, auto_details.handle, sp);

		print_area(sp,0,0,auto_details.columns-1, auto_details.rows-1);

		if(k!=-1)	/* key press has not been handled yet */
		{
			u=(int)(k>>16)&0xff;
			l=(int)(k%256);
			s=(int)(k>>24)&0x0f;	/* we are only interested in Shift, Cntrl and Alt see page 306 for details */

			switch(u)
			{
				default:
						break;
			}
		}
		else
		{
			u=0;l=0;s=0;
		}

	}

	if(s==2)
		return 0;
	else 
		return 1;

}

void main( void );

void main( )
{
/*	int loop;*/
	FILELIST *fl;
	FILE *fp;
	SET *sp;
	char *cp;
	char curr_path[FMSIZE];
	int ferror;
	long fatt;
	int save_sets_flag=1;
	int verbose_flag=1;
/*	int rename_the_lot=1;*/
	char defs_path[FMSIZE];
	char defs_path_bak[FMSIZE];
	VT52_WINDOW *temp_win1,*temp_win2;
	char temp_ext[15];
/*	int s,u,l;*/
	SET *temp_set;
	screen *temp_screen;
	LinkedList *circular_windows,*tl;
	int *areap;
	int area[4];
	int ret;
	char file_sel_path[FMSIZE];

	areap=area;

	circular_windows=NULL;


	prgs=NULL;
	accs=NULL;
	sets=NULL;

	logical_progs_cursor=0;
	logical_accs_cursor=0;
	logical_sets_cursor=0;
	logical_other_cursor=0;

	progs_cursor=0;
	accs_cursor=0;
	sets_cursor=0;
	other_cursor=0;

	old_progs_cursor=0;
	old_accs_cursor=0;
	old_sets_cursor=0;
	old_other_cursor=0;

	first_prog_on_screen=0;
	first_acc_on_screen=0;
	first_set_on_screen=0;
	first_other_on_screen=0;
	
	curr_set=NULL;

	others=NULL;
	curr_other=NULL;

	current_window=1;
	old_window=1;


	CLEAR_SCREEN
	NORMAL_VIDEO


	set_defaults();
	start_auto_environment();
/*	end_auto_environment();*/
	prog_defaults.rows=auto_details.rows;
	prog_defaults.columns=auto_details.columns;
	line_buffer=(char *)malloc(auto_details.columns +1);

	getcd(0,curr_path);

	/*
		Set the current path
	*/
#ifndef TESTING
	if(IS_AUTO)
	{
		cp=curr_path;
		while(*cp!='\0')
			cp++;

		cp=cp-5;
		if(strcmp(cp,"\\AUTO"))
			strcat(curr_path,"\\AUTO");
	}
#else
	strcpy(curr_path,"C:\\AUTO");
#endif

	printf("curr path %s\n",curr_path);
	sprintf(defs_path,"%s\\BOOTSETS.DEF",curr_path);
	sprintf(defs_path_bak,"%s\\BOOTSETS.BAK",curr_path);
	printf("SETS PATH is %s\n",defs_path);
	while(!Bconstat(2));
	Bconin(2);

	CLEAR_SCREEN


	

	fp=fopen(defs_path,"r");
	if(fp!=NULL)
	{

		/*curr_set*/ sets=load_sets( fp );
		curr_set=(SET *)sets->node;
		fclose(fp);


	}
	else
	{
		printf("\n\nERROR COULD NOT OPEN SETS FILE %s\n",defs_path);
		BELL
		printf("\nPRESS ANY KEY\n");
			while(!Bconstat(2));
			Bconin(2);
	}

	cp=prog_defaults.auto_path;
	while(*cp!='\0')
		cp++;

	cp--;
	if(*cp!='\\')
	{
		cp++;
		*cp='\\';
		cp++;
		*cp='\0';
	}

	cp=prog_defaults.accs_path;
	while(*cp!='\0')
		cp++;

	cp--;
	if(*cp!='\\')
	{
		cp++;
		*cp='\\';
		cp++;
		*cp='\0';
	}

	ret=file_selector(auto_details.columns, auto_details.rows, "C:\\", "TEST SELECTOR",file_sel_path);
	getchar();


	temp_screen=create_screen( auto_details.rows, auto_details.columns);
	clear_screen(temp_screen);

	circular_windows=set_default_windows(curr_path,curr_set,sets);
	circular_windows_list=circular_windows;
	
	while(((VT52_WINDOW *)circular_windows->node)->on_top!=1)
		circular_windows=circular_windows->right;

	print_ordered_windows(circular_windows,circular_windows->node,temp_screen);
	print_area(temp_screen,0,0,auto_details.columns-1, auto_details.rows-1);

	main_loop2(circular_windows,temp_screen);

/*	total_progs=0;
	fl=prgs;
	while(fl)
	{
		total_progs++;
		fl=fl->right;
	}

	if(total_progs>0)
	{
		logical_progs_cursor=1;
		progs_cursor=1;
		old_progs_cursor=1;
		first_prog_on_screen=1;
	}

	total_accs=0;
	fl=accs;
	while(fl)
	{
		total_accs++;
		fl=fl->right;
	}

	if(total_accs>0)
	{
		logical_accs_cursor=1;
		accs_cursor=1;
		old_accs_cursor=1;
		first_acc_on_screen=1;
	}

	total_sets=0;
	sp=sets;
	while(sp)
	{
		total_sets++;
		sp=sp->right;
	}

	if(total_sets>0)
	{
		logical_sets_cursor=1;
		sets_cursor=1;
		old_sets_cursor=1;
		first_set_on_screen=1;
	}
*/

#ifndef MAIN_BODY_MISSING

	set_other_counts();
/*	getchar();*/

	set_progs_on_off_from_set(curr_set, prgs);
	set_accs_on_off_from_set(curr_set,accs);
	if(curr_set!=NULL)
		others=curr_set->others;
	else
		others=NULL;

	calculate_screen_sizes();
	startup_screen();

	/* this initialises the shift keys in Bconin */
	Supexec(conset);

	draw_screen();
	save_sets_flag=main_loop();

	CLEAR_SCREEN
	
	if(save_sets_flag)
	{
		
		fatt=Fattrib(defs_path_bak,0,0);

		if(fatt>=0)
			Fdelete(defs_path_bak);

		ferror=Frename(0,defs_path,defs_path_bak);
		if(verbose_flag && ferror!=0)
		{
			BELL
			printf("Could not rename .DEF file\n");
		}

		fp=fopen(defs_path,"w");
		if(fp!=NULL)
		{
			save_sets(fp);
			fclose(fp);
		}
		else
		{
			BELL
			printf("Could not save .DEF file\n");
		}

/*		if(rename_the_lot==1)
		{*/

			rename_progs_and_accs(verbose_flag);		
			process_others(verbose_flag);

/*		}*/
	}
	
	/* this sets the shift key back to what it originally was */
	Supexec(conunset);

	free(line_buffer);

#endif

	end_auto_environment();
	if(/*prog_defaults.*/old_resolution==0 /* && prog_defaults.medium_res==1 */)
	{
		Setscreen((void *)-1,(void *)-1,(short)0);
		Setcolor(1,col1);
		Setcolor(2,col2);
		Setcolor(3,col3);	
	}

	while(!Bconstat(2));
/*	end_auto_environment();*/
}












/*
+=======================+
| F1   a full set name. |
| F2 	 a full set name. |
| F3 	 a full set name. |
| F10  a full set name. |
| sF1  a full set name. |
| sF2  a full set name. |
| sF10 a full set name. |
| aF1  a full set name. |
| aF10 a full set name. |
| cF10 a full set name. |
| cF10 a full set name. |
+=======================+






+========================+
| a full set name.  F1   |
| a full set name.  F10  |
| a full set name.  sF10 |
| a full set name.  aF10 |
| a full set name.  cF10 |
| a full set name.  F10  |
+========================+


*/