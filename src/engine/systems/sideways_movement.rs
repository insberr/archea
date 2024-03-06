use bevy::math::Vec3;
use bevy::prelude::{Query, Transform};
use crate::Pixel;
use rand;
use rand::Rng;
use crate::engine::check_position::check_pos;
use crate::engine::systems::timestep_system::{TIMER, TIMESTEP};

pub fn sideways_movement(
    mut pixels: Query<(&Pixel, &mut Transform)>,
) {
    // If its not time to update, return
    unsafe {
        if (TIMER < TIMESTEP) {
            return;
        }
    }

    for (pix, mut transform) in pixels.iter_mut() {
        let mut rng = rand::thread_rng();
        let direction = rng.gen_range(0..=3);
        match direction {
            0 => {
                let check = check_pos(pixels, Vec3::new(transform.translation.x, transform.translation.y, transform.translation.z));
                if check {
                    transform.translation.x += 1.0;
                }
            },
            1 => {
                let check = check_pos(pixels, Vec3::new(transform.translation.x, transform.translation.y, transform.translation.z));
                if check {
                    transform.translation.x -= 1.0;
                }
            },
            2 => {
                let check = check_pos(pixels, Vec3::new(transform.translation.x, transform.translation.y, transform.translation.z));
                if check {
                    transform.translation.z += 1.0;
                }
            },
            3 => {
                let check = check_pos(pixels, Vec3::new(transform.translation.x, transform.translation.y, transform.translation.z));
                if check {
                    transform.translation.z -= 1.0;
                }
            },
            _ => continue,
        }

    }
}