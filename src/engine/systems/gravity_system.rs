use bevy::prelude::{Commands, Mesh, Query, Res, ResMut, Time, Transform, With};
use crate::Pixel;

const TIMESTEP: f32 = 0.5;
static mut TIMER: f32 = 0.0;

pub fn gravity_system(
    mut pixels: Query<(&mut Pixel, &mut Transform)>,
    time: Res<Time>
) {
    unsafe {
        TIMER += time.delta_seconds();
        if (TIMER > TIMESTEP) {
            TIMER = 0.0;

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
    }
}