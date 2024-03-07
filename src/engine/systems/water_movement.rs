use std::iter::Map;
use bevy::prelude::{Entity, Fixed, Local, Query, Res, Time, Transform, Vec3, With, Without};
use crate::{Pixel, PixelTransform, PixelWater};
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
    mut pixels: Query<(&mut Pixel, &mut Transform), With<PixelWater>>,
    mut pixel_transforms: Local<Vec<PixelTransform>>,
    _: Res<Time<Fixed>>
) {
    // let mut transforms: Vec<Transform> = pixel_transforms.map(|transform| transform.clone()).collect();
    let mut rng: ThreadRng = rand::thread_rng();

    for (id, (mut pixel, mut transform)) in pixels.iter_mut().enumerate() {
        if pixel.dont_move {
            pixel.dont_move = false;
            continue;
        }

        // pixel_transforms[pixel.id] = transform;

        let dir_num = rng.gen_range(0..=1);
        let dir1 = if dir_num == 0 { _LEFT } else { _RIGHT };
        let dir2 = if dir_num == 0 { _FORWARD } else { _BACKWARD };

        let position = transform.translation.clone();
        let mut direction = _DOWN;

        if check_pos(position, &pixel_transforms) {
            direction = Vec3::new(0.0, 1.0, 0.0);
            pixel.dont_move = true;
        } else if check_pos(position + _DOWN, &pixel_transforms) {
            // Already set to down
            // direction = _Down;
        } else if check_pos(position + _DOWN + dir1, &pixel_transforms) {
            direction = _DOWN + dir1;
        } else if check_pos(position + _DOWN + dir2, &pixel_transforms) {
            direction = _DOWN + dir2;
        } else if check_pos(position + dir1, &pixel_transforms) {
            direction = dir1;
        } else if check_pos(position + dir2, &pixel_transforms) {
            direction = dir2;
        } else {
            pixel.dont_move = true;
            continue;
        }

        // pixel.dont_move = true;
        transform.translation += direction;
        // pixel_transforms[id] += direction;
        pixel_transforms[pixel.id] += direction;
    }
}
