#include <TickTackToe.h>

char user_input[21];
int state_game = HELLO;
int need_to_input = NO_INVITE;

char message_to_send[1024];
char buf_for_field[512];

const char* is_end_word = "end";
const char* is_hello_word = "hello";
char* enter_you_turn_or_end = "������� ��� ��� (��� end ��� ����������)\r\n";
int max_turns = 0;
int gamer_id = USER_ID;
int turn_counter = 0;
struct GameConfig game_cfg;
uint8_t* board = NULL;

const char symbols_coords[7] = 
{
	'a', 'b', 'c', 'd', 'e', 'f', 'g'
};

const char digits_coords[7] = 
{
	'1', '2', '3', '4', '5', '6', '7'
};

char symbols[4] = 
{
	'X', 'x', 'O', 'o'
};

//������� ������
void clear_the_buffer(char* buf)
{
	memset(buf, '\0', strlen(buf));
}

// �������� ������ ����� USB - ��� ������� ������ ����������� ������ �� main
void send_message_to_terminal(void)
{
	CDC_Transmit_FS((unsigned char*)(message_to_send), strlen(message_to_send));
	HAL_Delay(10);
	clear_the_buffer(message_to_send);
}

// ������������� ������
void send_to_buffer(char* buf)
{
  strcat(message_to_send, buf);
}

//�������� ��������� ������������� ������ �� ����������� �������
bool recieve_user_input(uint8_t* Buf, uint32_t *Len)
{
	strncpy(user_input,(char*)Buf,*Len);
	user_input[*Len]=0;
	return (strstr(user_input, "\\r\\n"));
}


// ��������� ������ ���� � �������
inline int board_size(void)
{
  return game_cfg.size*game_cfg.size;
}

// ��������� ���������� ��������� ������� �����
void setup_board(void)
{
  board = (uint8_t*)malloc(board_size()*sizeof(uint8_t));
  if (board == NULL) 
  {
	send_to_buffer("Error: Out of Memory!\r\n");
	while(1) {};
  }
  memset(board, FIELD_EMPTY, board_size());
}

// ����� ������� ����� � ��������
void send_board(void)
{
	clear_the_buffer(buf_for_field);
	buf_for_field[0] = ' ';
	for(int i = 0; i < game_cfg.size; i++)
		buf_for_field[i+1] = symbols_coords[i];
	buf_for_field[strlen(buf_for_field)] = ' ';
	strcat(buf_for_field, "\r\n");
	
	memset(&buf_for_field[strlen(buf_for_field)], '-', game_cfg.size+3);
	strcat(buf_for_field, "\r\n");
	for (int i = 0; i < game_cfg.size; i++)
	{
		buf_for_field[strlen(buf_for_field)] = '|';
		for(int j = 0; j <  game_cfg.size; j++)
			buf_for_field[strlen(buf_for_field)] = game_cfg.symbols[BOARD(i, j, game_cfg.size)];
		buf_for_field[strlen(buf_for_field)] = '|';
		buf_for_field[strlen(buf_for_field)] = digits_coords[i];
		strcat(buf_for_field, "\r\n");
	}

	memset(&buf_for_field[strlen(buf_for_field)], '-', game_cfg.size+3);
	strcat(buf_for_field, "\r\n");
	send_to_buffer(buf_for_field);
	clear_the_buffer(buf_for_field);
}

// ������ ����������������� ����
bool parse_user_turn(struct Turn* turn)
{
	char* symbol;

	symbol = (char*)memchr(symbols_coords, user_input[0], sizeof(symbols_coords));
	if(!symbol)
		return false;
	turn->i = (int)((uint32_t)symbol-(uint32_t)symbols_coords);
	
	symbol = (char*)memchr(digits_coords, user_input[1], sizeof(digits_coords));
	if(!symbol)
		return false;
	turn->j = (int)((uint32_t)symbol-(uint32_t)digits_coords);
	
	return true;
}

// ���������� ����
void update_board(struct Turn* turn, uint8_t symb)
{
	BOARD(turn->i,turn->j, game_cfg.size) = symb;
}

// �������� ������������ ����������������� �����
bool is_turn_correct(struct Turn* turn)
{
	return (BOARD(turn->i,turn->j, game_cfg.size) == FIELD_EMPTY);
}

// �������� ������
bool is_win(int m, int n)
{
	int c = 0;
	uint8_t sym = BOARD(m, n, game_cfg.size);
	
	c = 0;
	for (int i = 0; i < game_cfg.size; i++)
		if (BOARD(m, i, game_cfg.size) == sym)
			c++;
	if (c == game_cfg.size)
		return true;
	
	c = 0;
	for (int i = 0; i < game_cfg.size; i++)
		if (BOARD(i, n, game_cfg.size) == sym)
			c++;
	if (c == game_cfg.size)
		return true;

    if(m == n) {
		c = 0;
		for (int i = 0; i < game_cfg.size; i++)
			if (BOARD(i, i, game_cfg.size) == sym)
				c++;
		if (c == game_cfg.size)
			return true;
	}
	
	if(game_cfg.size-n-1 == m) {
		c = 0;
		for (int i = 0; i < game_cfg.size; i++)
			if (BOARD(m, game_cfg.size-i-1, game_cfg.size) == sym)
				c++;
		if (c == game_cfg.size)
			return true;
	}
	return false;
}

// ��������� ���� "end"
bool is_end(void)
{
	return (0 == strncmp(user_input, is_end_word, strlen(is_end_word))); 
}

// ��������� ���� "hello"
bool is_hello(void)
{
	return (0 == strncmp(user_input, is_hello_word, strlen(is_hello_word))); 
}

// ������� ��� ������������
void send_user_turn(struct Turn* turn)
{
	sprintf(buf_for_field, "��� ���: %c%c; \r\n", symbols_coords[turn->i], digits_coords[turn->j]);
	send_to_buffer(buf_for_field);
	send_board();
  send_to_buffer("\r\n");
}

// ������� ��� ��
void send_mk_turn(struct Turn* turn)
{
	sprintf(buf_for_field, "��� ���: %c%c; \r\n", symbols_coords[turn->i], digits_coords[turn->j]);
	send_to_buffer(buf_for_field);
	send_board();
}

// ������� ����� ���� ��� �������� ���������� �������������
int wait_user_turn_or_end(struct Turn* turn)
{
	if (is_end()) 
		return REQ_END_GAME;
	if (!parse_user_turn(turn))
		return NEED_INVITE;
	if (is_turn_correct(turn))
		return REQ_NEXT_TURN;
	return NEED_INVITE;
}

// ���������� ��� ��
void generate_mk_turn(struct Turn* turn)
{
	while (1)
	{
		turn->i = rand() % game_cfg.size;
		turn->j = rand() % game_cfg.size;
		if (is_turn_correct(turn))
			return;
	}
}

// ���������� ������ ����, ��������� �������������
int get_size_of_board(char* buf) 
{
	return atoi(buf);
}

// ������� ����� �������, ������� ����� ������ �������������
bool waiting_for_user_symbol(void)
{
	char* symbol = (char*)memchr(symbols, user_input[0], sizeof(symbols));
	if(symbol != NULL)
	{
		game_cfg.symbols[0] = *symbol;
		return true;
	}
	return false;
}

// ������ ������, ������� ����� ������ ��
void set_mk_symbol(void)
{
	switch (game_cfg.symbols[0])
	{
		case 'X' : 
			game_cfg.symbols[1] = 'O';
			break;
		case 'x' : 
			game_cfg.symbols[1] = 'o';
			break;
		case 'O' : 
			game_cfg.symbols[1] = 'X';
			break;
		case 'o' : 
			game_cfg.symbols[1] = 'x';
			break;
	}
}

// ������� �� ������������ �����������
void waiting_hello(void)
{
	if (!is_hello())
		return;
	state_game = SIZE;
}

// ������� �� ������������ ����������� � ����� ������� ����� ��� ����
void waiting_for_size(void)
{
	game_cfg.size = 0;
	game_cfg.symbols[0] = 0;
	game_cfg.symbols[1] = 0;
	game_cfg.symbols[2] = ' ';
	
	char p = user_input[0];
	game_cfg.size = get_size_of_board(&p);
	if (game_cfg.size == 0)
		return;
		
	max_turns = board_size();
	
	state_game = SYMBOL;
	sprintf(buf_for_field, "�������� ������, ������� �� ������ ������ (%s): \r\n", symbols);
	send_to_buffer(buf_for_field);
}

// ������ ��������� ���� � ������� ��������� ��������� �����
bool setup_the_symbols_and_board(void)
{
	if (!waiting_for_user_symbol())
		return false;
	set_mk_symbol();
	setup_board();
	send_board();
	gamer_id = (rand() % 2 ? MK_ID : USER_ID);
	return true;
}

// ������� ����
int game_process(void)
{
	struct Turn turn;
	
	if (need_to_input == NEED_INVITE)
	{
		send_to_buffer(enter_you_turn_or_end);
		need_to_input = NO_INVITE;
		return NEW_INPUT;
	}
	
	if (gamer_id == MK_ID)
	{
		generate_mk_turn(&turn);
		update_board(&turn, gamer_id);
		send_mk_turn(&turn);
		state_game = NEED_INVITE;
	}
	else
	{
		switch(wait_user_turn_or_end(&turn))
		{
		  case REQ_END_GAME:
				return HELLO;
		  case NEED_INVITE:
				need_to_input = NEED_INVITE;
				return NEW_INPUT;
		}
		update_board(&turn, gamer_id);
		send_user_turn(&turn);
	}
	
	turn_counter++;
		
	if(is_win(turn.i, turn.j))
	{
		if (gamer_id == USER_ID)
			send_to_buffer("�� ��������!\r\n");
		else
			send_to_buffer("�� ���������!\r\n");
		return SIZE;
	}
	if (turn_counter == max_turns)
	{
		send_to_buffer("�����!\r\n");
		return SIZE;
	}
	gamer_id = (gamer_id == USER_ID ? MK_ID : USER_ID);
	return NEW_INPUT;
}

void treat_user_input(void)
{
	switch(state_game)
			{
				case HELLO:
					waiting_hello();
					break;

				case SIZE:
					waiting_for_size();
					break;

				case SYMBOL:
					if (!setup_the_symbols_and_board())
						break;
					if (gamer_id == USER_ID)
						need_to_input = NEED_INVITE;
					state_game = game_process();
					break;

				case NEW_INPUT:
					for(int j = 0; j < 2; j++)
				  {
						state_game = game_process();
						if(state_game != NEW_INPUT)
						{
							delete_board();
							break;
						}
				  }	
			}
}

// ������� ��������� ��� �������� ��������� �������� ����
void delete_board(void)
{
	free(board);
	board = NULL;
}
