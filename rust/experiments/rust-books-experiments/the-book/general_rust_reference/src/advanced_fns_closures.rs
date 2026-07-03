enum Status {
    Value(u32),
    Stop,
}

pub struct FnPtrs {}
impl FnPtrs {
    fn add_one(x: i32) -> i32 {
        x + 1
    }
    fn do_twice(f: fn(i32) -> i32, arg: i32) -> i32 {
        f(arg) + f(arg)
    }
    pub fn use_fn_ptr() {
        let answer = FnPtrs::do_twice(FnPtrs::add_one, 5);
        println!("The answer is: {}", answer);
    }
    pub fn closure_or_fn_ptr() {
        let v0 = vec![1, 2, 3];
        let s0: Vec<String> = v0.iter().map(|i| i.to_string()).collect();
        println!("{:?}", s0);

        let v1 = vec![4, 5, 6];
        let s1: Vec<String> = v1.iter().map(ToString::to_string).collect();
        println!("{:?}", s1);
    }
    pub fn tuple_structs_and_enum_variants_as_fn_ptrs() {
        let list_of_statuses: Vec<Status> = (0u32..20).map(Status::Value).collect();
        let mut v = vec![];
        for status in list_of_statuses.iter() {
            match status {
                Status::Value(val) => v.push(val),
                Status::Stop => println!("Stopped"),
            }
        }
        println!("{:?}", v);
    }
    fn ret_closure() -> Box<dyn Fn(i32) -> i32> {
        Box::new(|x| x + 1)
    }
}
