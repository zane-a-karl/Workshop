use std::result::Result;

pub struct Post {
    content: String,
}

pub struct DraftPost {
    content: String,
}

impl Post {
    pub fn new() -> DraftPost {
        DraftPost {
            content: String::new(),
        }
    }

    pub fn content(&self) -> &str {
        &self.content
    }
}

impl DraftPost {
    pub fn add_text(&mut self, text: &str) {
        self.content.push_str(text);
    }
    pub fn request_review(self) -> PendingReviewPost {
        PendingReviewPost {
            content: self.content,
            pre_approved: false,
        }
    }
}

pub struct PendingReviewPost {
    content: String,
    pre_approved: bool,
}

impl PendingReviewPost {
    pub fn pre_approve(self) -> Self {
        PendingReviewPost {
            content: self.content,
            pre_approved: true,
        }
    }
    pub fn approve(self) -> Result<Post, ()> {
        if self.pre_approved {
            Ok(Post {
                content: self.content,
            })
        } else {
            println!("Seek pre approval first");
            Err(())
        }
    }
    pub fn reject(self) -> DraftPost {
        DraftPost {
            content: self.content,
        }
    }
}
