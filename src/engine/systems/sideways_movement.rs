use bevy::log::tracing_subscriber::fmt::writer::EitherWriter::B;
use bevy::prelude::{Entity, Fixed, Query, Res, Time, Transform, Vec3};
use bevy::prelude::MouseButton::Forward;
use crate::Pixel;
use rand;
use rand::Rng;
use crate::engine::check_position::check_pos;

const _Down: Vec3 = Vec3::new(0.0, -1.0, 0.0);
const _Left: Vec3 = Vec3::new(-1.0, 0.0, 0.0);
const _Right: Vec3 = Vec3::new(1.0, 0.0, 0.0);
const _Forward: Vec3 = Vec3::new(0.0, 0.0, 1.0);
const _Backward: Vec3 = Vec3::new(0.0, 0.0, -1.0);


pub fn sideways_movement(
    mut pixels: Query<(&mut Pixel, &mut Transform)>,
    _: Res<Time<Fixed>>
) {
    let mut transforms: Vec<_> = pixels.iter().map(|(_, transform)| transform.translation.clone()).collect();

    let mut rng = rand::thread_rng();
    let dir_num = rng.gen_range(0..=1);
    let dir1 = if dir_num == 0 { _Left } else { _Right };
    let dir2 = if dir_num == 0 { _Forward } else { _Backward };

    for (id, (mut pixel, mut transform)) in pixels.iter_mut().enumerate() {
        if (pixel.dont_move) {
            pixel.dont_move = false;
            continue;
        }

        let position = transform.translation.clone();
        let mut direction = _Down;

        if check_pos(position, &transforms) {
            direction = Vec3::new(0.0, 1.0, 0.0);
        } else if check_pos(position + _Down, &transforms) {
            direction = _Down;
        } else if check_pos(position + _Down + dir1, &transforms) {
            direction = _Down + dir1;
        // } else if check_pos(position + _Down + _Right, &transforms) {
        //     direction = _Down + _Right;
        } else if check_pos(position + _Down + dir2, &transforms) {
            direction = _Down + dir2;
        // } else if check_pos(position + _Down + _Backward, &transforms) {
        //     direction = _Down + _Backward;
        } else if check_pos(position + dir1, &transforms) {
            direction = dir1;
        // } else if check_pos(position + _Right, &transforms) {
        //     direction = _Right;
        } else if check_pos(position + dir2, &transforms) {
            direction = dir2;
        // } else if check_pos(position + _Backward, &transforms) {
        //     direction = _Backward;
        } else {
            pixel.dont_move = true;
            continue;
        }

        pixel.dont_move = true;
        transform.translation += direction;
        transforms[id] += direction;

        // let direction = rng.gen_range(0..=3);
        //
        // let offset = match direction {
        //     0 => Vec3::new(1.0, -1.0, 0.0),
        //     1 => Vec3::new(-1.0, -1.0, 0.0),
        //     2 => Vec3::new(0.0, -1.0, 1.0),
        //     3 => Vec3::new(0.0, -1.0, -1.0),
        //     _ => unreachable!()
        // };
        //
        // if !check_pos(transform.translation.clone() + offset, &transforms) {
        //     pixel.dont_move = true;
        //     continue;
        // }
        //
        // transform.translation += offset;
        // transforms[id] += offset;
    }
}
