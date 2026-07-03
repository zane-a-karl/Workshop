use crate::food::Food;
use crate::snake::{Direction, Snake, SnakeBodyPart};

use piston_window::*;
use rand::thread_rng;
use rand::Rng;
use std::collections::LinkedList;
use std::iter::FromIterator;

pub struct Game {
    pub snake: Snake,
    pub rows: u32,
    pub cols: u32,
    pub tile_width: u32,
    pub food: Food,
    pub snake_just_ate: bool,
    pub score: u32,
    pub gameover: bool,
}

impl Game {
    pub fn new_game(rows: u32, cols: u32, size: u32, start_x: u32, start_y: u32) -> Self {
        Game {
            rows: rows,
            cols: cols,
            tile_width: size,
            food: Food { x: 1, y: 1 },
            snake_just_ate: false,
            score: 0,
	    gameover: false,
            snake: Snake {
                body: LinkedList::from_iter(
                    (vec![
                        SnakeBodyPart {
                            x: start_x,
                            y: start_y,
                        },
                        SnakeBodyPart {
                            x: start_x,
                            y: start_y + 1,
                        },
                    ])
                    .into_iter(),
                ),
                dir: Direction::Right,
            },
        }
    }
    pub fn render(&mut self, ctx: Context, gfx: &mut G2d) {
        let black: [f32; 4] = [0.0, 0.0, 0.0, 1.0];

        clear(black, gfx);

        self.snake.render(ctx, gfx, self.tile_width);

        self.food.render(ctx, gfx, self.tile_width);
    }

    pub fn update(&mut self) -> bool {
        if !self.snake.update(self.snake_just_ate, self.rows, self.cols) {
            return false;
        }

        // The timing of this is a bit odd, b/c technically the snake ate
        // during the previous update but b/c we check for the collision
        // after updating the snake we need to do this check b/4 and
        // after the food update...
        if self.snake_just_ate {
            self.score += 1;
            self.snake_just_ate = false;
        }

        self.snake_just_ate = self.food.update(&self.snake);
        if self.snake_just_ate {
            self.generate_new_food_location();
        }
        return true;
    }

    pub fn pressed(&mut self, btn: &Button) {
        let last_direction = self.snake.dir.clone();

        self.snake.dir = match btn {
            &Button::Keyboard(Key::Up) if last_direction != Direction::Down => Direction::Up,
            &Button::Keyboard(Key::Down) if last_direction != Direction::Up => Direction::Down,
            &Button::Keyboard(Key::Right) if last_direction != Direction::Left => Direction::Right,
            &Button::Keyboard(Key::Left) if last_direction != Direction::Right => Direction::Left,
            _ => last_direction,
        }
    }

    pub fn generate_new_food_location(&mut self) {
        let mut r = thread_rng();
        loop {
            let rnd_x = r.gen_range(0..self.cols);
            let rnd_y = r.gen_range(0..self.rows);
            if !self
                .snake
                .body
                .iter()
                .any(|body_part| body_part.x == rnd_x && body_part.y == rnd_y)
            {
                self.food = Food { x: rnd_x, y: rnd_y };
                break;
            }
        }
    }
}
