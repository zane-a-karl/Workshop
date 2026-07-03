/// For demonstrating how to overload operators using
/// associated types in Rust.
#[derive(Debug, PartialEq)]
struct Point {
    x: i32,
    y: i32,
}

impl std::ops::Add for Point {
    type Output = Point;

    fn add(self, other: Point) -> Point {
        Point {
            x: self.x + other.x,
            y: self.y + other.y,
        }
    }
}
impl std::fmt::Display for Point {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "({}, {})", self.x, self.y)
    }
}
struct Millimeters(u32);
struct Meters(u32);

impl std::ops::Add<Meters> for Millimeters {
    type Output = Millimeters;

    fn add(self, other: Meters) -> Millimeters {
        Millimeters(self.0 + (other.0 * 1000))
    }
}

pub struct OperatorOverload {}
impl OperatorOverload {
    pub fn point_overdrive() {
        assert_eq!(
            Point { x: 1, y: 0 } + Point { x: 2, y: 3 },
            Point { x: 3, y: 3 }
        );
        println!(
            "{:?} + {:?} = {:?}",
            Point { x: 1, y: 0 },
            Point { x: 2, y: 3 },
            Point { x: 3, y: 3 }
        );
    }
    pub fn meters_overdrive() {
        let mm = Millimeters(1000);
        let m = Meters(41);
        let sum = mm + m;
        println!("1000mm + 41m = {}", sum.0);
    }
}

/// Fully Qualified Syntax for Disambiguation: Calling Methods with the Same Name.
trait Pilot {
    fn fly(&self);
}

trait Wizard {
    fn fly(&self);
}

struct Human;

impl Pilot for Human {
    fn fly(&self) {
        println!("This is your captain speaking.");
    }
}

impl Wizard for Human {
    fn fly(&self) {
        println!("Up!");
    }
}

impl Human {
    fn fly(&self) {
        println!("*waving arms furiously*");
    }
}
trait Animal {
    fn baby_name() -> String;
}

struct Dog;

impl Dog {
    fn baby_name() -> String {
        String::from("Spot")
    }
}

impl Animal for Dog {
    fn baby_name() -> String {
        String::from("puppy")
    }
}

pub struct Disambiguation {}
impl Disambiguation {
    pub fn test_fly() {
        let person = Human;
        Pilot::fly(&person);
        Wizard::fly(&person);
        person.fly();
    }
    pub fn test_baby_name() {
        println!("A baby dog is called a {}", Dog::baby_name());
        println!("A baby dog is called a {}", <Dog as Animal>::baby_name());
    }
}

trait OutlinePrint: std::fmt::Display {
    fn outline_print(&self) {
        let output = self.to_string();
        let len = output.len();
        println!("{}", "*".repeat(len + 4));
        println!("*{}*", " ".repeat(len + 2));
        println!("* {} *", output);
        println!("*{}*", " ".repeat(len + 2));
        println!("{}", "*".repeat(len + 4));
    }
}
impl OutlinePrint for Point {}

pub struct SuperTraits {}
impl SuperTraits {
    pub fn test_print() {
        let p = Point { x: 42, y: 43 };
        println!("{}", p);
        p.outline_print()
    }
}

/// Using the Newtype Pattern to Implement External Traits on External Types.
struct Wrapper(Vec<String>);

impl std::fmt::Display for Wrapper {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "[{}]", self.0.join(", "))
    }
}

pub struct NewtypePattern {}
impl NewtypePattern {
    pub fn test() {
        let w = Wrapper(vec![String::from("hello"), String::from("world")]);
        println!("w = {}", w);
    }
}
