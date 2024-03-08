use std::iter::Map;
use bevy::prelude::{Entity, Fixed, Local, Query, Res, ResMut, Time, Transform, Vec3, With, Without};
use crate::{Pixel, PixelPositions};
use crate::engine::check_position::check_pos;

const _DOWN: Vec3 = Vec3::new(0.0, -1.0, 0.0);
const _LEFT: Vec3 = Vec3::new(-1.0, 0.0, 0.0);
const _RIGHT: Vec3 = Vec3::new(1.0, 0.0, 0.0);
const _FORWARD: Vec3 = Vec3::new(0.0, 0.0, 1.0);
const _BACKWARD: Vec3 = Vec3::new(0.0, 0.0, -1.0);

pub fn unmovable_movement(
    // mut pixels: Query<(&mut Pixel, &mut Transform), With<PixelUnmovable>>,
    mut pixel_transforms: ResMut<PixelPositions>,
    _: Res<Time<Fixed>>
) {
    for (position_index, mut pixel) in pixel_transforms.iter_mut() {
        if pixel.dont_move {
            pixel.dont_move = false;
            continue;
        }

        let dir_num = 0; // rng.gen_range(0..=1);
        let dir1 = if dir_num == 0 { _LEFT } else { _RIGHT };
        let dir2 = if dir_num == 0 { _FORWARD } else { _BACKWARD };

        let position = transform.translation.clone();
        let mut direction = _DOWN;

        if !check_pos(position, &pixel_transforms.positions, Some(pixel.id)) {
            direction = Vec3::new(0.0, 1.0, 0.0);
            pixel.dont_move = true;
        } else {
            continue;
        }
        // } else if check_pos(position + _DOWN, &pixel_transforms.positions, None) {
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
        transform.translation += direction;
        // pixel_transforms[id] += direction;
        pixel_transforms.positions.iter_mut().find(|val| val.entity == pixel.id).unwrap_or(&mut PixelTransform {
            transform: Transform::from_xyz(0.0, 0.0, 0.0),
            entity: pixel.id
        }).transform.translation += direction;
        // pixel_transforms[pixel.id] += direction;
    }
}
