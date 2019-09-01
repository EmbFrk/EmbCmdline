/*******************************************
* line editor                              *
* simple line editor with history function *
* idea behind:                             *
* one line is read in here, return value   *
* is a pointer to this commandline.        *
* This line is a copy of the real line,    *
* the interpreter which is called with     *
* this pointer will modify the content.    *
*******************************************/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <getch.h>
#include <putch.h>

#include <lineedit.h>

#include <command_definition.h>

#include "is_whitespace.h"

/* maximum length of one line */
#define CMDLINE_LENGTH	1024

/* amount of lines in the history */
#define HISTORY 8

/* 0 --> overwrite, 1 --> insert mode */
static int insert_mode = 1;

/* actual cursor position */
static int act_idx = 0;

/* last position in the actual line */
static int max_idx = 0;

/* buffer which is returned */
/* during line editor it is used to keep the */
/* actual line during history browsing       */
static char outbuffer[CMDLINE_LENGTH] = "\0";

/* memory for the history */
static char history[HISTORY][CMDLINE_LENGTH] = {"\0"};

/* memory used during edit */
static char edit_line[CMDLINE_LENGTH] = "\0";

/* indices for history browsing */
static int actual_line = 0;
static int search_line = 0;
static int lines_in_history = 0;

/* add a character in the edit line at the actual position */
void add_character(int c)
{
	int i;

	if(insert_mode) {
		/* insert mode */
		/* space left in line? */
		if(max_idx < CMDLINE_LENGTH) {
			/* yes --> move chars behind the actual */
			/* position one position further        */
			for(i = max_idx; i > act_idx; i--) {
				edit_line[i] = edit_line[i - 1];
			}
			/* then place the new cahr */
			edit_line[i] = c;
			/* and increment the amount of chars */
			max_idx++;
			/* the memory is modified now, */
			/* let's show it on the screen */

			/* print any char starting with */
			/* the actual position          */
			while(i < max_idx) {
				putch(edit_line[i]);
				i++;
			}

			/* point to the char behind the new char */
			act_idx++;

			/* restore cursor position */
			while(i > act_idx) {
				printf("\x1b[D");
				i--;
			}
		} else {
			/* no space left! */
			putch('\a');
		}
	} else {
		/* overwrite mode */

		/* print the char */
		putch(c);

		/* store the char and increment actual pos */
		edit_line[act_idx++] = c;

		/* if neccessary increment amount of chars */
		if(act_idx > max_idx) {
			/* pull max_idx */
			max_idx = act_idx;
		}
	}

	/* add trailing \0 */
	edit_line[max_idx] = '\0';
}

/* return to the first char of the line */
/* CAUTION! if your terminal does a line break this doesn't work! */
static void go_home(void)
{
	while(act_idx) {
		printf("\x1b[D");
		act_idx--;
	}
}

/* go to the end of the actual line */
/* CAUTION! if your terminal does a line break this doesn't work! */
static void go_end(void)
{
	while(act_idx < max_idx) {
		printf("\x1b[C");
		act_idx++;
	}
}

/* print the whole line */
static void show_line(char *this)
{
	int i;
	int save_maxidx;

	/* we start with the first char */
	go_home();

	/* remember the length of the actual showed line */
	save_maxidx = max_idx;

	/* we increment max_idx during printing */
	/* but we have to start with index 0    */
	max_idx = 0;

	/* print the whole line */
	for(i = 0; i < CMDLINE_LENGTH; i++) {
		if(this[act_idx] == '\0') {
			break;
		} else {
			max_idx = act_idx;
			putch(this[act_idx++]);
		}
	}

	/* overwrite rest of previous line with spaces */
	for(i = max_idx; i <= save_maxidx; i++) {
		putch(' ');
	}

	/* go back to the end of the new line */
	for(i = max_idx; i <= save_maxidx; i++) {
		printf("\x1b[D");
	}

	/* use this position as max_idx */
	max_idx = act_idx;
}

/* handing of the escape sequences                  */
/* this is called after reading an escape character */
/* therefore no parameter required                  */
void handle_esc(void)
{
	int c;
	int i;

	/* get the nex char */
	c = getch();

	/* do what has to be done */
	switch(c) {
		case '[':
			c = getch();
			switch(c) {
				case 'C':
					/* arrow right */
					if(act_idx < max_idx) {
						act_idx += 1;
						printf("\x1b[C");
					} else {
						putch('\a');
					}
					break;
				case 'D':
					/* arrow left */
					if(act_idx > 0) {
						act_idx -= 1;
						printf("\x1b[D");
					} else {
						putch('\a');
					}
					break;
				case '3':
					/* del */
					c = getch();

					if(max_idx > 0) {
						max_idx--;
						for(i = act_idx; i < max_idx; i++) {
							edit_line[i] = edit_line[i + 1];
							putch(edit_line[i]);
						}
						putch(' ');
						edit_line[i] = '\0';
						/* restore cursor position */
						while(i >= act_idx) {
							printf("\x1b[D");
							i--;
						}
					} else {
						putch('\a');
					}
					break;
				case '2':
					/* Einf */
					c = getch();
					if(insert_mode) {
						insert_mode = 0;
					} else {
						insert_mode = 1;
					}
					break;
				case '1':
					/* num 7 */
				case '4':
					/* num 1 */
				case '5':
					/* page up */
				case '6':
					/* page down */
					c = getch();
					break;
				case 'A':
					/* arrow up */
					#if HISTORY
						if(search_line > 0) {
							if(search_line == lines_in_history) {
								outbuffer[act_idx] = '\0';
								memcpy(outbuffer, edit_line, CMDLINE_LENGTH);
							}
							i = actual_line - lines_in_history + search_line - 1;
							if(i < 0) {
								i += lines_in_history;
							}
							memcpy(edit_line, history[i], CMDLINE_LENGTH);
							show_line(edit_line);
							search_line -= 1;
						} else {
							putch('\a');
						}
					#else
						putch('\a');
					#endif
					break;
				case 'B':
					/* arrow down */
					#if HISTORY
						if(search_line < lines_in_history) {
							i = actual_line + search_line + 1;
							if(i >= lines_in_history) {
								i -= lines_in_history;
							}
							search_line += 1;
							if(search_line == lines_in_history) {
								memcpy(edit_line, outbuffer, CMDLINE_LENGTH);
							} else {
								memcpy(edit_line, history[i], CMDLINE_LENGTH);
							}
							show_line(edit_line);
						} else {
							putch('\a');
						}
					#else
						putch('\a');
					#endif
					break;
				case 'H':
					/* Pos 1 */
					go_home();
					break;
				case 'F':
					/* Ende */
					go_end();
					break;
				default:
					putch('\a');
					break;
			}
			break;
		case 'O':
			c = getch();
			switch(c) {
				case 'H':
					/* Pos 1 */
					go_home();
					break;
				case 'F':
					/* Ende */
					go_end();
					break;
				default:
					/* unknown */
					break;
			}
			break;
		case 0x3c:
			add_character('|');
			break;
		default:
			/* unknown */
			break;
	}
}

/* execute a backspace */
void backspace(void)
{
	int i;
	int save_pos;

	if(act_idx > 0) {
		/* there is a char left of the actual pos */

		/* decrement actual position */
		act_idx--;
		/* and remember this */
		save_pos = act_idx;

		/* move cursor one char to the left */
		printf("\x1b[D");

		/* then overwrite to the right */
		for(i = act_idx; i < max_idx; i++) {
			putch(edit_line[i + 1]);
			edit_line[i] = edit_line[i + 1];
		}
		/* as last overwrite the last char with a blank */
		putch(' ');
		/* store the line end at the right buffer location */
		edit_line[i] = '\0';

		/* return the cursor to the stored position */
		for(i = max_idx; i > save_pos; i--) {
			printf("\x1b[D");
		}
		/* and decrement the amount of chars in the line */
		max_idx --;
	} else {
		/* cursor already at the first character */
		/* --> error, beep                       */
		putch('\a');
	}
}

/* check if a char is printable */
static bool char_valid(char c)
{
	int ret = false;

	do {
		if((c >= (int)' ') && (c <= (int)'~')) {
			ret = true;
			break;
		}
	} while(false);

	return(ret);
}

/*******************************
* the line editor itself       *
* in:  string to use as prompt *
* out: pointer to the line     *
*******************************/
char *lineedit(char *prompt)
{
	int c;
	int i;

	/* initialization */
	act_idx = 0;
	max_idx = 0;
	edit_line[0] = '\0';
	search_line = lines_in_history;

	/* print the prompt */
	printf("%s", prompt);

	do {
		/* inner loop:   */
		/* line handling */
		c = getch();
		if(char_valid(c)) {
			/* regular character */
			if(act_idx < CMDLINE_LENGTH - 1) {
				/* still space in line */
				add_character(c);
			} else {
				/* no space left! */
				/* bell           */
				putch('\a');
			}
		} else {
			/* irregular character */
			if(c > 0) {
				switch(c) {
					case '\n':
					case '\r':
						/* line end                */
						/* trigger inner loop exit */
						c = 0;
						break;
					case 0x7f:
					case 0x08:
						backspace();
						break;
					case 0x1b:
						handle_esc();
						break;
				}
			}
		}
	} while(c != 0);

	printf("\r\n");

	edit_line[max_idx] = '\0';

	#if HISTORY
		/* check for empty line (only whitespaces) */
		for(i = 0; i < max_idx; i++) {
			if(!is_whitespace(edit_line[i])) {
				break;
			}
		}

		if(i < max_idx) {
			/* line is not empty */

			/* copy the line into the history */
			/* and increment the line index   */
			/* don't forget the trailing 0    */
			memcpy(history[actual_line++], edit_line, max_idx + 1);
			if(actual_line >= HISTORY) {
				actual_line = 0;
			}
			/* as long as there are free lines */
			if(lines_in_history < HISTORY) {
				/* increment the amount of used lines */
				lines_in_history++;
			}

			/* copy the line into the outbuffer */
			memcpy(outbuffer, edit_line, max_idx + 1);
		} else {
			/* line IS empty */
			outbuffer[0] = '\0';
		}
	#else
		if(max_idx > 0) {
			memcpy(outbuffer, edit_line, max_idx + 1);
		} else {
			outbuffer[0] = '\0';
		}
	#endif

	return(outbuffer);
}
