use bevy::prelude::{Query, Transform};
use crate::Pixel;
use rand;
use rand::Rng;
use crate::engine::systems::timestep_system::{TIMER, TIMESTEP};

pub fn sideways_movement(
    mut pixels: Query<(&Pixel, &mut Transform)>
) {
    // If its not time to update, return
    unsafe {
        if (TIMER < TIMESTEP) {
            return;
        }
    }

    for (mut pix, mut transform) in pixels.iter_mut() {
        let mut rng = rand::thread_rng();
        let direction = rng.gen_range(0..=3);
        match direction {
            0 => transform.translation.x += 1.0,
            1 => transform.translation.x -= 1.0,
            2 => transform.translation.z += 1.0,
            3 => transform.translation.z -= 1.0,
            _ => continue,
        }

    }
}