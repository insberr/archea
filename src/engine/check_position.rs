use std::collections::BTreeMap;
use bevy::ecs::storage::SparseSetIndex;
use bevy::math::Vec3;
use bevy::prelude::Entity;
use crate::{Pixel, Vect3};

// Return true if the position is free to use.
pub fn check_pos(pixels: &BTreeMap<u128, Pixel>, current_position: Vect3, new_position: Vect3) -> bool {
    if new_position.y < 0.0 {
        return false;
    }

    if new_position.x > 50.0 || new_position.x < -50.0 {
        return false;
    }
    if new_position.z > 50.0 || new_position.z < -50.0 {
        return false;
    }

    // !transforms.iter().any(|i|
    //     i.x == pos.x && i.y == pos.y && i.z == pos.z
    // )
    for (position_index, pix) in pixels.iter() {
        let t= Vect3::from_index(*position_index);
        if (current_position.x == t.x && current_position.y == t.y && current_position.z == t.z) {
            // We are checking ourself, LOL, continue;
            continue;
        }
        if (t.x == new_position.x && t.y == new_position.y && t.z == new_position.z) {
            // if (position.is_some()) {
            //     // if (t.entity == id.unwrap()) {
            //     //     continue;
            //     // } else {
            //     //     return false;
            //     // }
            //     return false;
            // } else {
                return false;
            // }
        }
    }

    return true;
}