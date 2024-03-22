use rand::prelude::ThreadRng;
use crate::{Pixel, PixelPositions};
use crate::engine::stuff::vect3::Vect3;

pub fn unmovable_update_position(
    mut pixel_transforms: &mut PixelPositions,
    mut rng: &mut ThreadRng,
    position: &Vect3,
    pixel: &Pixel,
) {
    return;
}
