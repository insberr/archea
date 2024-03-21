use std::collections::BTreeMap;

use crate::Pixel;
use crate::engine::stuff::vect3::Vect3;

// Return true if the position is free to use.
pub fn check_pos(pixels: &BTreeMap<Vect3, Pixel>, current_position: &Vect3, new_position: &Vect3) -> bool {
    if new_position == current_position {
        panic!("WTF ARE YOU DOING!!");
    }

    if *new_position.y < 0.0 || *new_position.y > 50.0 {
        return false;
    }

    if *new_position.x > 50.0 || *new_position.x < -50.0 {
        return false;
    }
    if *new_position.z > 50.0 || *new_position.z < -50.0 {
        return false;
    }


    if pixels.contains_key(&new_position) {
        return false;
    }

    true
}