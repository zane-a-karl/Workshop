fn main() {
    let num: u8 = 6;

    println!("num = {num:}");
    println!("num shifted 0 right and AND'd with 1 is {}", (num) & 1);
    println!("num shifted 1 right and AND'd with 1 is {}", (num >> 1) & 1);
    println!("num shifted 2 right and AND'd with 1 is {}", (num >> 2) & 1);
    println!("Clearly Rust u8's are in big endian!");

    let x = 0b11_00_11_00;
    println!("{x:}");
    let y = 0xc0_c0;
    println!("{y:}");
    let z = 0o1;
    println!("{z:}");
}
