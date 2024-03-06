use bevy::math::Vec3;
use bevy::prelude::{Query, Transform};
use crate::Pixel;

// Return true if the position is free to use.
pub fn check_pos(
    pixels: Query<(&Pixel, &mut Transform)>,
    position: Vec3,
) -> bool {
    for (pix, transform) in pixels.iter() {
        if (transform.translation.x == position.x && transform.translation.y == position.y && transform.translation.z == position.z) {
            return false;
        }
    }

    return true;
}