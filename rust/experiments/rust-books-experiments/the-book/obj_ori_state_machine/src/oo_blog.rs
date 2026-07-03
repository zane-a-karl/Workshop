/**
 * This is pretty cool, because you did this entire
 * project sequence and even added the posited option
 * of a delcarative macro that reduced the amount of code
 * duplication. It was even an interesting macro b/c
 * you had to use 'self' within the body and to do that
 * you needed to pass self as a argument to the macro.
 */

#[macro_use]
mod macros {
    macro_rules! apply {
        ($self:ident, $func:ident) => {
            if let Some(s) = $self.state.take() {
                $self.state = Some(s.$func());
            }
        };
	($self:ident, $func:ident($input:expr)) => {
            if let Some(s) = $self.state.take() {
                $self.state = Some(s.$func($input));
            }
        };
    }
}

pub struct Post {
    // Need to wrap in Option<T> b/c w/o it any call to
    // state.fcn() will try and 'move' the boxed::Box<>
    // heap memory which can't happen without the Copy
    // trait implemented which it won't be. So we wrap it
    // in an Option<T> and then we can fill the
    // vacated/'move'd heap memory with a None so the
    // compiler won't complain.

    // My inution for the need to wrap the trait obj State
    // in a Box<T> pointer is the same reason you can't
    // make a stack reference to an interface (pure virtual
    // class) in c++. That's why even the methods in the
    // trait obj definition return Box<dyn State>'s.
    state: Option<Box<dyn State>>,
    content: String,
    approval_stamps: i32,
}

impl Post {
    pub fn new() -> Post {
        Post {
            state: Some(Box::new(Draft {})),
            content: String::new(),
            approval_stamps: 0,
        }
    }
    pub fn add_text(&mut self, text: &str) {
        self.content
            .push_str(self.state.as_ref().unwrap().add_text(text));
    }
    pub fn content(&self) -> &str {
        self.state.as_ref().unwrap().content(self)
    }
    pub fn request_review(&mut self) {
	apply!(self, request_review);
    }
    pub fn approve(&mut self) {
        self.approval_stamps += 1;
	apply!(self, approve(&self));
    }
    pub fn reject(&mut self) {
        self.approval_stamps = 0;
        apply!(self, reject);
    }
}

trait State {
    fn request_review(self: Box<Self>) -> Box<dyn State>;
    fn approve(self: Box<Self>, post: &Post) -> Box<dyn State>;
    fn content<'a>(&self, _post: &'a Post) -> &'a str {
        ""
    }
    fn reject(self: Box<Self>) -> Box<dyn State>;
    fn add_text<'a>(&self, _text: &'a str) -> &'a str {
        ""
    }
}

struct Draft {}

impl State for Draft {
    fn request_review(self: Box<Self>) -> Box<dyn State> {
        Box::new(PendingReview {})
    }
    fn approve(self: Box<Self>, _post: &Post) -> Box<dyn State> {
        Box::new(PendingReview {})
    }
    fn reject(self: Box<Self>) -> Box<dyn State> {
        self
    }
    fn add_text<'a>(&self, text: &'a str) -> &'a str {
        text
    }
}

struct PendingReview {}

impl State for PendingReview {
    fn request_review(self: Box<Self>) -> Box<dyn State> {
        self
    }
    fn approve(self: Box<Self>, post: &Post) -> Box<dyn State> {
        if post.approval_stamps >= 2 {
            return Box::new(Published {});
        }
        Box::new(PendingReview {})
    }
    fn reject(self: Box<Self>) -> Box<dyn State> {
        Box::new(Draft {})
    }
}

struct Published {}

impl State for Published {
    fn request_review(self: Box<Self>) -> Box<dyn State> {
        self
    }
    fn approve(self: Box<Self>, _post: &Post) -> Box<dyn State> {
        self
    }
    fn content<'a>(&self, post: &'a Post) -> &'a str {
        &post.content
    }
    fn reject(self: Box<Self>) -> Box<dyn State> {
        self
    }
}

#[cfg(test)]
mod test {
    use crate::oo_blog::Post;

    #[test]
    fn returns_empty_content() {
        let post = Post::new();
        assert_eq!("", post.content());
    }
}
