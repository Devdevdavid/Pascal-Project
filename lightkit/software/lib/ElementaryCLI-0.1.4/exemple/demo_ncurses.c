#include <ncurses.h>
#include <signal.h>
#include <unistd.h>

#include "cli.h"

// Tell if main loop should keep running
static volatile int keepRunning = 1;

/**
 * @brief Handler to stop app on ctrl-c
 *
 * @param dummy unused
 */
void sigint_handler(int dummy)
{
	keepRunning = 0;
	cli_exit();
	printf("\n\r- Quitting...\n\r");
}

// ================
// CMD CALLBACKS
// ================

/**
 * @brief Trigger the SIGINT signal to request application exit
 *
 * @param argc UNUSED
 * @param argv UNUSED
 *
 * @return 0
 */
int cli_cb_exit(uint8_t argc, char * argv[])
{
	sigint_handler(SIGINT);
	return 0;
}

/**
 * @brief Default callback for leaf tokens
 *
 * @param argc Argument count
 * @param argv Argument values
 *
 * @return The status of the function
 */
int print_args(uint8_t argc, char * argv[])
{
	printf("print_args() Found %d args:\n\r", argc);
	for (uint8_t i = 0; i < argc; ++i) {
		printf("\t%s\n\r", argv[i]);
	}
	return 0;
}

/**
 * @brief Create the command line interface
 */
static int create_cli_commands(void)
{
	cli_token * tokRoot = cli_get_root_token();
	cli_token * tokLvl1;
	cli_token * tokLvl2;
	cli_token * curTok;

	// Create commands
	curTok = cli_add_token("exit", "Exit application");
	cli_set_callback(curTok, &cli_cb_exit);
	cli_add_children(tokRoot, curTok);

	tokLvl1 = cli_add_token("flash", "Manage flash memory");
	{
		curTok = cli_add_token("default", "Reset flash setting to default");
		cli_set_callback(curTok, &print_args);
		cli_add_children(tokLvl1, curTok);
	}
	cli_add_children(tokRoot, tokLvl1);

	tokLvl1 = cli_add_token("lan", "LAN configuration");
	{
		curTok = cli_add_token("show", "[interface] Show configuration");
		cli_set_callback(curTok, &print_args);
		cli_set_argc(curTok, 0, 1);
		cli_add_children(tokLvl1, curTok);

		tokLvl2 = cli_add_token("set", "Define new configuration");
		{
			curTok = cli_add_token("ip", "<address> Set IP adress");
			cli_set_callback(curTok, &print_args);
			cli_set_argc(curTok, 1, 0);
			cli_add_children(tokLvl2, curTok);

			curTok = cli_add_token("gateway", "<address> Set gateway adress");
			cli_set_callback(curTok, &print_args);
			cli_set_argc(curTok, 1, 0);
			cli_add_children(tokLvl2, curTok);

			curTok = cli_add_token("mask", "<address> Set network mask");
			cli_set_callback(curTok, &print_args);
			cli_set_argc(curTok, 1, 0);
			cli_add_children(tokLvl2, curTok);
		}
		cli_add_children(tokLvl1, tokLvl2);
	}
	cli_add_children(tokRoot, tokLvl1);
	return 0;
}

int main(void)
{
	uint8_t byte;

	// Init signal handler
	signal(SIGINT, sigint_handler);

	// Init ncurses stuff
	initscr();
	endwin();
	noecho();
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	timeout(100);

	// Using getch now allows us to see firsts printf()
	getch();

	// motd
	printf("Exemple using %s\n\r", cli_get_version());

	cli_init();
	create_cli_commands();

	while (keepRunning) {
		byte = getch();
		if (byte == 0xFF) {
			continue;
		}
		cli_rx(byte);
	}

	endwin(); // Restore terminal to previous state

	return 0;
}