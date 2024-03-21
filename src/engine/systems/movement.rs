use bevy::math::Vec3;
use rand::prelude::ThreadRng;
use crate::{PixelPositions, PixelType};
use crate::engine::systems::lava_movement::lava_update;
use crate::engine::systems::rock::rock_update;
use crate::engine::systems::sand_movement::sand_update;
use crate::engine::systems::steam::steam_update;
use crate::engine::systems::water_movement::water_update;

/* Constants */
pub const _DOWN: Vec3 = Vec3::new(0.0, -1.0, 0.0);
pub const _UP: Vec3 = Vec3::new(0.0, 1.0, 0.0);
pub const _LEFT: Vec3 = Vec3::new(-1.0, 0.0, 0.0);
pub const _RIGHT: Vec3 = Vec3::new(1.0, 0.0, 0.0);
pub const _FORWARD: Vec3 = Vec3::new(0.0, 0.0, 1.0);
pub const _BACKWARD: Vec3 = Vec3::new(0.0, 0.0, -1.0);

pub fn update_pixel_positions(
    mut pixel_transforms: &mut PixelPositions,
    mut rng: &mut ThreadRng,
) {
    // Update every pixel's position and temperature
    for (position_index, pixel) in pixel_transforms.map.clone().iter_mut() {
        match pixel.pixel_type {
            PixelType::Sand => sand_update(&mut pixel_transforms, rng, position_index, pixel),
            PixelType::Water => water_update(&mut pixel_transforms, rng, position_index, pixel),
            PixelType::Lava => lava_update(&mut pixel_transforms, rng, position_index, pixel),
            PixelType::Steam => steam_update(&mut pixel_transforms, rng, position_index, pixel),
            PixelType::Rock => rock_update(&mut pixel_transforms, rng, position_index, pixel),
            _ => {}
        }
    }
}