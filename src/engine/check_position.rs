use bevy::ecs::storage::SparseSetIndex;
use bevy::math::Vec3;
use bevy::prelude::Entity;
use crate::PixelTransform;

// Return true if the position is free to use.
pub fn check_pos(pos: Vec3, transforms: &Vec<PixelTransform>, id: Option<Entity>) -> bool {
    if pos.y < 0.0 {
        return false;
    }

    if pos.x > 50.0 || pos.x < -50.0 {
        return false;
    }
    if pos.z > 50.0 || pos.z < -50.0 {
        return false;
    }

    // !transforms.iter().any(|i|
    //     i.x == pos.x && i.y == pos.y && i.z == pos.z
    // )
    for t in transforms.iter() {
        if (t.transform.translation.x == pos.x && t.transform.translation.y == pos.y && t.transform.translation.z == pos.z) {
            if (id.is_some()) {
                if (t.entity == id.unwrap()) {
                    continue;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
    }

    return true;
}