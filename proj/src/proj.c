#include "proj.h"

int main(int argc, char *argv[]) {
    // general settings
    lcf_set_language("EN-US");
    lcf_trace_calls("/home/lcom/labs/proj/trace.txt");
    lcf_log_output("/home/lcom/labs/proj/output.txt");
    if (lcf_start(argc, argv))
        return 1;
    lcf_cleanup();
    return 0;
}

int (proj_main_loop)(int argc, char* argv[]) {   
    
  // init video mode
  if (vg_init(0x118) == NULL){
    printf("error initializing video mode");
    return 1;
  }

  // sub interrupts
  uint8_t mouse_bit_no, timer_bit_no, keyboard_bit_no;
    
  if (subscribe_interrupts(&mouse_bit_no, &timer_bit_no, &keyboard_bit_no) != 0) {
      printf("error subbing\n");
      vg_exit();
      return 1;
  }
  
  int mouse_rq = BIT(mouse_bit_no);
  int keyboard_rq = BIT(keyboard_bit_no);
  int timer_rq = BIT(timer_bit_no);

  // load all xpm to mem - faster rendering
  load_allxpm();

  cursor_init();

  game game_ = {MENU_STATE, true};
  
  // timer vars
  unsigned int game_timer = 0;
  const unsigned int GAME_DURATION = 3600; // 60 secs x60 ticks/second
  bool game_timer_active = false;
  
  // Add flags to prevent multiple win/lose screen draws
  bool win_screen_drawn = false;
  bool lose_screen_drawn = false;

  load(game_.currState);

  cursor_save_background();
  cursor_draw();

  // mouse packet variables 
  uint8_t packet_bytes[3];
  uint8_t byte_idx = 0;

  int ipc_status, r;
  message msg;
  bool screen_needs_redraw = false;

  while (game_.running){
    //-------------------------
    // INTERRUPT LOGIC 
    //-------------------------
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:
                // Mouse interrupt
                if (msg.m_notify.interrupts & mouse_rq) {
                 mouse_ih();
                 if (mouse_error) break;
    
                 uint8_t b = mouse_byte;
                 if (byte_idx == 0 && !(b & BIT(3))) break; 
    
                 packet_bytes[byte_idx++] = b;
                 if (byte_idx == 3) {
                  struct packet pp;
                  process_packet(&pp, packet_bytes);
                  // Handle cursor movement and clicking
                  cursor_handle_packet(&pp);
                  byte_idx = 0;
                }
               }
                // Keyboard interrupt
                if (msg.m_notify.interrupts & keyboard_rq) {
                    kbc_ih();
                    if (scancode == 0x81) { // ESC key released
                        game_.running = false;
                    }
                }

                // timer interrupt 
                if (msg.m_notify.interrupts & timer_rq) {
                    timer_int_handler(); // Your timer handler
                    
                    // Update game logic on timer interrupts
                    if (game_.currState == MAIN_GAME_STATE) {

                        if (game_timer_active && !moles_is_game_over()) {
                            game_timer++;
                            
                            // check if player survived 60 secs
                            if (game_timer >= GAME_DURATION) {
                                moles_set_player_won();
                                game_timer_active = false;
                                
                                // Clear all moles before drawing win screen
                                moles_clear_all();
                                
                                // Draw win screen only once
                                if (!win_screen_drawn) {
                                    drawImage(win_esc_screen_pixmap, &win_esc_screen_img, 0, 0);
                                    win_screen_drawn = true;
                                    screen_needs_redraw = false; 
                                }
                            }
                        }
                        
                        // Update moles only if game not over
                        if (!moles_is_game_over()) {
                            moles_update();
                            
                            // Check for immediate lose condition
                            if (moles_is_game_over() && !moles_player_won()) {
                                game_timer_active = false;
                                printf("GAME OVER! A mole escaped!\n");
                                
                                // Clear all moles before drawing lose screen
                                moles_clear_all();
                                
                                // Draw lose screen only once
                                if (!lose_screen_drawn) {
                                    drawImage(loss_esc_screen_pixmap, &loss_esc_screen_img, 0, 0);
                                    lose_screen_drawn = true;
                                    screen_needs_redraw = false; // Don't redraw game
                                }
                            } else if (!moles_is_game_over()) {
                                // Only redraw moles if game is still active
                                moles_draw();
                                cursor_draw();
                            }
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    
    //-------------------------
    // GAME LOGIC
    //-------------------------
    gameState prev_state = game_.currState;
    
    switch (game_.currState){
      case MENU_STATE:
        handle_menu_input(&game_);
        if (prev_state != game_.currState) {
            screen_needs_redraw = true;
            // reset timer and flags when entering game
            if (game_.currState == MAIN_GAME_STATE) {
                game_timer = 0;
                game_timer_active = true;
                win_screen_drawn = false;
                lose_screen_drawn = false;
            }
        }
        break;
      case MAIN_GAME_STATE:
        handle_game_input(&game_);
        if (prev_state != game_.currState) {
            screen_needs_redraw = true;
            game_timer_active = false; // stop timer if leaving game
            moles_cleanup();
            win_screen_drawn = false;
            lose_screen_drawn = false;
        }
        break;
      case SETTINGS_STATE:
        handle_settings_input(&game_);
        if (prev_state != game_.currState) {
            screen_needs_redraw = true;
        }
        break;
      case EXIT_STATE:
        game_.running = false;
        break;
    }
    
    if (screen_needs_redraw) {
        load(game_.currState);
        
        // oonly save bg if not in a win or lose state
        if (game_.currState != MAIN_GAME_STATE || 
            (!moles_is_game_over() && !win_screen_drawn && !lose_screen_drawn)) {
            cursor_save_background();
            cursor_draw();
        }
        screen_needs_redraw = false;
    }
  }

  // cleanup
  cursor_cleanup();
  free_allxpm();
  unsubscribe_interrupts();
  vg_exit();
  return 0;
}
