use crate::engine::systems::chunk::PixelType;
use std::collections::BTreeMap;
use bevy::prelude::{Fixed, Res, ResMut, Time, Vec3};
use crate::{Pixel, PixelPositions};
use rand;
use rand::Rng;
use rand::rngs::ThreadRng;
use crate::engine::check_position::check_pos;
use crate::engine::stuff::vect3::Vect3;

use crate::engine::systems::movement::{_DOWN, _LEFT, _RIGHT, _FORWARD, _BACKWARD};

pub fn lava_update_position(
    mut pixel_transforms: &mut PixelPositions,
    mut rng: &mut ThreadRng,
    position: &Vect3,
    pixel: &Pixel,
) {
    if pixel.dont_move {
        let mut new_pixel = pixel.clone();
        new_pixel.dont_move = false;
        pixel_transforms.map.insert(*position, new_pixel);
        return;
    }

    let dir_num1 = rng.gen_range(0..=1);
    let dir_num2 = rng.gen_range(0..=1);
    let dir1 = if dir_num1 == 0 { _LEFT } else { _RIGHT };
    let dir2 = if dir_num2 == 0 { _FORWARD } else { _BACKWARD };

    let check_directions = vec![
        _DOWN,
        _DOWN + dir1,
        _DOWN + dir2,
    ];

    for dir in check_directions.iter() {
        // Check if it can move - the position is an air spot
        let can_move = check_pos(&pixel_transforms.map, position, &(position + dir));

        // Check swap rules - say, water is in the spot to move to
        // This should probably be moved...
        if let Some(pix_at_dir) = pixel_transforms.map.get(&(*position + *dir)) {
            match pix_at_dir.pixel_type {
                PixelType::Water => {
                    // swap places
                    let temp_pixel = pix_at_dir.clone();
                    pixel_transforms.map.insert(*position + *dir, pixel.clone());
                    pixel_transforms.map.insert(*position, temp_pixel);
                    // direction = Vec3::new(0.0, 0.0, 0.0);
                    pixel_transforms.is_map_dirty = true;
                    break;
                },
                _ => {}
            }
        }

        if (can_move) {
            let new_position = *position + *dir;
            pixel_transforms.map.insert(new_position, pixel.clone());
            pixel_transforms.map.remove(position);
            pixel_transforms.is_map_dirty = true;
            break;
        }
    }
}

pub fn lava_update_temp(
    mut pixel_transforms: &mut PixelPositions,
    mut rng: &mut ThreadRng,
    position: &Vect3,
    pixel: &Pixel,
) {
    let mut new_pixel = pixel.clone();
    let temp = new_pixel.temperature;

    let rand_variation = rng.gen_range(-0.09..=0.09);

    if temp > 2_000.0 {
        // todo get average of surrounding and set all to the average.
        new_pixel.temperature -= 100.0 + (temp * rand_variation); // some rate, to do later
    } else if (temp > 1_000.0) {
        new_pixel.temperature -= 50.0 + (temp * rand_variation); // some rate, to do later
    } else {
        // Convert to rock, the rest will be handled later I suppose
        new_pixel.temperature = 60.0; //  TEMPORARY
        new_pixel.pixel_type = PixelType::Rock;
    }

    // Set the updated pixel
    pixel_transforms.map.insert(*position, new_pixel);
    // We can assume that lava is always going to be changing temperature
    pixel_transforms.is_map_dirty = true;
}
