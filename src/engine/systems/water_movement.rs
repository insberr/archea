use std::collections::BTreeMap;
use bevy::prelude::{Fixed, Res, ResMut, Time, Vec3};
use crate::{PixelPositions, PixelType};
use rand;
use rand::Rng;
use rand::rngs::ThreadRng;
use crate::engine::check_position::check_pos;

const _DOWN: Vec3 = Vec3::new(0.0, -1.0, 0.0);
const _LEFT: Vec3 = Vec3::new(-1.0, 0.0, 0.0);
const _RIGHT: Vec3 = Vec3::new(1.0, 0.0, 0.0);
const _FORWARD: Vec3 = Vec3::new(0.0, 0.0, 1.0);
const _BACKWARD: Vec3 = Vec3::new(0.0, 0.0, -1.0);

pub fn water_movement(
    mut pixel_transforms: ResMut<PixelPositions>,
    _: Res<Time<Fixed>>
) {
    let mut rng: ThreadRng = rand::thread_rng();
    let mut pixel_transforms_clone = BTreeMap::new();
    let mut is_dirty = pixel_transforms.is_map_dirty;

    pixel_transforms_clone.clone_from(&pixel_transforms.map);

    for (position_index, pixel) in pixel_transforms.map.iter_mut() {
        if pixel.pixel_type != PixelType::Water {
            continue;
        }

        if pixel.dont_move {
            pixel.dont_move = false;
            continue;
        }

        let mut position = *position_index;

        let dir_num1 = rng.gen_range(0..=1);
        let dir_num2 = rng.gen_range(0..=1);
        let dir1 = if dir_num1 == 0 { _LEFT } else { _RIGHT };
        let dir2 = if dir_num2 == 0 { _FORWARD } else { _BACKWARD };

        let mut direction = _DOWN;

        if check_pos(&pixel_transforms_clone, position, position + _DOWN) {
            // Already set to down
            // direction = _Down;
        } else if check_pos(&pixel_transforms_clone, position,position + _DOWN + dir1) {
            direction = _DOWN + dir1;
        } else if check_pos(&pixel_transforms_clone, position,position + _DOWN + dir2) {
            direction = _DOWN + dir2;
        } else if check_pos(&pixel_transforms_clone, position, position + dir1) {
            direction = dir1;
        } else if check_pos(&pixel_transforms_clone, position, position + dir2) {
            direction = dir2;
        } else {
            // pixel.dont_move = true;
            continue;
        }

        // if check_pos(position + _DOWN, &pixel_transforms.positions, None) {
        //     // Already set to down
        //     // direction = _Down;
        // } else if check_pos(position + _DOWN + dir1, &pixel_transforms.positions, None) {
        //     direction = _DOWN + dir1;
        // } else if check_pos(position + _DOWN + dir2, &pixel_transforms.positions, None) {
        //     direction = _DOWN + dir2;
        // } else if check_pos(position + dir1, &pixel_transforms.positions, None) {
        //     direction = dir1;
        // } else if check_pos(position + dir2, &pixel_transforms.positions, None) {
        //     direction = dir2;
        // } else {
        //     pixel.dont_move = true;
        //     continue;
        // }

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
        pixel_transforms_clone.insert(position, pixel.clone());
        pixel_transforms_clone.remove(position_index);
        is_dirty = true;
    }
    pixel_transforms.is_map_dirty = is_dirty;
    pixel_transforms.map = pixel_transforms_clone;
    pixel_transforms = pixel_transforms;
}
