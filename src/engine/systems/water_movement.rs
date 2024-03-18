use std::collections::BTreeMap;
use bevy::prelude::{Fixed, Res, ResMut, Time, Vec3};
use crate::{Pixel, PixelPositions, PixelType};
use rand;
use rand::prelude::SliceRandom;
use rand::Rng;
use rand::rngs::ThreadRng;
use crate::engine::check_position::check_pos;
use crate::engine::stuff::vect3::Vect3;

use crate::engine::systems::movement::{_DOWN, _LEFT, _RIGHT, _FORWARD, _BACKWARD};

pub fn water_update(
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

    let mut new_pixel = pixel.clone();
    new_pixel.pixel_temperature += 2.0;
    pixel_transforms.map.insert(*position, new_pixel);

    if pixel.pixel_temperature > 120.0 {
        let mut new_pixel = pixel.clone();
        new_pixel.pixel_type = PixelType::Steam;
        pixel_transforms.map.insert(*position, new_pixel);
        return;
    }

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
