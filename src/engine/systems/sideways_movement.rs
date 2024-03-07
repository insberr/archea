use bevy::prelude::{Fixed, Query, Res, Time, Transform, Vec3};
use crate::Pixel;
use rand;
use rand::Rng;
use crate::engine::check_position::check_pos;

const _DOWN: Vec3 = Vec3::new(0.0, -1.0, 0.0);
const _LEFT: Vec3 = Vec3::new(-1.0, 0.0, 0.0);
const _RIGHT: Vec3 = Vec3::new(1.0, 0.0, 0.0);
const _FORWARD: Vec3 = Vec3::new(0.0, 0.0, 1.0);
const _BACKWARD: Vec3 = Vec3::new(0.0, 0.0, -1.0);


pub fn sideways_movement(
    mut pixels: Query<(&mut Pixel, &mut Transform)>,
    _: Res<Time<Fixed>>
) {
    let mut transforms: Vec<_> = pixels.iter().map(|(_, transform)| transform.translation.clone()).collect();

    let mut rng = rand::thread_rng();
    let dir_num = rng.gen_range(0..=1);
    let dir1 = if dir_num == 0 { _LEFT } else { _RIGHT };
    let dir2 = if dir_num == 0 { _FORWARD } else { _BACKWARD };

    for (id, (mut pixel, mut transform)) in pixels.iter_mut().enumerate() {
        if pixel.dont_move {
            pixel.dont_move = false;
            continue;
        }

        let position = transform.translation.clone();
        let mut direction = _DOWN;

        if check_pos(position, &transforms) {
            direction = Vec3::new(0.0, 1.0, 0.0);
            pixel.dont_move = true;
        } else if check_pos(position + _DOWN, &transforms) {
            // Already set to down
            // direction = _Down;
        } else if check_pos(position + _DOWN + dir1, &transforms) {
            direction = _DOWN + dir1;
        } else if check_pos(position + _DOWN + dir2, &transforms) {
            direction = _DOWN + dir2;
        } else if check_pos(position + dir1, &transforms) {
            direction = dir1;
        } else if check_pos(position + dir2, &transforms) {
            direction = dir2;
        } else {
            pixel.dont_move = true;
            continue;
        }

        // pixel.dont_move = true;
        transform.translation += direction;
        transforms[id] += direction;
    }
}
