use bevy::math::Vec3;
use bevy::prelude::{Query, Transform};
use crate::Pixel;

// Return true if the position is free to use.
pub fn check_pos(pos: Vec3, transforms: &[Vec3]) -> bool {
    transforms.iter().any(|i|
        i.x == pos.x && i.y == pos.y && i.z == pos.z
    )
}