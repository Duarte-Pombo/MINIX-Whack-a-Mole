#include "mole.h"
#include "uiElements/cursor.h"
#include "labDrivers/video/video_gr.h"
#include <stdlib.h>
#include <time.h>

mole moles[MOLE_NUM];
mole_game_config game_config = {MEDIUM, false, false};

extern uint8_t* moleUp_pixmap;
extern xpm_image_t moleUp_img;
extern uint8_t* moleDown_pixmap;
extern xpm_image_t moleDown_img;

// mole pos
mole_position mole_positions[MOLE_NUM] = {
    {150, 200},  
    {400, 200},  
    {650, 200},  
    {150, 400},  
    {400, 400},  
    {650, 400}   
};

static const int time_up_durations[3][2] = {
    {180, 300}, // EASY 3-5 secs
    {120, 240}, // MEDIUM 2-4 secs
    {60, 180}   // HARD 1-3 secs
};

// Helper function to clear mole area with background
void mole_clear_area(int mole_index) {
    if (mole_index < 0 || mole_index >= MOLE_NUM) return;
    
}

void moles_init(difficulty difficulty) {
    game_config.difficulty = difficulty;
    game_config.game_over = false;
    game_config.player_won = false;

    srand(time(NULL));
    
    // init moles
    for (int i = 0; i < MOLE_NUM; i++) {
        moles[i].x = mole_positions[i].x;
        moles[i].y = mole_positions[i].y;
        moles[i].hammered = false;
        moles[i].visible = false;
        moles[i].prev_visible = false; 
        moles[i].time_up = 0;
        moles[i].current_time = 0;
        moles[i].animation_cycle_complete = false;
        
        // rand next appearance
        moles[i].next_appearance_time = rand() % 300 + 60; // 1-5 sec delay
        
        mole_clear_area(i);
    }
}

void moles_update() {
    printf("Difficulty set to %s\n", 
        game_config.difficulty == EASY ? "EASY" : 
        game_config.difficulty == MEDIUM ? "MEDIUM" : "HARD");
    if (game_config.game_over) return;
    
    for (int i = 0; i < MOLE_NUM; i++) {
        mole* currMole = &moles[i];
        
        // store prev state
        bool was_visible = currMole->visible;
        
        if (!currMole->visible && !currMole->hammered) {
            // if mole hiding, check if its time to show up
            currMole->next_appearance_time--;
            if (currMole->next_appearance_time <= 0) {
                mole_appear(i);
            }
        }
        else if (currMole->visible && !currMole->hammered) {
            // if mole up, count down timer
            currMole->current_time++;
            // check if mole should go down
            if (currMole->current_time >= currMole->time_up) {
                // clear previous state before hiding
                if (was_visible) {
                    mole_clear_area(i);
                }
                
                // hide mole
                mole_hide(i);
                currMole->animation_cycle_complete = true;
                
                // check if mole was hit and trigger gameover if not
                if (!currMole->hammered) {
                    // game over
                    game_config.game_over = true;
                    game_config.player_won = false;
                    return;
                }
            }
        }
        else if (currMole->hammered) {
            currMole->current_time++;
            if (currMole->current_time >= 60) { // 1 sec delay
                // clear hammered state before reset
                mole_clear_area(i);
                mole_reset(i);
            }
        }
    }
}

void mole_appear(int mole_index) {
    if (mole_index < 0 || mole_index >= MOLE_NUM) return;
    
    mole* currMole = &moles[mole_index];
    
    // clear previous state
    mole_clear_area(mole_index);
    
    currMole->visible = true;
    currMole->hammered = false;
    currMole->current_time = 0;
    currMole->animation_cycle_complete = false;
    
    // fetch difficulty time settings and randomize it
    int min_time = time_up_durations[game_config.difficulty][0];
    int max_time = time_up_durations[game_config.difficulty][1];
    currMole->time_up = rand() % (max_time - min_time + 1) + min_time;
}

void mole_hide(int mole_index) {
    if (mole_index < 0 || mole_index >= MOLE_NUM) return;
    
    // clear before hiding
    mole_clear_area(mole_index);
    
    moles[mole_index].visible = false;
    moles[mole_index].current_time = 0;
}

void mole_reset(int mole_index) {
    if (mole_index < 0 || mole_index >= MOLE_NUM) return;
    
    mole* currMole = &moles[mole_index];
    
    // clear curr state before reseting 
    mole_clear_area(mole_index);
    
    currMole->visible = false;
    currMole->hammered = false;
    currMole->current_time = 0;
    currMole->animation_cycle_complete = false;
    
    // rand next appearance time
    currMole->next_appearance_time = rand() % 240 + 120; // 2-4 second delay
}

bool mole_check_collision(int16_t cursor_x, int16_t cursor_y) {
    if (!_cursor.click) return false; // Only check collision on click
    
    for (int i = 0; i < MOLE_NUM; i++) {
        mole* currMole = &moles[i];
        
        if (currMole->visible && !currMole->hammered) {
            // check cursor collision
            if (cursor_x >= currMole->x && 
                cursor_x <= currMole->x + MOLE_WIDTH &&
                cursor_y >= currMole->y && 
                cursor_y <= currMole->y + MOLE_HEIGHT) {
                
                // Clear the up state before marking as hammered
                mole_clear_area(i);
                
                //hit mole
                currMole->hammered = true;
                currMole->visible = false;
                currMole->current_time = 0;
                return true;
            }
        }
    }
    return false;
}

void moles_draw() {
    for (int i = 0; i < MOLE_NUM; i++) {
        mole* currMole = &moles[i];
        
        if (currMole->visible && !currMole->hammered) {
            draw_xpm_transparent(moleUp_pixmap, &moleUp_img, 
                               currMole->x, currMole->y);
        } else {
            draw_xpm_transparent(moleDown_pixmap, &moleDown_img, 
                               currMole->x, currMole->y);
        }
    }
}

void moles_clear_all() {
    for (int i = 0; i < MOLE_NUM; i++) {
        mole_clear_area(i);
    }
}

bool moles_check_game_over() {
    return game_config.game_over;
}

void moles_cleanup() {
    moles_clear_all();
    
    // reset moles
    for (int i = 0; i < MOLE_NUM; i++) {
        moles[i].visible = false;
        moles[i].prev_visible = false;
        moles[i].hammered = false;
        moles[i].current_time = 0;
        moles[i].time_up = 0;
        moles[i].next_appearance_time = 0;
        moles[i].animation_cycle_complete = false;
    }

    game_config.game_over = false;
    game_config.player_won = false;
}

bool moles_is_game_over() {
    return game_config.game_over;
}

bool moles_player_won() {
    return game_config.player_won;
}

void moles_set_player_won() {
    game_config.game_over = true;
    game_config.player_won = true;
}

void moles_set_difficulty(difficulty difficulty) {
    game_config.difficulty = difficulty;

}
