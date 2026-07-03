fn main() {
    let string_literal = "Hello, world!";
    let t = string_literal;
    println!("{string_literal}");

    // This will throw a compile error because String isn't copy and `u` is moved!
    // let u = String::from("Hola, mundo!");
    // let v = u;
    // println!("{u}");

    let s = String::from("hello"); // s comes into scope

    takes_ownership(s); // s's value moves into the function...
    // ... and so is no longer valid here


    let x = 5; // x comes into scope

    makes_copy(x); // x would move into the function,
                   // but i32 is Copy, so it's okay to still
    println!("{x}");// use x afterward

} // Here, x goes out of scope, then s. But because s's value was moved, nothing
  // special happens.

fn takes_ownership(some_string: String) {
    // some_string comes into scope
    println!("{}", some_string);
} // Here, some_string goes out of scope and `drop` is called. The backing
  // memory is freed.

fn makes_copy(some_integer: i32) {
    // some_integer comes into scope
    println!("{}", some_integer);
}
