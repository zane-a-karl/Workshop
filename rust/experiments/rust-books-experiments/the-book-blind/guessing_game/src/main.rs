///  Here’s how it works: the program will generate a random
/// integer between 1 and 100. It will then prompt the player
/// to enter a guess. After a guess is entered, the program
/// will indicate whether the guess is too low or too high.
/// If the guess is correct, the game will print a
/// congratulatory message and exit.
use rand::{rngs::ThreadRng, Rng};
use std::cmp::Ordering;
use std::io::{stdin, stdout, Write};

fn main() {
    let mut thread_rng = ThreadRng::default();
    println!("Welcome to the guessing game!");
    println!("Guess the random number between 1 and 100 in as few guesses as possible!");
    let rn: u32 = ((thread_rng.gen::<u32>()) % 99) + 1;
    //    println!("The random number is = {rn}");
		// Note: this is how you label a loop in Rust
		// you can also label your `break` and `continue` stmts
		// This lets you break out of an outer loop from within
		// an inner loop! If you don't specify loop labels then
		// breaking and continuing only applies to the loop you
		// are breaking or continuing from!
    'main_event_loop: loop {
        print!("Enter a guess: ");
        stdout().flush().unwrap(); // Needed to get the print order correct
        let mut guess_buf = String::new();
        if let Ok(_n_bytes_read) = stdin().read_line(&mut guess_buf) {
            let guess_as_u32 = match guess_buf.trim().parse::<u32>() {
                Ok(n) => {
                    if n < 1 || n > 100 {
                        println!("Integers must be between 1 and 100.");
                        continue 'main_event_loop;
                    }
                    n
                }
                Err(_) => {
                    println!("Only integers are allowed.");
                    continue 'main_event_loop;
                }
            };
            match guess_as_u32.cmp(&rn) {
                Ordering::Greater => println!("Too High!"),
                Ordering::Less => println!("Too Low!"),
                Ordering::Equal => {
                    println!("Spot on!");
                    break 'main_event_loop;
                }
            };
        }
    }
}
