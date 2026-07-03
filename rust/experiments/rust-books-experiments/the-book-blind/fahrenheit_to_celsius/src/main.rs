use std::io::{stdin, stdout, Write};

fn main() {
    println!("Welcome to the Temperature Converter!");
    println!("Would you like to convert to Fahrenheit(F) or Celsius(C)?");
    'main_event_loop: loop {
        let mut choice = String::with_capacity(1); //"".to_string();
        let mut input = String::new(); //"".to_string();

        print!("Please input F/C: ");
        stdout().flush().unwrap();
        if let Ok(_n_bytes_read) = stdin().read_line(&mut choice) {
            if choice.trim() == "F" || choice.trim() == "f" {
                println!("Great! Now what number would you like to convert?");
            } else if choice.trim() == "C" || choice.trim() == "c" {
                println!("Great! Now what number would you like to convert?");
            } else {
                println!("Only `F` or `C` are allowed.");
                continue 'main_event_loop;
            }
        } else {
            continue 'main_event_loop;
        }

        print!("Please input a real number: ");
        stdout().flush().unwrap();
        if let Ok(_nbr) = stdin().read_line(&mut input) {
            let input = match input.trim().parse::<f64>() {
                Ok(n) => n,
                Err(_) => {
                    println!("You must only enter real numbers.");
                    continue 'main_event_loop;
                }
            };
            if choice.trim() == "F" || choice.trim() == "f" {
                println!(
                    "{} in Fahrenheit is {}",
                    input,
                    (9.0 / 5.0) * (input) + 32.0
                );
            } else {
                println!("{} in Celsius is {}", input, (5.0 / 9.0) * (input - 32.0));
            }
        }

        choice = "".to_string();
        print!("Would you like to convert another number?(Y/N) ");
        stdout().flush().unwrap();
        if let Ok(_n_bytes_read) = stdin().read_line(&mut choice) {
            if choice.trim() == "Y" || choice.trim() == "y" {
                continue 'main_event_loop;
            } else {
                println!("Goodbye.");
                break 'main_event_loop;
            }
        } else {
            break 'main_event_loop;
        }
    }
}
