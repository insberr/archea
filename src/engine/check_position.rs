use bevy::math::Vec3;
use crate::PixelTransform;

// Return true if the position is free to use.
pub fn check_pos(pos: Vec3, transforms: &Vec<PixelTransform>) -> bool {
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
        if (t.x == pos.x && t.y == pos.y && t.z == pos.z) {
            return false;
        }
    }

    return true;
}