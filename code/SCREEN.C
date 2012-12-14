#include "screen.h"
#include "vt52.h"

pixel **create_screen_array(int y, int x)
{

	pixel **pp1;
	pixel *pp2;
	int xloop,yloop;

	pp1=(pixel **)malloc(sizeof(pixel *)*y);

	if(pp1==NULL)
	{
		fprintf(stderr,"ERROR not enough space for screen array\n");
		exit(1);
	}

	for(yloop=0;yloop<y;yloop++)
	{
/*		for(xloop=0;xloop<x;xloop++)
		{*/
			pp2=(pixel *)malloc(sizeof(pixel)*x);

			if(pp2==NULL)
			{
				fprintf(stderr,"ERROR not enough space for screen array\n");
				exit(1);
			}

			pp1[yloop]=(pixel *)pp2;
/*		}*/
	}


	return pp1;
}

screen *create_screen(int y, int x)
{
	screen *sp;

	sp=(screen *)malloc(sizeof(screen));

	if(sp==NULL)
	{
		fprintf(stderr,"ERROR not enough space for screen \n");
		exit(1);
	}

	sp->width=x;
	sp->height=y;
	sp->currx=0;
	sp->curry=0;
	sp->reverse_mode=0;
	sp->screen_array=create_screen_array(y,x);

	return sp;
}

/*
void print_area(screen *sp, int x1, int y1, int x2, int y2)
{
	register int xloop;
	register int yloop;
	register int rev;
	

DISCARD_EOL
NORMAL_VIDEO


	for(yloop=y1;yloop<=y2;yloop++)
	{
		rev=0;
		AT(yloop,x1)

		for(xloop=x1;xloop<=x2;xloop++)
		{
/*			if(sp->screen_array[yloop][xloop].reverse!=rev)
			{

	/*		(rev==0)	?	{ REVERSE_VIDEO rev=1;} : { NORMAL_VIDEO rev=0;};*/

				if(rev==0)
				{
					REVERSE_VIDEO
					rev=1;
				}
				else
				{
					NORMAL_VIDEO
					rev=0;
				}
			}
*/

			if(sp->screen_array[yloop][xloop].reverse)	
					REVERSE_VIDEO  
			else
					 NORMAL_VIDEO 
			
			Bconout(2,sp->screen_array[yloop][xloop].contents);
		}
	}
NORMAL_VIDEO

WRAP_EOL
}
*/

/* optomised to build horizontal string in memory and blast it out,
	instead of a bunch of Bconouts */
void print_area(screen *sp, int x1, int y1, int x2, int y2)
{
	register int xloop;
	register int yloop;
	register int rev;
	char *string;
	char *pch;


DISCARD_EOL
NORMAL_VIDEO

	string=(char *)malloc(((x2-x1)*3)+1);
	if(string!=NULL)
	{

	for(yloop=y1;yloop<=y2;yloop++)
	{
		pch=string;
		rev=0;
		AT(yloop,x1)

		for(xloop=x1;xloop<=x2;xloop++)
		{
			if(sp->screen_array[yloop][xloop].reverse!=rev)
			{

				if(rev==0)
				{
					*pch='\033';
					pch++;
					*pch='p';
					pch++;
					/*REVERSE_VIDEO*/
					rev=1;
				}
				else
				{
					*pch='\033';
					pch++;
					*pch='q';
					pch++;
					/*NORMAL_VIDEO*/
					rev=0;
				}
			}


/*			if(sp->screen_array[yloop][xloop].reverse)	
					REVERSE_VIDEO  
			else
					 NORMAL_VIDEO 
*/
			*pch=sp->screen_array[yloop][xloop].contents;		
			pch++;


		}
		*pch='\0';

		pch=string;



		while(*pch!='\0')
			{Bconout(2,*pch);pch++;}
		NORMAL_VIDEO
	}

	free(string);
	}
	else
	{
	for(yloop=y1;yloop<=y2;yloop++)
	{
		rev=0;
		AT(yloop,x1)


		for(xloop=x1;xloop<=x2;xloop++)
		{

			if(sp->screen_array[yloop][xloop].reverse)	
					REVERSE_VIDEO  
			else
					 NORMAL_VIDEO 
			
			Bconout(2,sp->screen_array[yloop][xloop].contents);

		}
		NORMAL_VIDEO
	}
	}
/*NORMAL_VIDEO*/

WRAP_EOL
}

void clear_screen(screen *sp)
{

	register int xloop;
	register int yloop;

/*	for(yloop=0;yloop<sp->height;yloop++)
	{
		for(xloop=0;xloop<sp->width;xloop++)
		{
			sp->screen_array[yloop][xloop].reverse=0;
			sp->screen_array[yloop][xloop].contents=' ';
		}
	}
*/
/*	Optomised with pointer increments */
	pixel *pp;


	for(yloop=0;yloop<sp->height;yloop++)
	{
		pp=sp->screen_array[yloop];
		for(xloop=0;xloop<sp->width;xloop++)
		{
			(*pp).reverse=0;
			(*pp).contents=' ';
			pp++;
		}
	}

}

void	print_string(screen *sp, char *string)
{

	register int xloop;
	register int yloop;

	yloop=sp->curry;

	for(xloop=sp->currx;xloop<sp->width && *string!='\0';xloop++)
	{
		sp->screen_array[yloop][xloop].reverse=sp->reverse_mode;
		sp->screen_array[yloop][xloop].contents=*string;
		string++;
		sp->currx++;
	}
}

void	print_int(screen *sp, int i)
{

	char string[10];
	char *string_int;

	string_int=string;

	sprintf(string_int,"%d",i);
	print_string(sp,string_int);
}

void	print_long(screen *sp, long l)
{

	char string[15];
	char *string_int;

	string_int=string;

	sprintf(string_int,"%ld",l);
	print_string(sp,string_int);
}

void	print_char(screen *sp, char ch)
{

	char string[2];
	char *string_int;

	string_int=string;

	sprintf(string_int,"%c",ch);
	print_string(sp,string_int);
}

void set_reverse_mode( screen *sp)
{
	sp->reverse_mode=1;
}

void set_normal_mode( screen *sp)
{
	sp->reverse_mode=0;
}

void screen_at(screen *sp, int y, int x)
{
	sp->currx=x;
	sp->curry=y;
}

void print_screen( screen *sp)
{

if(sp)
	printf("SCREEN: width %d, height %d, x %d, y %d, rev %d\n",
				sp->width,
				sp->height,
				sp->currx,
				sp->curry,
				sp->reverse_mode);
else
	printf("SCREEN: is NULL\n");
}

/*void main(void);

void main()
{
	screen *scr;

	printf("creating screen\n");
	scr=create_screen(25,80);
	printf("created screen\n");

	clear_screen(scr);
	while(!Bconstat(2));
	Bconin(2);
	screen_at(scr,10,10);
	set_reverse_mode(scr);
	print_string(scr,"BOO HOO");
	set_normal_mode(scr);

	print_area(scr,0,0,79,24);
	getchar();
}
*/
