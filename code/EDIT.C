#include "edit.h"
#include "display.h"
#include "screen.h"

int valid_filename_char(char l)	
{

	if(	isupper(l) || islower(l) || (isdigit(l) && l!='�') ||
		 	l=='_' || l==':' || l=='.' || l=='\\' )
		return 1;

	return 0;
}

void edit_bottom_title( int screen_height )
{
	
	char temp[80];

	DISCARD_EOL

	sprintf( temp,	"CURSOR-<>, INSERT/OVERWRITE-INSERT");
	AT(screen_height-2,0)
	REVERSE_VIDEO
	centre_text(temp,line_buffer,prog_defaults.columns,' ');
	printf("%s",line_buffer);


	sprintf( temp,"ABORT EDIT-ESC, ACCEPT EDIT-RETURN");
	AT(screen_height-1,0)
	REVERSE_VIDEO
	centre_text(temp,line_buffer,prog_defaults.columns,' ');
	printf("%s",line_buffer);

	NORMAL_VIDEO
	WRAP_EOL
}
	

int edit_at(int row, int col, int max_length, int screen_length, char *string_to_edit, char filler, int fixed, int (*check_func)( char ), int convert_upper, int screen_height)
{

#define BLINK 2
#define NOBLINK 3

	char *edit_string;
	int blink,logical_cursor, cursor, last_char, insert;
	int first_char_on_screen;
	int finished, esc_pressed;
	long key;
	int upper,lower;
	char asc;
	int loop,loop2/*,delete_loop*/;
	char *blank=(char *)malloc(screen_length+1);


	edit_bottom_title(screen_height);

	for(loop=0;loop<=screen_length;loop++)
		blank[loop]=' ';

		blank[loop]='\0';

	/* set up the string - create space, copy old, pad out new with filler */
	edit_string=(char *)malloc(max_length+1);

	strcpy(edit_string,string_to_edit);

	logical_cursor=0;

	while(edit_string[logical_cursor]!='\0')
		logical_cursor++;
	
		last_char=logical_cursor;

	
/*	for(;logical_cursor<max_length;logical_cursor++)
		edit_string[logical_cursor]=filler;
*/
	edit_string[logical_cursor]='\0';

	cursor=0;
	logical_cursor=0; 
	first_char_on_screen=0;
	insert=1;
	esc_pressed=0;

	if(insert==1)
	{
		blink=BLINK;
		Cursconf(blink,0);
	}
	else
	{
		blink=NOBLINK;
		Cursconf(blink,0);
	}

	/* print string */
	print_at_from(row,col,first_char_on_screen,screen_length,edit_string);
	SHOW_CURSOR
	AT(row,col)

	finished=0;
	while(!finished)
	{	

/*		AT(0,0)
		printf(" %d ",last_char);
*/
		while(!Bconstat(2))
		;

		key=Bconin(2);
		upper=(int)(key>>16);
		lower=(int)(key%256);
		asc=(char)lower&0x00FF;

		switch(upper)
		{


			case 0x4D:	/* right arrow */
					
					if(logical_cursor<last_char/*max_length*/)
					{
						logical_cursor++;

						if(cursor<screen_length)
						{
						cursor++;
						}
						else
						{
							cursor=screen_length;
							if(first_char_on_screen+screen_length<last_char /*max_length*/ && !fixed)
							{
								first_char_on_screen++;
								print_at_from(row,col,first_char_on_screen,screen_length,edit_string);
							}
						}
					}
					else
					{
						BELL
						logical_cursor=last_char/*max_length*/;
					}

					break;

			case 0x4B: /* left arrow */
					if(logical_cursor>0)
					{
						logical_cursor--;
					}
					else
					{
						BELL
						logical_cursor=0;
					}
		
					if(cursor>0)
					{
						cursor--;
					}
					else
					{
						cursor=0;
						if(first_char_on_screen>0 && !fixed)
						{
							first_char_on_screen--;
							print_at_from(row,col,first_char_on_screen,screen_length,edit_string);
						}
					}

/*					logical_cursor--;
					cursor--;

 					if(logical_cursor<0)
					{
						BELL
						logical_cursor=0;
						cursor=0;
					}
					else
					if(cursor<0)
					{
						cursor=0;
						if(first_char_on_screen>0 && !fixed)
						{
							first_char_on_screen--;
							print_at_from(row,col,first_char_on_screen,screen_length,edit_string);
						}
					}*/
					break;

			case 0x1C: /* return */
					finished=1;
/*					print_at_from(row,col,0,screen_length,blank);
*/
					AT(row,col)
					printf("%s",blank+1);

					
/*					printf("%s",blank);*/
					AT(row,col)
					strcpy(string_to_edit,edit_string);
					print_at_from(row,col,0,screen_length,string_to_edit);
					break;

			case 0x0E:	/* backspace */
					if(logical_cursor>0)
					{
						loop=logical_cursor-1;
						loop2=logical_cursor;
						while(loop2<=last_char)
						{
							edit_string[loop]=edit_string[loop2];
							loop++;
							loop2++;
						}
						edit_string[last_char]='\0';
						last_char--;

						logical_cursor--;


						if(cursor>0 && last_char<screen_length)
						{
							cursor--;
						}
						else
						{
							if(cursor==0)
							{							
								first_char_on_screen--;
								if(first_char_on_screen<0)
									first_char_on_screen=0;
							}
							else
							{
								cursor--;
							}
						}


						if(first_char_on_screen+screen_length>last_char)
						{
/*							for(delete_loop=0;cursor+delete_loop<screen_length;delete_loop++)
							{
								AT(row,delete_loop+col)
								Bconout(2,' ');
							}*/
							AT(row,col+cursor)
							printf("%s",blank+cursor+1);

						}
	
					}
					else
						BELL
					break;

			case 0x53:	/* delete */
					if(logical_cursor<last_char)
					{
						loop=logical_cursor;
						loop2=logical_cursor+1;
						while(loop2<=last_char)
						{
							edit_string[loop]=edit_string[loop2];
							loop++;
							loop2++;
						}
						edit_string[last_char]='\0';
						last_char--;

						if(first_char_on_screen+screen_length>last_char)
						{
/*							for(delete_loop=0;cursor+delete_loop<=screen_length;delete_loop++)
							{
								AT(row,delete_loop+col)
								Bconout(2,' ');
							}
*/
							AT(row,col+cursor)
							printf("%s",blank+cursor+1);

						}
	
					}
					else
						BELL
					break;

			case 0x01: /* escape */
					finished=1;
					esc_pressed=1;
/*					print_at_from(row,col,0,screen_length,blank);
*//*					AT(row,col)
					CLEAR_EOL
*/							AT(row,col)
							printf("%s",blank+1);

/*					printf("%s",blank);
*/					AT(row,col)
					print_at_from(row,col,0,screen_length,string_to_edit);
					break;

			case 0x52: /* insert */
					BELL
					if(insert==1)
						insert=0;
					else
						insert=1;
					if(insert==1)
					{
						blink=BLINK;
						Cursconf(blink,0);
					}
					else
					{
						blink=NOBLINK;
						Cursconf(blink,0);
					}

					break;

			default:
					if((*check_func)(asc))
					{
						if(insert==1)
						{
							if(last_char<max_length)
							{

								loop=last_char-1;
								loop2=last_char;
								while(loop2>logical_cursor)
								{
									edit_string[loop2]=edit_string[loop];
									loop--;
									loop2--;
								}
								last_char++;
								edit_string[last_char]='\0';
	
								if(convert_upper)
									edit_string[logical_cursor]=toupper(asc);
								else
									edit_string[logical_cursor]=asc;

								logical_cursor++;
								cursor++;
								if(cursor>screen_length)
								{
									cursor=screen_length;
									if(first_char_on_screen+screen_length<last_char)
											first_char_on_screen++;	
								}

							}
							else
								BELL
						}
						else
						{
	
							if(logical_cursor<last_char)
							{
								if(convert_upper)
									edit_string[logical_cursor]=toupper(asc);
								else
									edit_string[logical_cursor]=asc;

								logical_cursor++;
								cursor++;
								if(cursor>screen_length)
								{
									first_char_on_screen++;
									cursor=screen_length;
								}
							}
							else
							{
								
								if(logical_cursor>=last_char && last_char<max_length)
								{
								if(convert_upper)
									edit_string[last_char]=toupper(asc);
								else
									edit_string[last_char]=asc;

									logical_cursor=last_char;
									last_char++;
									logical_cursor++;
									if(logical_cursor>max_length)
										logical_cursor=max_length;
									edit_string[last_char]='\0';							
										cursor++;
									if(cursor>screen_length)
									{
										cursor=screen_length;
										if(first_char_on_screen+screen_length<max_length)
												first_char_on_screen++;
		
									}
								}
								else
									BELL
							}
						}
					}
					break;
		}


		if(finished!=1)
			{
/*				if(last_char<screen_length)
				{
					AT(row,col)
					CLEAR_EOL
				}*/
					print_at_from(row,col,first_char_on_screen,screen_length,edit_string);
					AT(row,col+cursor)
			}


	}

	Cursconf(BLINK,0);

	HIDE_CURSOR
	return esc_pressed;
}

void print_date_at(int row, int col, char *date_field)
{
	int end;
	char *cp;
	int loop;
	AT(row,col)
	
	loop=0;
	cp=date_field;
	if(*cp=='\0')
		end=1;
	end=0;
		
	while(loop<10)
	{
		if(loop==2 || loop==5)
		{
				Bconout(2,'/');
				loop++;
		}
		else
		{	
			if(end==0)
			{
				Bconout(2,*cp);
				cp++;
				loop++;
				if(*cp=='\0')
					end=1;
			}
			else
			{
				Bconout(2,'-');
				loop++;
			}
		}	
	}
}

void edit_date_at(int row, int col, int *day, int *month, int *year)
{
	char edit_field[9]; /* 0+1 = day, 2+3 = month, 4+5+6+7=year */
	char dayc[3];
	char monthc[3];
	char yearc[5];
	
	if(*day<0 || *day>31)
		*day=1;
	if(*month<0 || *month>12)
		*month=1;
	if(*year<1980 || *year>(1980+127))
		*year=1980;
		
	sprintf(dayc,"%d",*day);
	sprintf(monthc,"%d",*month);
	sprintf(yearc,"%d",*year);
	
	if(strlen(dayc)==1)
	{
		dayc[1]=dayc[0];
		dayc[0]='0';
		dayc[2]='\0';
	}

	if(strlen(monthc)==1)
	{
		monthc[1]=monthc[0];
		monthc[0]='0';
		monthc[2]='\0';
	}

	sprintf(edit_field,"%s%s%s",dayc,monthc,yearc);

	print_date_at(row,col,edit_field);
}
	
void edit_time_at(int row, int col, int *hours, int *mins, int *secs)
{
	char edit_field[9]; /* 0+1 = hours, 2+3 = mins, 4+5=secs */
	char hoursc[3];
	char minsc[3];
	char secsc[3];
	
	if(*hours<0 || *hours>23)
		*hours=0;
	if(*mins<0 || *mins>59)
		*mins=0;
	if(*secs<0 || *secs>(59))
		*secs=0;
		
	sprintf(hoursc,"%d",*hours);
	sprintf(minsc,"%d",*mins);
	sprintf(secsc,"%d",*secs);
	
	if(strlen(hoursc)==1)
	{
		hoursc[1]=hoursc[0];
		hoursc[0]='0';
		hoursc[2]='\0';
	}

	if(strlen(minsc)==1)
	{
		minsc[1]=minsc[0];
		minsc[0]='0';
		minsc[2]='\0';
	}
	
	if(strlen(secsc)==1)
	{
		secsc[1]=secsc[0];
		secsc[0]='0';
		secsc[2]='\0';
	}
	

	sprintf(edit_field,"%s%s%s",hoursc,minsc,secsc);

	print_time_at(row,col,edit_field);
}

void print_time_at(int row, int col, char *time)
{
	int end=0;
	char *cp;
	int loop=0;
	
	AT(row,col)
	
	cp=time;
	if(*cp=='\0')
		end=1;
	
	while(loop<8)
	{
		if(loop==2 || loop==5)
		{
				Bconout(2,':');
				loop++;
		}
		else
		{	
			if(end==0)
			{
				Bconout(2,*cp);
				cp++;
				loop++;
				if(*cp=='\0')
					end=1;
			}
			else
			{
				Bconout(2,'-');
				loop++;
			}
		}	
	}

}

