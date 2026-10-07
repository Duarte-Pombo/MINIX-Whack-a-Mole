#ifndef _MOLE_H_
#define _MOLE_H_

#include <stdint.h>
#include <stdbool.h>
#include "labDrivers/video/video_gr.h"
#include <lcom/lcf.h>
#include "../projDrivers/video/video.h"
#include "../labDrivers/mouse/mouse.h"  
#include "projDrivers/keyboard/keyboard.h"
#include "projDrivers/keyboard/kbc.h"
#include "projDrivers/keyboard/i8042.h"

#define MOLE_NUM 6 
#define MOLE_WIDTH 256
#define MOLE_HEIGHT 256

struct packet; 

// global TIME UP OPTIONS (EASY / MEDIUM / HARD) -> changeable in settings
typedef enum {
    EASY = 0,
    MEDIUM = 1,
    HARD = 2
} difficulty;

typedef struct {
    int16_t x, y;
    bool hammered;
    bool visible;
    bool prev_visible;            
    int time_up; //will be randomized
    int current_time;             
    int next_appearance_time;     
    bool animation_cycle_complete;
} mole;

typedef struct {
    int16_t x, y;
} mole_position;

extern mole_position mole_positions[MOLE_NUM];

typedef struct {
    difficulty difficulty;
    bool game_over;
    bool player_won;              
} mole_game_config;

// global MOLE
extern mole moles[MOLE_NUM];
extern mole_game_config game_config;

//function declarations
void moles_init(difficulty difficulty);
void moles_update();
void mole_appear(int mole_index);
void mole_hide(int mole_index);
void mole_reset(int mole_index);
bool mole_check_collision(int16_t cursor_x, int16_t cursor_y);
void moles_draw();
bool moles_check_game_over();
void moles_cleanup();
void moles_clear_all();

bool moles_is_game_over();
bool moles_player_won();        
void moles_set_player_won();     

void moles_set_difficulty(difficulty difficulty); // called by settings state

#endif
