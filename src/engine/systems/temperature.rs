use rand::prelude::ThreadRng;
use crate::{PixelPositions};
use crate::engine::systems::chunk::PixelType;
use crate::engine::systems::pixels::{
    lava::lava_update_temp,
    water::water_update_temp,
    steam::steam_update_temp,
};

pub fn update_pixel_temperatures(
    mut pixel_transforms: &mut PixelPositions,
    mut rng: &mut ThreadRng,
) {
    // Update every pixel's position and temperature
    for (position_index, pixel) in pixel_transforms.map.clone().iter_mut() {
        match pixel.pixel_type {
            // PixelType::Sand => sand_update(&mut pixel_transforms, rng, position_index, pixel),
            PixelType::Water => water_update_temp(&mut pixel_transforms, rng, position_index, pixel),
            PixelType::Lava => lava_update_temp(&mut pixel_transforms, rng, position_index, pixel),
            PixelType::Steam => steam_update_temp(&mut pixel_transforms, rng, position_index, pixel),
            // PixelType::Rock => rock_update(&mut pixel_transforms, rng, position_index, pixel),
            _ => {}
        }
    }
}