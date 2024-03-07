use bevy::prelude::{Fixed, Local, Query, Res, ResMut, Time, Transform, Vec3, With};
use crate::{Pixel, PixelPositions, PixelSand, PixelTransform};
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
    mut pixels: Query<(&mut Pixel, &mut Transform), With<PixelSand>>,
    mut pixel_transforms: ResMut<PixelPositions>,
    _: Res<Time<Fixed>>
) {
    // let mut transforms: Vec<_> = pixels.iter().map(|(transform)| transform.translation.clone()).collect();
    let mut rng: ThreadRng = rand::thread_rng();

    for (id, (mut pixel, mut transform)) in pixels.iter_mut().enumerate() {
        if pixel.dont_move {
            pixel.dont_move = false;
            continue;
        }

        let dir_num = rng.gen_range(0..=1);
        let dir1 = if dir_num == 0 { _LEFT } else { _RIGHT };
        let dir2 = if dir_num == 0 { _FORWARD } else { _BACKWARD };

        let position = transform.translation.clone();
        let mut direction = _DOWN;

        if !check_pos(position, &pixel_transforms.positions, Some(pixel.id)) {
            direction = Vec3::new(0.0, 1.0, 0.0);
            pixel.dont_move = true;
        } else if check_pos(position + _DOWN, &pixel_transforms.positions, None) {
            // Already set to down
            // direction = _Down;
        } else if check_pos(position + _DOWN + dir1, &pixel_transforms.positions, None) {
            direction = _DOWN + dir1;
        } else if check_pos(position + _DOWN + dir2, &pixel_transforms.positions, None) {
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
        transform.translation += direction;
        // pixel_transforms[id] += direction;
        // pixel_transforms.iter_mut().find(|val| val.entity == pixel.id).unwrap().transform.translation += direction;
        pixel_transforms.positions.iter_mut().find(|val| val.entity == pixel.id).unwrap_or(&mut PixelTransform {
            transform: Transform::from_xyz(0.0, 0.0, 0.0),
            entity: pixel.id
        }).transform.translation += direction;
    }
}
