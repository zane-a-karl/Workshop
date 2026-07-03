use piston_window::*;

fn main() {
    //    let font = include_bytes!("/Library/Fonts/Arial Unicode.ttf");
    let font = include_bytes!("./amatic/Amatic-Bold.ttf");
    let opengl = OpenGL::V3_2;

    let settings = WindowSettings::new("test", [500, 500])
        .graphics_api(opengl)
        .fullscreen(false)
        .vsync(true)
        .exit_on_esc(true);

    let mut window: PistonWindow = settings.build().unwrap();

    let mut glyphs = Glyphs::from_bytes(
        font,
        window.create_texture_context(),
        TextureSettings::new(),
    )
    .unwrap();

    while let Some(e) = window.next() {
        window.draw_2d(&e, |c, gfx, device| {
            clear([0.2; 4], gfx);

            Text::new_color([1.0, 1.0, 1.0, 0.7], 30)
                .draw(
                    "Hi!",
                    &mut glyphs,
                    &c.draw_state,
                    c.transform.trans(100., 100.),
                    gfx,
                )
                .unwrap();
            glyphs.factory.encoder.flush(device);
        });
    }
}
