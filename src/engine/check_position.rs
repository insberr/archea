use std::cmp::min;
use std::collections::BTreeMap;
use bevy::utils::default;
use ordered_float::OrderedFloat;
use rand::prelude::ThreadRng;

use crate::{Pixel, PixelPositions, PixelType};
use crate::engine::stuff::vect3::Vect3;

// Return true if the position is free to use.
pub fn check_pos(pixels: &BTreeMap<Vect3, Pixel>, current_position: &Vect3, new_position: &Vect3) -> bool {
    if new_position == current_position {
        panic!("WTF ARE YOU DOING!!");
    }

    if *new_position.y < 0.0 || *new_position.y > 50.0 {
        return false;
    }

    if *new_position.x > 50.0 || *new_position.x < -50.0 {
        return false;
    }
    if *new_position.z > 50.0 || *new_position.z < -50.0 {
        return false;
    }


    if pixels.contains_key(&new_position) {
        return false;
    }

    true
}

pub fn average_temp(
    mut pixel_transforms: &mut PixelPositions,
    mut rng: &mut ThreadRng,
    current_position: &Vect3,
    current_pixel: &Pixel,
    check_radius: u8,
    max_increase: f32,
) -> f32 {
    let mut temps: Vec<f32> = vec![];

    // this is sort of temporary

    // x - 1
    temps.push(pixel_transforms.map.get(&(*current_position + Vect3::new(-1.0,0.0,0.0))).unwrap_or(&(Pixel { pixel_type: PixelType::Invalid, pixel_temperature: 60.0, dont_move: false })).pixel_temperature);
    temps.push(pixel_transforms.map.get(&(*current_position + Vect3::new(1.0,0.0,0.0))).unwrap_or(&(Pixel { pixel_type: PixelType::Invalid, pixel_temperature: 60.0, dont_move: false })).pixel_temperature);

    // y
    temps.push(pixel_transforms.map.get(&(*current_position + Vect3::new(0.0,-1.0,0.0))).unwrap_or(&(Pixel { pixel_type: PixelType::Invalid, pixel_temperature: 60.0, dont_move: false })).pixel_temperature);
    temps.push(pixel_transforms.map.get(&(*current_position + Vect3::new(0.0,1.0,0.0))).unwrap_or(&(Pixel { pixel_type: PixelType::Invalid, pixel_temperature: 60.0, dont_move: false })).pixel_temperature);

    // z
    temps.push(pixel_transforms.map.get(&(*current_position + Vect3::new(0.0,0.0,-1.0))).unwrap_or(&(Pixel { pixel_type: PixelType::Invalid, pixel_temperature: 60.0, dont_move: false })).pixel_temperature);
    temps.push(pixel_transforms.map.get(&(*current_position + Vect3::new(0.0,0.0,1.0))).unwrap_or(&(Pixel { pixel_type: PixelType::Invalid, pixel_temperature: 60.0, dont_move: false })).pixel_temperature);

    let mut average = current_pixel.pixel_temperature;
    for temp in temps.iter() {
        average += temp;
    }

    *min(OrderedFloat(current_pixel.pixel_temperature + max_increase), OrderedFloat(average / temps.len() as f32))
}