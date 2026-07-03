use std::io::{stdout, Write};

fn main() {
    println!("And now... The 12 Days of Christmas");
    'main_event_loop: for day in 1..=12 {
        print!("On the ");
        match day {
            1 => print!("first"),
            2 => print!("second"),
            3 => print!("third"),
            4 => print!("fourth"),
            5 => print!("fifth"),
            6 => print!("sixth"),
            7 => print!("seventh"),
            8 => print!("eighth"),
            9 => print!("ninth"),
            10 => print!("tenth"),
            11 => print!("eleventh"),
            12 => print!("twelfth"),
            _ => {
                println!("Invalid day of Christmas");
                break 'main_event_loop;
            }
        }
        println!(" day of Christmas may true love gave to me");
        stdout().flush().unwrap();
        let mut and_flag = false;
        let mut ctr = day;
        if day > 1 {
            and_flag = true;
        }
        'repetition: loop {
            match ctr {
                0 => break 'repetition,
                1 => {
                    if and_flag {
                        println!("And a partridge in a pear tree");
                    } else {
                        println!("A partridge in a pear tree");
                    }
                }
                2 => println!("Two turtledoves"),
                3 => println!("Three French hens"),
                4 => println!("Four calling birds"),
                5 => println!("FIVE GOLDEN RINGS!!!"),
                6 => println!("Six geese a-laying"),
                7 => println!("Seven swans a-swimming"),
                8 => println!("Eight maids a-milking"),
                9 => println!("Nine ladies dancing"),
                10 => println!("Ten lords a-leaping"),
                11 => println!("Eleven pipers piping"),
                12 => println!("Twelve drummers drumming"),
                _ => println!("Invalid day of Christmas"),
            }
            ctr -= 1;
        }
        println!("");
        println!("");
    }
}
