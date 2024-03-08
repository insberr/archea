use std::collections::BTreeMap;
use bevy::ecs::storage::SparseSetIndex;
use bevy::math::Vec3;
use bevy::prelude::Entity;
use crate::{Pixel, Vect3};

// Return true if the position is free to use.
pub fn check_pos(pixels: &BTreeMap<u128, Pixel>, position: Vect3) -> bool {
    if position.y < 0.0 {
        return false;
    }

    if position.x > 50.0 || position.x < -50.0 {
        return false;
    }
    if position.z > 50.0 || position.z < -50.0 {
        return false;
    }

    // !transforms.iter().any(|i|
    //     i.x == pos.x && i.y == pos.y && i.z == pos.z
    // )
    for (position_index, pix) in pixels.iter() {
        let t= Vect3::from_index(*position_index);
        if (t.x == position.x && t.y == position.y && t.z == position.z) {
            if (position.is_some()) {
                // if (t.entity == id.unwrap()) {
                //     continue;
                // } else {
                //     return false;
                // }
                return false;
            } else {
                return false;
            }
        }
    }

    return true;
}