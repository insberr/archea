use std::collections::BTreeMap;
use bevy::prelude::{Fixed, Res, ResMut, Time, Vec3};
use crate::{Pixel, PixelPositions, PixelType};
use rand;
use rand::prelude::SliceRandom;
use rand::Rng;
use rand::rngs::ThreadRng;
use crate::engine::check_position::{average_temp, check_pos};
use crate::engine::stuff::vect3::Vect3;

use crate::engine::systems::movement::{_DOWN, _LEFT, _RIGHT, _FORWARD, _BACKWARD};

pub fn water_update_position(
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
        dir1,
        dir2,
    ];

    for dir in check_directions.iter() {
        // Check if it can move - the position is an air spot
        let can_move = check_pos(&pixel_transforms.map, position, &(position + dir));

        if can_move {
            let new_position = *position + *dir;
            pixel_transforms.map.insert(new_position, pixel.clone());
            pixel_transforms.map.remove(position);
            pixel_transforms.is_map_dirty = true;
            break;
        }
    }
}

pub fn water_update_temp(
    mut pixel_transforms: &mut PixelPositions,
    mut rng: &mut ThreadRng,
    position: &Vect3,
    pixel: &Pixel,
) {
    let mut dirty = pixel_transforms.is_map_dirty;

    let mut new_pixel = pixel.clone();

    // This is way too fast, need to figure out to slowly up the temp based on the average surrounding temp
    new_pixel.pixel_temperature = average_temp(pixel_transforms,rng,position,pixel,1, 10.0);

    let temp = new_pixel.pixel_temperature;

    if (temp > 120.0) {
        new_pixel.pixel_type = PixelType::Steam;
        dirty = true;
    } else if (temp > 60.0) {
        new_pixel.pixel_temperature -= 2.0;
        dirty = true;
    } else if (temp < 32.0) {
        dirty = true;
        new_pixel.pixel_type = PixelType::Invalid; // Ice (to do)
    }

    pixel_transforms.map.insert(*position, new_pixel);
    pixel_transforms.is_map_dirty = dirty;
}
