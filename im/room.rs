use std::sync::{Arc, Mutex};
use std::collections::HashMap;

// 假设 Participant 和 Message 已在其他地方定义
// use crate::basic::{Participant, Message};

pub struct Room {
    name: String,
    participants: Mutex<Vec<Arc<Participant>>>,
}

impl Room {
    pub fn new(name: String) -> Self {
        Room {
            name,
            participants: Mutex::new(Vec::new()),
        }
    }

    pub fn join(&self, p: Arc<Participant>) {
        let mut participants = self.participants.lock().unwrap();
        participants.push(p);
    }

    pub fn leave(&self, p: &Arc<Participant>) {
        let mut participants = self.participants.lock().unwrap();
        participants.retain(|x| !Arc::ptr_eq(x, p));
    }

    pub fn broadcast(&self, msg: &Message) {
        let participants = self.participants.lock().unwrap();
        for p in participants.iter() {
            p.receive(msg);
        }
    }

    pub fn send(&self, msg: &Message, to: &Arc<Participant>) {
        to.receive(msg);
    }

    pub fn get_name(&self) -> &str {
        &self.name
    }
}

// 你需要根据实际情况实现 Participant 和 Message 结构体及其 receive 方法。
