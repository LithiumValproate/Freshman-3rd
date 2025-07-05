use std::sync::{Arc, Mutex};
use std::collections::HashMap;
use crate::basic::{Participant, Message};

pub struct Room {
    name: String,
    participants: Mutex<Vec<Arc<dyn Participant + Send + Sync>>>,
}

impl Room {
    pub fn new(name: String) -> Self {
        Room {
            name,
            participants: Mutex::new(Vec::new()),
        }
    }

    pub fn join(&self, p: Arc<dyn Participant + Send + Sync>) {
        let mut participants = self.participants.lock().unwrap();
        participants.push(p);
    }

    pub fn leave(&self, p: Arc<dyn Participant + Send + Sync>) {
        let mut participants = self.participants.lock().unwrap();
        if let Some(pos) = participants.iter().position(|x| Arc::ptr_eq(x, &p)) {
            participants.remove(pos);
        }
    }

    pub fn broadcast(&self, msg: &Message) {
        let participants = self.participants.lock().unwrap();
        for p in participants.iter() {
            p.receive(msg);
        }
    }

    pub fn send(&self, msg: &Message, to: Arc<dyn Participant + Send + Sync>) {
        let participants = self.participants.lock().unwrap();
        for p in participants.iter() {
            if Arc::ptr_eq(p, &to) {
                p.receive(msg);
                break;
            }
        }
    }

    pub fn get_name(&self) -> &str {
        &self.name
    }

    pub fn all_student_ids(&self) -> Vec<u64> {
        let participants = self.participants.lock().unwrap();
        participants.iter().map(|p| p.student_id()).collect()
    }
}

#[cxx::bridge]
mod ffi {
    extern "C++" {
        include!("message.h");
        include!("../struct/student.h");

        type Message;
        fn get_type(self: &Message) -> Message_Type;

        type Participant;
        fn get_user(self: &Participant) -> &Student;
        fn get_nickname(self: &Participant) -> &CxxString;

        type Student;
        fn get_id(self: &Student) -> i64;
    }

    #[namespace = "Message"]
    enum Message_Type {
        Text,
        Image,
        Gif,
        Video,
        Emoji,
    }
}

// 你需要根据实际情况实现 Participant trait 和 Message 结构体及其 receive_message 方法。
