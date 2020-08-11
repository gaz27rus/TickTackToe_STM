#include "usbd_cdc_if.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HELLO	 0
#define SIZE 1
#define SYMBOL 2
#define NEW_INPUT 3

#define NO_INVITE 0
#define NEED_INVITE 5

#define REQ_END_GAME  0
#define REQ_NEXT_TURN 1

#define USER_ID 0
#define MK_ID   1

#define FIELD_USER  USER_ID
#define FIELD_MK    MK_ID
#define FIELD_EMPTY 2
#define BOARD(i,j, size) board[(j)+(i)*(size)]

extern char user_input[21];
extern int state_game;
extern int need_to_input;

extern char message_to_send[1024];
extern char buf_for_field[512];

struct Turn
{
	int i, j;
};

// ������������ ����
struct GameConfig
{
	int size;
	char symbols[3];
};

// ������� ��� ���������� y
extern const char symbols_coords[7];
// ������� ��� ���������� x
extern const char digits_coords[7];

// ������� ���������
extern char symbols[4];

extern const char* is_end_word;
extern const char* is_hello_word;
extern char* enter_you_turn_or_end;
extern int max_turns;
extern int gamer_id;
extern int turn_counter;
extern struct GameConfig game_cfg;
extern uint8_t* board;

void clear_the_buffer(char* buf); //������� ������
void send_message_to_terminal(void); // �������� ������ ����� USB - ��� ������� ������ ����������� ������ �� main
void send_to_buffer(char* buf); // ������������� ������
bool recieve_user_input(uint8_t* Buf, uint32_t *Len); //�������� ��������� ������������� ������ �� ����������� �������
inline int board_size(void); // ��������� ������ ���� � �������
void setup_board(void); // ��������� ���������� ��������� ������� �����
void send_board(void); // ����� ������� ����� � ��������
bool parse_user_turn(struct Turn* turn); // ������ ����������������� ����
void update_board(struct Turn* turn, uint8_t symb); // ���������� ����
bool is_turn_correct(struct Turn* turn); // �������� ������������ ����������������� �����
bool is_win(int m, int n); // �������� ������
bool is_end(void); // ��������� ���� "end"
bool is_hello(void); // ��������� ���� "hello"
void send_user_turn(struct Turn* turn); // ������� ��� ������������
void send_mk_turn(struct Turn* turn); // ������� ��� ��
int wait_user_turn_or_end(struct Turn* turn); // ������� ����� ���� ��� �������� ���������� �������������
void generate_mk_turn(struct Turn* turn); // ���������� ��� ��
int get_size_of_board(char* buf); // ���������� ������ ����, ��������� �������������
bool waiting_for_user_symbol(void); // ������� ����� �������, ������� ����� ������ �������������
void set_mk_symbol(void); // ������ ������, ������� ����� ������ ��
void waiting_hello(void); // ������� �� ������������ �����������
void waiting_for_size(void); // ������� �� ������������ ����������� � ����� ������� ����� ��� ����
bool setup_the_symbols_and_board(void); // ������ ��������� ���� � ������� ��������� ��������� �����
int game_process(void); // ������� ����
void treat_user_input(void); //��������� ����� ����������������� ����
void delete_board(void); // ������� ��������� ��� �������� ��������� �������� ����
