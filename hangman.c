#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

void newgame (char **text, int last, int maxy, int maxx);

void draw_hanger (WINDOW * game) {
	int i;
	for (i = 2; i <= 17; i++)
		mvwprintw (game, i, 2, "#");
	for (i = 3; i <= 7; i++)
		mvwprintw (game, 2, i, "#");
	for (i = 3; i <= 5; i++)
		mvwprintw (game, i, 7, "#");
	mvwprintw (game, 17, 1, "#");
	mvwprintw (game, 17, 3, "#");
	wrefresh (game);
}

void get_time (WINDOW * cpanel) {
	time_t t;
	time (&t);
	struct tm *ctime = localtime (&t);
	mvwprintw (cpanel, 1, 1, "          ");
	mvwprintw (cpanel, 1, 1, "%d/%d/%d", ctime->tm_mday, ctime->tm_mon + 1, ctime->tm_year + 1900);
	mvwprintw (cpanel, 2, 1, "        ");
	mvwprintw (cpanel, 2, 1, "%d:%d:%d", ctime->tm_hour, ctime->tm_min, ctime->tm_sec);
	wrefresh (cpanel);
}


void draw_menu (char **text, int last, int finished) {
	int maxy, maxx;
	getmaxyx (stdscr,maxy,maxx);
	WINDOW * menu = newwin (9, 16, maxy/2-4, maxx/2-7);
	refresh();
	box (menu, 0, 0);
	mvwprintw (menu, 0, 4, "HANGMAN!");
	mvwprintw (menu, 2, 3, "MENU:");
	mvwprintw (menu, 4, 3, "New Game");
	if (finished) {
		wattron (menu, A_DIM);
		mvwprintw (menu, 5, 3, "Resume Game");
		wattroff (menu, A_DIM);
	}
	else {
		mvwprintw (menu, 5, 3, "Resume Game");
	}
	mvwprintw (menu, 6, 3, "Quit");
	wrefresh (menu);
	keypad (menu, true);
	mvwaddch (menu, 4, 2, '>');
	wrefresh (menu);
	int pos;
	pos = 0;
	int input;
	while (1) {
		input = wgetch (menu);
		if (input == KEY_UP && pos != 0) {
			mvwaddch (menu, pos + 4, 2, ' ');
			mvwaddch (menu, pos + 3 - finished, 2, '>');
			wrefresh (menu);
			pos = pos - 1 - finished;
		}
		else {
			if (input == KEY_DOWN && pos != 2) {
				mvwaddch (menu, pos + 4, 2, ' ');
				mvwaddch (menu, pos + 5 + finished, 2, '>');
				wrefresh (menu);
				pos = pos + 1 + finished;
			}
		}

		if (input == 10) {
			break;
		}

	}

	if (pos == 1) {
		if (finished) {
			mvwprintw (stdscr, 0, 0, "There is no game to resume.");
			wrefresh (stdscr);
			draw_menu (text, last, 1);
		}
		//ramas de facut
	}
	if (pos == 0) {
		clear();
		newgame(text, last, maxy, maxx);
		
	}
}

char *replace_ch (char *original, char* mod, int len, char ch) {
	int i;
	for (i = 0; i < len; i++) {
		if(original [i] == ch) {
			mod [i] = ch;
		}
	}
	return mod;
}

char *init_line(char *line) {
	int len;
	len = strlen (line);
	char *new;
	new = (char*)malloc((len+1) * sizeof(char));
	int i;
	for (i = 0; i < len; i++) {
		if (line [i] == ' ') {
			new [i] = ' ';
		}
		else {
			new [i] = '_';
		}
	}
	new [len] = '\0';
	for (i = 0; i < len; i++) {
		if (new [i] == '_'){
			if (new [i - 1] != ' ' && new [i - 2] != ' ' && i >= 2 && (new [i + 1] == ' ' || new [i + 1] == '\0')) {
				new [i] = line [i];
				new = replace_ch (line, new, len, line [i]);
				//cauta litera si prin restu liniei
			}
			if ((new [i - 1] == ' ' || i < 1) && new [i + 1] != ' ' && new [i + 1] != '\0') {
				new [i] = line [i];
				new = replace_ch (line, new, len, line [i]);
			}
		}
	}
	return new;
}

int check_ch (int input, char *line, int len) {
	int i;
	for (i = 0; i < len; i++) {
		if (input == line [i]) {
			return 1;
		}
	}
	return 0;
}

void draw_man(int x, WINDOW * game, WINDOW * cpanel) {
	if (x == 0) {
		mvwprintw (game, 6, 7, "@");
		mvwprintw (game, 7, 6, "@");
		mvwprintw (game, 7, 7, "@");
		mvwprintw (game, 7, 8, "@");
		mvwprintw (game, 8, 7, "@");
	}
	if (x == 1) {
		mvwprintw (game, 9, 7, "@");
		mvwprintw (game, 10, 7, "@");
		mvwprintw (game, 11, 7, "@");
		mvwprintw (game, 12, 7, "@");
	}
	if (x == 2) {
		mvwprintw (game, 9, 5, "@");
		mvwprintw (game, 10, 6, "@");
	}
	if (x == 3) {
		mvwprintw (game, 9, 9, "@");
		mvwprintw (game, 10, 8, "@");
	}
	if (x == 4) {
		mvwprintw (game, 13, 6, "@");
		mvwprintw (game, 14, 5, "@");
	}
	if (x == 5) {
		mvwprintw (game, 13, 8, "@");
		mvwprintw (game, 14, 9, "@");
	}
	if (x == 6) {
		mvwprintw (game, 16, 5, "GAME OVER");
		mvwprintw (cpanel, 7, 1, "Press N for a new game.");
	}
}

void newgame (char **text, int last, int maxy, int maxx) {
	WINDOW * game = newwin (maxy, maxx/2, 0, 0);
	WINDOW * cpanel = newwin (maxy, maxx/2 - 1, 0, maxx/2 + 1);
	refresh();
	box (game, 0, 0);
	box (cpanel, 0, 0);
	wrefresh (game);
	wrefresh (cpanel);
	draw_hanger (game);
	get_time (cpanel);
	int ch_nr = 0;
	int i;
	for (i = 0; i <= last; i++) {
		ch_nr = ch_nr + strlen (text[i]);
	}
	mvwprintw (cpanel, 3, 1, "Press Home Key to sync time.");
	mvwprintw (cpanel, 4, 1, "Press Q to return to menu.");
	mvwprintw (cpanel, 5, 1, "Valid lines read: %i", (last + 1));
	mvwprintw (cpanel, 6, 1, "Total chars read: %d", ch_nr);
	char *new;
	int len;

	srand (time (NULL));
	int r;
	r = (rand() % (last + 1));
	len = strlen (text[r]);

	new = (char *)malloc((len+1) * sizeof (char));
	new = init_line(text [r]);

	mvwprintw (game, 1, 1, " %s", new);
	wrefresh (game);
	keypad (cpanel, true);
	int input;
	int inputg;
	char *wrong;
	wrong  = (char *)malloc (sizeof (char));
	wrong [0] = '\0'; 
	int x = 1;
	int x_tried = 1;
	int finished = 0;
	while(true) {
		input = wgetch (cpanel);
		if (input == KEY_HOME){
			get_time (cpanel);
		}
		else{
			if (input == 'Q') {
				clear();
				draw_menu (text, last, finished);
				break;
			}
			else {
				if (check_ch (input, text [r], len)) {
					new = replace_ch (text[r], new, len, input);
					mvwprintw (game, 1, 1, " %s", new);
					if (!(check_ch ('_', new, len))){
						mvwprintw (game, 16, 5, "YOU WIN");
						mvwprintw (cpanel, 7, 1, "Press N for a new game.");
						finished = 1;
						x = 8;
					}
					wrefresh (game);
					wrefresh (cpanel);
				}
				else {
					if (x >= 8 && input == 'N') {
						newgame(text, last, maxy, maxx);
						break;
					}
					else {
						if (x >= 8) {
							finished = 1;
							continue;
						}
					}
					x++;
					wrong = (char *)realloc (wrong, x_tried * sizeof (char));
					if (!(check_ch (input, wrong, strlen (wrong)))){
						x_tried++;
						wrong [x_tried - 2] = input;
						wrong [x_tried - 1] = '\0';
					}
					mvwprintw (game, 19, 1, " %s", wrong);
					draw_man((x - 2), game, cpanel);
					wrefresh (game);
					wrefresh (cpanel);
				}
			}
		}
	}
	free (wrong);
	free (new);
}


char **add_line (char **text, int *last, int *size, char *line) {
	*last = *last + 1;

	if (*last >= *size - 1) {
		*size = (*size) * 2;
		text = (char **)realloc (text, (*size) * sizeof (char*));
	}

	text[*last] = line;
	return text;
}

char *add_char (char *line, int *last, int *size, char c) {
	*last = *last + 1;

	if (*last >= *size - 1) {
		*size = (*size) * 2;
		line = (char *)realloc (line, (*size) * sizeof (char));
	}

	line[*last] = c;
	line[*last + 1] = '\0';
	return line;
}

// Deschide si citeste toate liniile dintr-un fisier
// apoi le adauga in <text>
char **read_file (char *filename, char **text, int *last, int *size) {
	FILE *file = fopen (filename, "r");
	
	int last_char = -1;
	int line_size = 5;

	char *line = (char *)malloc (line_size * sizeof (char));
	char c;
	// Procesare
    while ((c = (char)fgetc(file)) != EOF) {
    	if (c == '\n') {
    		if (last_char >= 0) {
    			text = add_line (text, last, size, line);
    		}
    		else {
    			free (line);
    		}
    		last_char = -1;
    		line_size = 5;
			line = (char *)malloc (line_size * sizeof (char));
    		continue;
    	}

    	if (isprint (c)) {
    		if (isalpha(c)) {
    			c = tolower (c);
    		}
    		line = add_char (line, &last_char, &line_size, c);
    	}
    }
    if (last_char >= 0) {
    			text = add_line (text, last, size, line);
    		}
    		else {
    			free (line);
    		}
    return text;
}

int main (int argc, char **argv) {
	initscr();
	noecho();
	cbreak();
	if (argc < 2) {
		mvwprintw (stdscr, 1, 1, "[Eroare]: Nu s-au dat argumente  de comanda.");
		wrefresh (stdscr);
		getch();
		endwin();
		return 1;
	}
	int i;
	int x = 1;
	int works = 1;
	char **text;
	FILE *input;
	int last = -1;
	int size = 5;
	text = (char **)malloc(size * sizeof(char *));
	for (i = 0; i < argc - 1; i++) {
		input = fopen (argv [i + 1], "r");
		if (!input) {
			mvwprintw (stdscr, x, 1, "[Eroare]: Fisierul %s nu poate fi deschis.", argv[i + 1]);
			x++;
			wrefresh (stdscr);
			works = 0;
		}
		fclose (input);
	}
	if (!works) {
		getch();
		endwin();
		return 1;
	}
	else {
		for (i = 0; i < argc - 1; i++) {
			text = read_file(argv [i + 1], text, &last, &size);
		}
	}


	draw_menu(text, last, 1);	
	endwin();
	return 0;
}