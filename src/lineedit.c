#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <getch.h>
#include <putch.h>

#include <lineedit.h>

#include <command_definition.h>

#include "is_whitespace.h"

#define CMDLINE_LENGTH	1024
#define HISTORY 8

static int insert_mode = 1;

static int act_idx = 0;
static int max_idx = 0;

static char outbuffer[CMDLINE_LENGTH] = "\0";
static char history[HISTORY][CMDLINE_LENGTH] = {"\0"};
static char edit_line[CMDLINE_LENGTH] = "\0";
static int actual_line = 0;
static int search_line = 0;
static int lines_in_history = 0;

void add_character(int c)
{
	int i;

	if(insert_mode) {
		if(max_idx < CMDLINE_LENGTH) {
			for(i = max_idx; i > act_idx; i--) {
				edit_line[i] = edit_line[i - 1];
			}
			edit_line[i] = c;
			max_idx++;

			while(i < max_idx) {
				putch(edit_line[i]);
				i++;
			}

			act_idx++;

			/* restore cursor position */
			while(i > act_idx) {
			printf("\x1b[D");
			i--;
            }

		} else {
			putch('\a');
		}
	} else {
		putch(c);
		edit_line[act_idx++] = c;
		if(act_idx > max_idx) {
			/* pull max_idx */
			max_idx = act_idx;
			/* add trailing \0 */
		}
	}
	edit_line[max_idx] = '\0';
}

static void go_home(void)
{
	while(act_idx) {
		printf("\x1b[D");
		act_idx--;
	}
}

static void go_end(void)
{
	while(act_idx < max_idx) {
		printf("\x1b[C");
		act_idx++;
	}
}

static void show_line(char *this)
{
	int i;
	int save_maxidx;

	go_home();

	save_maxidx = max_idx;
	max_idx = 0;
	for(i = 0; i < CMDLINE_LENGTH; i++) {
		if(this[act_idx] == '\0') {
			break;
		} else {
			max_idx = act_idx;
			putch(this[act_idx++]);
		}
	}
	for(i = max_idx; i <= save_maxidx; i++) {
		putch(' ');
	}
	for(i = max_idx; i <= save_maxidx; i++) {
		printf("\x1b[D");
	}
	max_idx = act_idx;
}

void handle_esc(void)
{
	int c;
	int i;

	c = getch();

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

void backspace(void)
{
	int i;
	int save_pos;

	if(act_idx > 0) {
		act_idx--;
		save_pos = act_idx;

		printf("\x1b[D");

		for(i = act_idx; i < max_idx; i++) {
			putch(edit_line[i + 1]);
			edit_line[i] = edit_line[i + 1];
		}
		putch(' ');
		edit_line[i] = '\0';

		for(i = max_idx; i > save_pos; i--) {
			printf("\x1b[D");
		}
		max_idx --;
	} else {
		putch('\a');
	}
}

static bool char_valid(char c)
{
	int ret = false;

	do {
		if((c >= (int)' ') && (c <= (int)'~')) {
			ret = true;
			break;
		}

		if(c == '*') {
			ret = true;
			break;
		}
	} while(false);

	return(ret);
}

char *lineedit(char *prompt)
{
	int c;
	int i;

	act_idx = 0;
	max_idx = 0;
	edit_line[0] = '\0';

	search_line = lines_in_history;

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
	// check for empty line
	for(i = 0; i < max_idx; i++) {
		if(!is_whitespace(edit_line[i])) {
			break;
		}
	}

	if(i < max_idx) {
		// line is not empty
		max_idx++;
		memcpy(history[actual_line++], edit_line, max_idx);
		if(actual_line >= HISTORY) {
			actual_line = 0;
		}
		if(lines_in_history < HISTORY) {
			lines_in_history++;
		}
		memcpy(outbuffer, edit_line, max_idx);
	}
#else
	memcpy(outbuffer, edit_line, max_idx);
#endif

	return(outbuffer);
}

static char const exit_name[] =	"exit";

static int exit_fkt(int argc, const char *argv[]);
static char exit_help[] = "exit the interpreter";

const struct one_command cmd_exit = {
	.name = exit_name,
	.help_short = exit_help,
	.help = exit_help,
	.fkt = exit_fkt,
	.parameter = NULL,
};

volatile int leave_interpreter = 0;

int exit_fkt(int argc, const char *argv[])
{
	leave_interpreter = 1;

	return(1);
}
