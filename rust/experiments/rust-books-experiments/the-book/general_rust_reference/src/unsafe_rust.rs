static mut COUNTER: u32 = 0;

extern "C" {
    fn abs(input: i32) -> i32;
}

/// For demonstrating how to use unsafe rust safely.
/// Dereference a raw pointer
/// Call an unsafe function or method
/// Access or modify a mutable static variable
/// Implement an unsafe trait
/// Access fields of unions
pub struct UnsafeRust {}

impl UnsafeRust {
    pub unsafe fn deref_raw_ptrs() {
        let mut num = 5;
        let ptr0 = &num as *const i32;
        let ptr1 = &mut num as *mut i32;
        println!("created raw ptr0={:?}, ptr1={:?}", ptr0, ptr1);
        *ptr1 = 42; // changes *ptr0 and *ptr1.
        println!("created raw *ptr0={:?}, *ptr1={:?}", *ptr0, *ptr1);
        let address = 0x012345usize;
        let r = address as *const i32;
        println!("uncertain memory at {:?}", r);
    }
    fn split_at_mut<T>(slice: &mut [T], idx: usize) -> (&mut [T], &mut [T])
    where
        T: std::cmp::PartialEq,
    {
        let len = slice.len();
        let ptr = slice.as_mut_ptr();
        assert!(idx <= len);
        unsafe {
            (
                std::slice::from_raw_parts_mut(ptr, idx),
                std::slice::from_raw_parts_mut(ptr.add(idx), len - idx),
            )
        }
    }
    pub fn use_safe_abstraction_for_split_at_mut() {
        let mut v = vec![1, 2, 3, 4, 5, 6];

        let r = &mut v[..];

        let (a, b) = UnsafeRust::split_at_mut(r, 3);

        assert_eq!(a, &mut [1, 2, 3]);
        assert_eq!(b, &mut [4, 5, 6]);
        println!("a={:?}, b={:?}", a, b);

        let mut vv = vec!["a", "b", "c", "d", "e", "f"];

        let rr = &mut vv[..];

        let (aa, bb) = UnsafeRust::split_at_mut(rr, 3);

        assert_eq!(aa, &mut ["a", "b", "c"]);
        assert_eq!(bb, &mut ["d", "e", "f"]);
        println!("aa={:?}, bb={:?}", aa, bb);
    }
    pub fn call_external_c_function() {
        unsafe {
            println!("Absolute value of -3 according to C: {}", abs(-3));
        }
    }
    fn add_to_count(inc: u32) {
        unsafe {
            COUNTER += inc;
        }
    }
    pub fn access_and_modify_mutable_static_vars() {
        UnsafeRust::add_to_count(5);
        unsafe {
            println!("counter = {}", COUNTER);
        }
    }
}
