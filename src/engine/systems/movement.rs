use bevy::math::Vec3;
use rand::prelude::ThreadRng;
use crate::{PixelPositions};
use crate::engine::systems::chunk::PixelType;
use crate::engine::systems::pixels::{
    lava::lava_update_position,
    rock::rock_update_position,
    sand::sand_update_position,
    steam::steam_update_position,
    water::water_update_position,
};

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
            PixelType::Sand => sand_update_position(&mut pixel_transforms, rng, position_index, pixel),
            PixelType::Water => water_update_position(&mut pixel_transforms, rng, position_index, pixel),
            PixelType::Lava => lava_update_position(&mut pixel_transforms, rng, position_index, pixel),
            PixelType::Steam => steam_update_position(&mut pixel_transforms, rng, position_index, pixel),
            PixelType::Rock => rock_update_position(&mut pixel_transforms, rng, position_index, pixel),
            _ => {}
        }
    }
}