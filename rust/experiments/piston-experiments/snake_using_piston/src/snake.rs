use piston_window::*;

use std::collections::LinkedList;

#[derive(Clone, PartialEq)]
pub enum Direction {
    Right,
    Left,
    Up,
    Down,
}

pub struct Snake {
    pub body: LinkedList<SnakeBodyPart>,
    pub dir: Direction,
}

#[derive(Clone)]
pub struct SnakeBodyPart {
    pub x: u32,
    pub y: u32,
}

impl Snake {
    pub fn render(&self, ctx: Context, gl: &mut G2d, tile_width: u32) {
        let white: [f32; 4] = [1.0, 1.0, 1.0, 1.0];

        let snake_body: Vec<graphics::types::Rectangle> = self
            .body
            .iter()
            .map(|body_part| {
                graphics::rectangle::square(
                    (body_part.x * tile_width) as f64,
                    (body_part.y * tile_width) as f64,
                    tile_width as f64,
                )
            })
            .collect();

        snake_body
            .into_iter()
            .for_each(|square| graphics::rectangle(white, square, ctx.transform, gl));
    }

    pub fn update(&mut self, just_eaten: bool, rows: u32, cols: u32) -> bool {
        let mut new_head: SnakeBodyPart = (*self.body.front().expect("Snake has no body")).clone();

        if (self.dir == Direction::Up && new_head.y == 0)
            || (self.dir == Direction::Down && new_head.y == rows - 1)
            || (self.dir == Direction::Left && new_head.x == 0)
            || (self.dir == Direction::Right && new_head.x == cols - 1)
        {
            return false;
        }

        match &self.dir {
            Direction::Right => new_head.x += 1,
            Direction::Left => new_head.x -= 1,
            Direction::Up => new_head.y -= 1,
            Direction::Down => new_head.y += 1,
        }

        if !just_eaten {
            self.body.pop_back().expect("Failed to pop_back snake tail");
        }

        if self.collided_with_snake(&new_head) {
            return false;
        }

        self.body.push_front(new_head);

        true
    }

    fn collided_with_snake(&self, head: &SnakeBodyPart) -> bool {
        self.body
            .iter()
            .any(|body_part| body_part.x == head.x && body_part.y == head.y)
    }
}
