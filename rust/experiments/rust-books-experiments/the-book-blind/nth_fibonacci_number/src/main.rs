use std::io::{stdin, stdout, Write};

fn main() {
    println!("Welcome to the nth Fibonacci Calculator!");

    'main_event_loop: loop {
        let mut input = "".to_string();
        let mut again = "".to_string();
        print!("Please enter the positive integer representing the Fibonacci number you'd like to receive: ");
        stdout().flush().unwrap();
        if let Ok(_n_read_bytes) = stdin().read_line(&mut input) {
            match input.trim().parse::<i32>() {
                Ok(n) => {
                    if n < 0 {
                        println!("Integers must be positive.");
                        continue 'main_event_loop;
                    }
                    println!("The {}-th Fibonacci number is {}", n, nth_fibonacci(n));
                }
                Err(_) => {
                    println!("Only positive integers are allowed");
                    continue 'main_event_loop;
                }
            }
        }
        print!("Would you like to play again?(Y/N)");
        stdout().flush().unwrap();
        if let Ok(_nbr) = stdin().read_line(&mut again) {
            if again.trim() == "Y" || again.trim() == "y" {
                continue 'main_event_loop;
            } else {
                println!("Goodbye.");
                break 'main_event_loop;
            }
        }
    }
}

fn nth_fibonacci(input: i32) -> i32 {
    if input == 0 {
        return 0;
    } else if input == 1 {
        return 1;
    } else {
        return nth_fibonacci(input - 1) + nth_fibonacci(input - 2);
    }
}
