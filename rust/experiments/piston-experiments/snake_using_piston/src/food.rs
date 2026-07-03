use crate::snake::Snake;

use piston_window::*;

pub struct Food {
    pub x: u32,
    pub y: u32,
}

impl Food {
    pub fn render(&self, ctx: Context, gfx: &mut G2d, tile_width: u32) {
        let red: [f32; 4] = [1.0, 0.0, 0.0, 1.0];

        let x: u32 = self.x * tile_width;
        let y: u32 = self.y * tile_width;

        let piece_of_food: graphics::types::Rectangle =
            graphics::rectangle::square(x as f64, y as f64, tile_width as f64);

        graphics::rectangle(red, piece_of_food, ctx.transform, gfx);
    }

    pub fn update(&mut self, snake: &Snake) -> bool {
        let head = snake.body.front().expect("Snake has no body");
        if head.x == self.x && head.y == self.y {
            return true;
        }
        return false;
    }
}
