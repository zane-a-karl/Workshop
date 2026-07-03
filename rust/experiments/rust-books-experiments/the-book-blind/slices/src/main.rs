use std::io::{stdin, stdout, Write};

fn main() {
    print!("Please enter a string: ");
    stdout().flush().unwrap();
    let mut input = String::new();
    if let Ok(_n_bytes_read) = stdin().read_line(&mut input) {
        let input = input.trim();
        for (i, c) in input.chars().enumerate() {
            if c == ' ' {
                let slice = &input[0..i];
                println!("{slice}");
                break;
            } else if i == input.len() - 1 {
                println!("{input}");
                break;
            }
        }
    }
}
