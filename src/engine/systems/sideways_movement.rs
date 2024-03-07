use bevy::prelude::{Fixed, Query, Res, Time, Transform, Vec3};
use crate::Pixel;
use rand;
use rand::Rng;
use crate::engine::check_position::check_pos;

pub fn sideways_movement(
    mut pixels: Query<(&Pixel, &mut Transform)>,
    _: Res<Time<Fixed>>
) {
    let mut transforms: Vec<_> = pixels.iter().map(|(_, transform)| transform.translation.clone()).collect();

    let mut rng = rand::thread_rng();

    for (id, (pixel, mut transform)) in pixels.iter_mut().enumerate() {
        let direction = rng.gen_range(0..=3);

        let offset = match direction {
            0 => Vec3::new(1.0, -1.0, 0.0),
            1 => Vec3::new(-1.0, -1.0, 0.0),
            2 => Vec3::new(0.0, -1.0, 1.0),
            3 => Vec3::new(0.0, -1.0, -1.0),
            _ => unreachable!()
        };

        if check_pos(transform.translation.clone() + offset, &transforms) {
            continue;
        }

        transform.translation += offset;
        transforms[id] += offset;
    }
}
