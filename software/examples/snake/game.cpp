#include "game.h"

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

extern TFT_eSPI tft;       // Invoke custom library
extern TFT_eSprite clk;

uint8_t screen_buffer[SCREEN_HEIGHT/8][SCREEN_WIDTH];

uint8_t snake_x[SCREEN_WIDTH * SCREEN_HEIGHT];
uint8_t snake_y[SCREEN_WIDTH * SCREEN_HEIGHT];
uint8_t snake_length;
uint8_t snake_direction;

uint8_t food_x, food_y;

uint8_t score;

void draw_point(uint8_t x, uint8_t y) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
        return;
    }
    clk.drawPixel(x,y,TFT_WHITE);
}

void draw_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    clk.fillRect(x,y,width,height,TFT_BLACK);
}

//void draw_string(uint8_t x, uint8_t y, const char *str) {
//    clk.drawCentreString(str, x, y, 4);
//}

void clear_screen() {
    for (int i = 0; i < SCREEN_HEIGHT/8; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            screen_buffer[i][j] = 0;
        }
    }
    clk.fillScreen(TFT_WHITE); //填充全屏白色，TFT_BLACK为TFT库内部定义，可直接使用
}

void draw_snake_and_food() {
    // 画贪吃蛇身体
    for (int i = 0; i < snake_length; i++) {
        draw_rectangle(snake_x[i]*8, snake_y[i]*8, 8, 8);
    }
    // 画果实
    draw_rectangle(food_x*8, food_y*8, 8, 8);
}

void draw_score() {
    clk.drawCentreString("Score", 120, 0, 4); // Draw text centre at position 120, 0 using font 4
    clk.drawFloat((float)score, 150, 0, 4); 
}

void generate_food() {
    // 随机生成果实位置
    food_x = rand() % (SCREEN_WIDTH/8);
    food_y = rand() % (SCREEN_HEIGHT/8);

    // 如果果实位置和贪吃蛇身体重合，则重新生成
    for (int i = 0; i < snake_length; i++) {
        if (food_x == snake_x[i] && food_y == snake_y[i]) {
            generate_food();
            return;
        }
    }
}

int check_collision() {
    // 检查是否碰撞到墙壁
    if (snake_x[0] < 0 || snake_x[0] >= SCREEN_WIDTH/8 ||
        snake_y[0] < 0 || snake_y[0] >= SCREEN_HEIGHT/8) {
        return 1;
    }
    // 检查是否碰撞到自己
    for (int i = 1; i < snake_length; i++) {
        if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i]) {
            return 1;
        }
    }
    return 0;
}

void init_snake_and_food() {
    // 初始化贪吃蛇
    snake_x[0]= SCREEN_WIDTH/16;
    snake_y[0] = SCREEN_HEIGHT/16;
    snake_length = 1;
    snake_direction = 4; // 初始方向向右
    // 初始化果实
    generate_food();
}

void move_snake() {
  // 根据方向移动贪吃蛇
  for (int i = snake_length-1; i > 0; i--) {
    snake_x[i] = snake_x[i-1];
    snake_y[i] = snake_y[i-1];
  }
  switch(snake_direction) {
    case 1: // 向上
      snake_y[0]--;
      break;
    case 2: // 向下
      snake_y[0]++;
      break;
    case 3: // 向左
      snake_x[0]--;
      break;
    case 4: // 向右
      snake_x[0]++;
      break;
  }
  // 检查是否吃到果实
  if (snake_x[0] == food_x && snake_y[0] == food_y) {
  snake_length++;
  score++;
  generate_food();
  }
}


void delay_ms(uint16_t ms) {
  delay(ms);
}

void run_snake(){
  static uint8_t game_started = 0; // 游戏是否已开始
  static uint8_t game_over = 0; // 游戏是否已结束
  // 处理按键事件
  uint8_t key = get_key();
  if (key == 5 && !game_started) { // 按下5键开始游戏
      game_started = 1;
  } else if (key == 6 && game_started) { // 按下6键结束游戏
      game_over = 1;
  } else if (key == 1 && snake_direction != 2) { // 按下上键
      snake_direction = 1;
  } else if (key == 2 && snake_direction != 1) { // 按下下键
      snake_direction = 2;
  } else if (key == 3 && snake_direction != 4) { // 按下左键
      snake_direction = 3;
  } else if (key == 4 && snake_direction != 3) { // 按下右键
      snake_direction = 4;
  }

  if (game_started && !game_over) {
      clear_screen();
      move_snake();
      if (check_collision()) {
          game_over = 1;
      }
      draw_snake_and_food();
      draw_score();
  }
}
