// `block_on` blocks the current thread until the provided future has run to
// completion. Other executors provide more complex behavior, like scheduling
// multiple futures onto the same thread.
use futures::executor::block_on;
use std::{thread, time};

async fn hello_world() {
    println!("Hello World!");
}

fn main() {
    let future = hello_world(); // Nothing is printed

    println!("This prints first!");
    let ten_millis = time::Duration::from_millis(10);
    thread::sleep(ten_millis);

    block_on(future); // `future` is run and "hello, world!" is printed
}
