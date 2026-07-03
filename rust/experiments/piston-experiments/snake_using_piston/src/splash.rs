use piston_window::*;

pub enum ScreenType {
    GameStart,
    GameOver,
}

pub struct Splash {
    pub high_score: u32,
    pub screen: ScreenType,
}

impl Splash {
    pub fn render(&mut self, ctx: Context, gfx: &mut G2d, glyphs: &mut Glyphs) {
        clear([0.2; 4], gfx);
        match self.screen {
            ScreenType::GameStart => {
                let msg = format!("Welcome To Snake!");
                Text::new_color([1.0, 1.0, 1.0, 0.7], 30)
                    .draw(
                        &msg,
                        glyphs,
                        &ctx.draw_state,
                        ctx.transform.trans(100., 100.),
                        gfx,
                    )
                    .unwrap();
                let msg = format!("Press the Spacebar to play.");
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
            ScreenType::GameOver => {
                let msg = format!("Gameover :( your high score was {}", self.high_score);
                Text::new_color([1.0, 1.0, 1.0, 0.7], 30)
                    .draw(
                        &msg,
                        glyphs,
                        &ctx.draw_state,
                        ctx.transform.trans(100., 100.),
                        gfx,
                    )
                    .unwrap();
                let msg = format!("Press the Spacebar to play again.");
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
    }
}
