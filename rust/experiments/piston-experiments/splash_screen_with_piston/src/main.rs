use piston_window::*;

struct SplashScreen;

impl SplashScreen {
    fn render(&mut self, ctx: Context, gfx: &mut G2d, glyphs: &mut Glyphs) {
        clear([0.2; 4], gfx);
        let msg: String =
            "Splash Screen!".to_string();
        Text::new_color([1.0, 1.0, 1.0, 0.7], 30)
            .draw(
                &msg,
                glyphs,
                &ctx.draw_state,
                ctx.transform.trans(100., 100.),
                gfx,
            )
            .unwrap();
        let msg: String =
            "Press the Spacebar to move to the Game Screeen.".to_string();
        Text::new_color([1.0, 1.0, 1.0, 0.7], 25)
            .draw(
                &msg,
                glyphs,
                &ctx.draw_state,
                ctx.transform.trans(25., 125.),
                gfx,
            )
            .unwrap();
    }
}

struct GameScreen;

impl GameScreen {
    fn render(&mut self, ctx: Context, gfx: &mut G2d, glyphs: &mut Glyphs) {
        clear([0.0; 4], gfx);
        let msg: String = "Congratulations! You've made it to the game screen!.".to_string();
        Text::new_color([0.0, 1.0, 0.0, 1.0], 30)
            .draw(
                &msg,
                glyphs,
                &ctx.draw_state,
                ctx.transform.trans(100., 100.),
                gfx,
            )
            .unwrap();
    }
}

fn main() {
    let font = include_bytes!("/Library/Fonts/Arial Unicode.ttf");

    let opengl = OpenGL::V4_5;

    let window_settings = WindowSettings::new("move from splash to game screen", [1000, 500])
        .graphics_api(opengl)
        .vsync(true)
        .exit_on_esc(true);
    let mut window: PistonWindow = window_settings
        .build()
        .expect("Something went wrong building the window");

    let mut glyphs = Glyphs::from_bytes(
        font,
        window.create_texture_context(),
        TextureSettings::new(),
    )
    .unwrap();

    let mut ss = SplashScreen;
    let mut gs = GameScreen;

    while let Some(e) = window.next() {
        if let Some(_r) = e.render_args() {
            window.draw_2d(&e, |ctx, gfx, dev| {
                ss.render(ctx, gfx, &mut glyphs);
                glyphs.factory.encoder.flush(dev);
            });
        }
        if let Some(b) = e.button_args() {
            if b.state == ButtonState::Press && b.button == Button::Keyboard(Key::Space) {
                while let Some(e) = window.next() {
                    window.draw_2d(&e, |ctx, gfx, dev| {
                        gs.render(ctx, gfx, &mut glyphs);
                        glyphs.factory.encoder.flush(dev);
                    });
                }
            }
        }
    }
}
