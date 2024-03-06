use bevy::prelude::{Query, Transform};
use crate::{Pixel};
use crate::engine::systems::timestep_system::{TIMER, TIMESTEP};

pub fn gravity_system(
    mut pixels: Query<(&mut Pixel, &mut Transform)>
) {
    // If its not time to update, return
    unsafe {
        if (!(TIMER > TIMESTEP)) {
            return;
        }
    }

    for (mut pix, mut transform) in pixels.iter_mut() {
        if pix.dont_move {
            pix.dont_move = false;
            continue;
        }

        if (transform.translation.y > 1.0) {
            transform.translation.y -= 1.0;
        }
    }
}