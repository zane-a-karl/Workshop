/// For demonstrating if let, else let usage.
pub struct BackgroundColor {}

impl BackgroundColor {
    pub fn choose_color() {
        let favorite_color: Option<&str> = None;
        let is_tuesday = false;
        let age: Result<u8, _> = "34".parse();

        if let Some(color) = favorite_color {
            println!("Using your favorite color, {}, as the background", color);
        } else if is_tuesday {
            println!("Tuesday is green day!");
        } else if let Ok(age) = age {
            if age > 30 {
                println!("Using purple as the background color");
            } else {
                println!("Using orange as the background color");
            }
        } else {
            println!("Using blue as the background color");
        }
    }
}

/// For demonstrating 'while let' usage.
pub struct VectorStack {}

impl VectorStack {
    pub fn example_stack() {
        let mut stack = Vec::new();

        stack.push(1);
        stack.push(2);
        stack.push(3);

        while let Some(top) = stack.pop() {
            println!("{}", top);
        }
    }
}

/// For demonstrating 'for loop' usage.
pub struct DestructureVector {}

impl DestructureVector {
    pub fn example_destructure() {
        let v = vec!['a', 'b', 'c'];

        for (index, value) in v.iter().enumerate() {
            println!("{} is at index {}", value, index);
        }
    }
}

/// For demonstrating multiple assignment usage.
pub struct DestructureTuple {}

impl DestructureTuple {
    /// Demonstrates function param pattern matching.
    fn print_coordinates(&(x, y): &(i32, i32)) {
        println!("Current location: ({}, {})", x, y);
    }
    pub fn example_destructure() {
        let point = (3, 5);
        DestructureTuple::print_coordinates(&point);
    }
}

/// For demonstrating the variety of uses of 'match'.
pub struct UseMatch {}

impl UseMatch {
    pub fn tricky_ex_with_same_variable_names() {
        let x = Some(5);
        let y = 10;

        match x {
            Some(50) => println!("Got 50"),
            Some(y) => println!("Matched, y = {:?}", y),
            _ => println!("Default case, x = {:?}", x),
        }

        println!("at the end: x = {:?}, y = {:?}", x, y);
    }
    pub fn or_operator_matches_multiple_inputs() {
        let x = 1;

        match x {
            1 | 2 => println!("one or two"),
            3 => println!("three"),
            _ => println!("anything"),
        }
    }
    pub fn range_operator_matches_int_range_of_inputs() {
        let x = 5;

        match x {
            1..=5 => println!("one through five"),
            _ => println!("something else"),
        }
    }
    pub fn range_operator_matches_char_range_of_inputs() {
        let x = 'c';

        match x {
            'a'..='j' => println!("early ASCII letter"),
            'k'..='z' => println!("late ASCII letter"),
            _ => println!("something else"),
        }
    }
}

/// For Demonstrating how to destructure Structs.
pub struct Point {
    x: i32,
    y: i32,
}
impl Point {
    pub fn destructure() {
        let p = Point { x: 0, y: 7 };

        let Point { x: a, y: b } = p;
        assert_eq!(0, a);
        assert_eq!(7, b);
        let Point { x, y } = Point { x: 42, y: 42 };
        println!("x={}, y={}", x, y);
    }
    pub fn where_in_space_is_the_point() {
        let p = Point { x: 0, y: 7 };

        match p {
            Point { x, y: 0 } => println!("On the x axis at {}", x),
            Point { x: 0, y } => println!("On the y axis at {}", y),
            Point { x, y } => println!("On neither axis: ({}, {})", x, y),
        }
    }
    pub fn complex_destructure() {
        let ((feet, inches), Point { x, y }) = ((3, 10), Point { x: 3, y: -10 });
        println!("feet={}, inches={}, x={}, y={}", feet, inches, x, y);
    }
}

/// For demonstrating how to destructure enums with match.
enum Color {
    Rgb(i32, i32, i32),
    Hsv(i32, i32, i32),
}
enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
    ChangeColor(Color),
}
pub struct TestMessage {}
impl TestMessage {
    pub fn destructure() {
        let _msg0 = Message::Quit;
        let _msg1 = Message::Move { x: 0, y: 1 };
        let _msg2 = Message::Write("zane".to_string());
        let _msg3 = Message::ChangeColor(Color::Rgb(0, 160, 255));
        let msg4 = Message::ChangeColor(Color::Hsv(0, 160, 255));

        match msg4 {
            Message::Quit => println!("The Quit variant has no data to destructure."),
            Message::Move { x, y } => {
                println!("Move in the x direction {} and in the y direction {}", x, y);
            }
            Message::Write(text) => println!("Text message: {}", text),
            Message::ChangeColor(Color::Rgb(r, g, b)) => {
                println!("Change the color to red {}, green {}, and blue {}", r, g, b)
            }
            Message::ChangeColor(Color::Hsv(h, s, v)) => println!(
                "Change the color to hue {}, saturation {}, and value {}",
                h, s, v
            ),
        }
    }
}

/// For the usage of different ways to ignore params
pub struct IgnoreParams {
    x: i32,
    y: i32,
    z: i32,
}
impl IgnoreParams {
    fn foo(_: i32, y: i32) {
        println!("This code only uses the y parameter: {}", y);
    }
    pub fn in_fcns() {
        IgnoreParams::foo(3, 4);
    }
    pub fn within_other_patterns() {
        let mut setting_value = Some(5);
        let new_setting_value = Some(10);
        match (setting_value, new_setting_value) {
            (Some(_), Some(_)) => {
                println!("Can't overwrite an existing customized value");
            }
            _ => {
                setting_value = new_setting_value;
            }
        }
        println!("setting is {:?}", setting_value);
    }
    pub fn in_certain_places() {
        let numbers = (2, 4, 8, 16, 32);

        match numbers {
            (first, _, third, _, fifth) => {
                println!("Some numbers: {}, {}, {}", first, third, fifth)
            }
        }
    }
    pub fn using_elipses_in_structs() {
        let origin = IgnoreParams { x: 0, y: 0, z: 0 };

        match origin {
            IgnoreParams { x, .. } => println!("x is {}", x),
        }
        match origin {
            IgnoreParams { y, .. } => println!("y is {}", y),
        }
        match origin {
            IgnoreParams { z, .. } => println!("z is {}", z),
        }
    }
    pub fn using_elipses_in_tuples() {
        let numbers = (2, 4, 8, 16, 32);

        match numbers {
            (first, .., last) => {
                println!("Some numbers: {}, {}", first, last);
            }
        }
    }
}

/// For extra match and pattern examples.
pub struct ExtraStuff {}
enum HelloMsg {
    Hello { id: i32 },
}
impl ExtraStuff {
    pub fn extra_conditionals_in_match_arms() {
        let num = Some(4);

        match num {
            Some(x) if x < 5 => println!("less than five: {}", x),
            Some(x) => println!("{}", x),
            None => (),
        }

        let x = Some(5);
        let y = 10;

        match x {
            Some(50) => println!("Got 50"),
            Some(n) if n == y => println!("Matched, n = {}", n),
            _ => println!("Default case, x = {:?}", x),
        }

        println!("at the end: x = {:?}, y = {}", x, y);

        let x = 4;
        let y = false;

        match x {
            4 | 5 | 6 if y => println!("yes"),
            _ => println!("no"),
        }
    }
    pub fn using_at_symbol_bindings() {
        let msg = HelloMsg::Hello { id: 7 };

        match msg {
            HelloMsg::Hello {
                id: id_variable @ 3..=7,
            } => println!("Found an id in range: {}", id_variable),
            HelloMsg::Hello { id: 10..=12 } => println!("Found an id in another range"),
            HelloMsg::Hello { id } => println!("Found some other id: {}", id),
        }
    }
}
