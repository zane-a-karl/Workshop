use obj_ori_state_machine::oo_blog::Post;
//use obj_ori_state_machine::rusty_blog::Post;

fn main() {
    let mut post = Post::new();

    post.add_text("I ate a salad for lunch");

    // The OO way
    assert_eq!("", post.content());

    post.request_review();
    assert_eq!("", post.content());
    post.reject();
    assert_eq!("", post.content());
    post.add_text(", and eggs for breakfast");
    post.request_review();
    assert_eq!("", post.content());

    post.approve();
    post.add_text(". And don't forget I'm awesome!");
    assert_eq!("", post.content());

    post.approve();
    assert_eq!(
        "I ate a salad for lunch, and eggs for breakfast",
        post.content()
    );

    // The Rusty way
    // let post = post.request_review();

    // let mut post = post.reject();

    // post.add_text(", and eggs for bfast.");

    // let post = post.request_review();

    // let post = post.pre_approve();
    // let post = post.approve().unwrap();

    // assert_eq!(
    //     "I ate a salad for lunch, and eggs for bfast.",
    //     post.content()
    // );

    println!("{}", post.content());
}
