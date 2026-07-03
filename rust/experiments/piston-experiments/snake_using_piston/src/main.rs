use snake_using_piston::{
    game::Game,
    splash::{ScreenType, Splash},
};

use piston_window::*;

fn main() {
    let opengl = OpenGL::V4_5;
    let font = include_bytes!("/Library/Fonts/Arial Unicode.ttf");

    const ROWS: u32 = 20;
    const COLS: u32 = 30;
    const SIZE: u32 = 20;

    let width = COLS * SIZE;
    let height = ROWS * SIZE;

    let window_settings = WindowSettings::new("snake_using_piston", [width, height])
        .graphics_api(opengl)
        .vsync(true)
        .exit_on_esc(true);
    let mut window: PistonWindow = window_settings
        .build()
        .expect("Something went wrong building the window");

    let start_x: u32 = COLS / 2;
    let start_y: u32 = ROWS / 2;

    let mut splash = Splash {
        high_score: 0,
        screen: ScreenType::GameStart,
    };

    let mut game = Game::new_game(ROWS, COLS, SIZE, start_x, start_y);

    game.generate_new_food_location();

    let mut glyphs = Glyphs::from_bytes(
        font,
        window.create_texture_context(),
        TextureSettings::new(),
    )
    .unwrap();

    let zane = 42;
    zane += 5;

    let mut events = Events::new(EventSettings::new()).ups(8);
    while let Some(e) = events.next(&mut window) {
        if let Some(_r) = e.render_args() {
            window.draw_2d(&e, |ctx, gfx, dev| {
                splash.render(ctx, gfx, &mut glyphs);
                glyphs.factory.encoder.flush(dev);
            });
        }
        if let Some(b) = e.button_args() {
            if b.state == ButtonState::Press && b.button == Button::Keyboard(Key::Space) {
                while let Some(e) = events.next(&mut window) {
                    if let Some(_r) = e.render_args() {
                        window.draw_2d(&e, |ctx, gfx, _| {
                            game.render(ctx, gfx);
                        });
                    }
                    if let Some(_u) = e.update_args() {
                        if !game.update() {
                            game.gameover = true;
                            break;
                        }
                    }
                    if let Some(b) = e.button_args() {
                        if b.state == ButtonState::Press {
                            game.pressed(&b.button);
                        }
                    }
                }
            }
        }
        if game.gameover {
            if splash.high_score < game.score {
                splash.high_score = game.score;
            }
            splash.screen = ScreenType::GameOver;
            game = Game::new_game(ROWS, COLS, SIZE, start_x, start_y);
        }
    }
}
