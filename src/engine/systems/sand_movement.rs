use std::collections::BTreeMap;
use bevy::prelude::{Fixed, Local, Query, Res, ResMut, Time, Transform, Vec3, With};
use crate::{Pixel, PixelPositions, PixelType, Vect3};
use rand;
use rand::Rng;
use rand::rngs::ThreadRng;
use crate::engine::check_position::check_pos;

const _DOWN: Vec3 = Vec3::new(0.0, -1.0, 0.0);
const _LEFT: Vec3 = Vec3::new(-1.0, 0.0, 0.0);
const _RIGHT: Vec3 = Vec3::new(1.0, 0.0, 0.0);
const _FORWARD: Vec3 = Vec3::new(0.0, 0.0, 1.0);
const _BACKWARD: Vec3 = Vec3::new(0.0, 0.0, -1.0);

pub fn sand_movement(
    // mut pixels: Query<(&mut Pixel, &mut Transform), With<PixelSand>>,
    mut pixel_transforms: ResMut<PixelPositions>,
    _: Res<Time<Fixed>>
) {
    // let mut transforms: Vec<_> = pixels.iter().map(|(transform)| transform.translation.clone()).collect();
    let mut rng: ThreadRng = rand::thread_rng();
    let mut pixel_transforms_clone = BTreeMap::new();
    // for (position_index, mut pixel) in pixel_transforms.map.iter() {
    //     pixel_transforms_clone.insert(*position_index, pixel.clone());
    // }
    pixel_transforms_clone.clone_from(&pixel_transforms.map);

    for (position_index, mut pixel) in pixel_transforms.map.iter_mut() {
        if pixel.pixel_type != PixelType::Sand {
            continue;
        }

        if pixel.dont_move {
            pixel.dont_move = false;
            continue;
        }

        let mut position = Vect3::from_index(*position_index);

        let dir_num = rng.gen_range(0..=1);
        let dir1 = if dir_num == 0 { _LEFT } else { _RIGHT };
        let dir2 = if dir_num == 0 { _FORWARD } else { _BACKWARD };

        let mut direction = _DOWN;

        // if !check_pos(&pixel_transforms_clone, position, position) {
        //     direction = Vec3::new(0.0, 1.0, 0.0);
        //     pixel.dont_move = true;
        // } else
        if check_pos(&pixel_transforms_clone, position, position.clone() + _DOWN) {
            // Already set to down
            // direction = _Down;
        } else if check_pos(&pixel_transforms_clone, position,position.clone() + _DOWN + dir1) {
            direction = _DOWN + dir1;
        } else if check_pos(&pixel_transforms_clone, position,position.clone() + _DOWN + dir2) {
            direction = _DOWN + dir2;
        // } else if check_pos(position + dir1, &pixel_transforms, None) {
        //     direction = dir1;
        // } else if check_pos(position + dir2, &pixel_transforms, None) {
        //     direction = dir2;
        } else {
            pixel.dont_move = true;
            continue;
        }

        // pixel.dont_move = true;
        // transform.translation += direction;
        // // pixel_transforms[id] += direction;
        // // pixel_transforms.iter_mut().find(|val| val.entity == pixel.id).unwrap().transform.translation += direction;
        // pixel_transforms.positions.iter_mut().find(|val| val.entity == pixel.id).unwrap_or(&mut PixelTransform {
        //     transform: Transform::from_xyz(0.0, 0.0, 0.0),
        //     entity: pixel.id
        // }).transform.translation += direction;

        // pixel.dont_move = true;
        position += direction;
        pixel_transforms_clone.insert(position.to_index(), pixel.clone());
        pixel_transforms_clone.remove(position_index);
    }
    pixel_transforms.map = pixel_transforms_clone;
    pixel_transforms = pixel_transforms;
}
