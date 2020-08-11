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

// Конфигурация игры
struct GameConfig
{
	int size;
	char symbols[3];
};

// Символы для координаты y
extern const char symbols_coords[7];
// Символы для координаты x
extern const char digits_coords[7];

// Символы игральные
extern char symbols[4];

extern const char* is_end_word;
extern const char* is_hello_word;
extern char* enter_you_turn_or_end;
extern int max_turns;
extern int gamer_id;
extern int turn_counter;
extern struct GameConfig game_cfg;
extern uint8_t* board;

void clear_the_buffer(char* buf); //очистка буфера
void send_message_to_terminal(void); // передача данных через USB - эта функция должна вызываеться только из main
void send_to_buffer(char* buf); // буферирование вывода
bool recieve_user_input(uint8_t* Buf, uint32_t *Len); //проверка введенных пользователем данных на соответсвие формату
inline int board_size(void); // Вычисляет размер поля в клетках
void setup_board(void); // Установка начального состояния игровой доски
void send_board(void); // Вывод игровой доски в терминал
bool parse_user_turn(struct Turn* turn); // Чтения пользовательского хода
void update_board(struct Turn* turn, uint8_t symb); // Обновление поля
bool is_turn_correct(struct Turn* turn); // Проверка корректности пользовательского ввода
bool is_win(int m, int n); // Проверка победы
bool is_end(void); // Проверяет ввод "end"
bool is_hello(void); // Проверяет ввод "hello"
void send_user_turn(struct Turn* turn); // Выводит ход пользователя
void send_mk_turn(struct Turn* turn); // Выводит ход МК
int wait_user_turn_or_end(struct Turn* turn); // Ожидает ввода хода или признака завершения пользователем
void generate_mk_turn(struct Turn* turn); // Генерирует ход МК
int get_size_of_board(char* buf); // Определяет размер поля, введенный пользователем
bool waiting_for_user_symbol(void); // Ожидает ввода символа, которым будет играть пользователеь
void set_mk_symbol(void); // Задает символ, которым будет играть МК
void waiting_hello(void); // Ожидает от пользователя приветствия
void waiting_for_size(void); // Ожидает от пользователя приглашения и ввода размера доски для игры
bool setup_the_symbols_and_board(void); // Задает параметры игры и выводит начальное состояние доски
int game_process(void); // Процесс игры
void treat_user_input(void); //обработка ввода пользовательского хода
void delete_board(void); // Удаляет структуру для хранения состояния игрового поля
