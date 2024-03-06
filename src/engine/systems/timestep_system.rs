use bevy::prelude::{Res, Time};

pub const TIMESTEP: f32 = 0.2;
pub static mut TIMER: f32 = 0.0;

static mut RESET: bool = false;

pub fn time_step(
    time: Res<Time>
) {
    unsafe {
        TIMER += time.delta_seconds();
        if (RESET) {
            TIMER = 0.0;
            RESET = false;
        }
        if (TIMER > TIMESTEP) {
            RESET = true;
        }
    }
}