#ifndef _SCREEN_H_
#define _SCREEN_H_

typedef struct{
					char reverse;
					char contents;
					}pixel;


typedef struct{
					int width;
					int height;
					int currx;
					int curry;
					int reverse_mode;
					pixel **screen_array/*[]*/;
					}screen;

extern screen 	*create_screen(int, int);
extern pixel 	**create_screen_array(int, int);
extern void		print_area(screen *, int, int, int, int);
extern void 	clear_screen(screen *);
extern void		print_string(screen *, char *);
extern void		print_int(screen *, int);
extern void		print_long(screen *, long);
extern void		print_char(screen *, char);
extern void		set_reverse_mode( screen *);
extern void		set_normal_mode(screen *);
extern void		screen_at( screen *, int, int);		
extern void		print_screen( screen * );

#endif