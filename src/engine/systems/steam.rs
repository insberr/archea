use rand::prelude::{SliceRandom, ThreadRng};
use rand::Rng;
use crate::engine::stuff::vect3::Vect3;
use crate::{Pixel, PixelPositions, PixelType};
use crate::engine::check_position::check_pos;
use crate::engine::systems::movement::{_BACKWARD, _DOWN, _FORWARD, _LEFT, _RIGHT, _UP};

pub fn steam_update(
    mut pixel_transforms: &mut PixelPositions,
    mut rng: &mut ThreadRng,
    position: &Vect3,
    pixel: &Pixel,
) {
    if pixel.dont_move {
        let mut new_pixel = pixel.clone();
        new_pixel.dont_move = false;
        pixel_transforms.map.insert(*position, new_pixel);
        return;
    }

    let dir_num1 = rng.gen_range(0..=1);
    let dir_num2 = rng.gen_range(0..=1);
    let dir1 = if dir_num1 == 0 { _LEFT } else { _RIGHT };
    let dir2 = if dir_num2 == 0 { _FORWARD } else { _BACKWARD };

    let mut check_directions = vec![
        _UP,
        _UP + dir1,
        _UP + dir2,
        dir1,
        dir2
    ];
    check_directions.shuffle(rng);

    for dir in check_directions.iter() {
        // Check if it can move - the position is an air spot
        let can_move = check_pos(&pixel_transforms.map, position, &(position + dir));

        if (can_move) {
            let new_position = *position + *dir;
            pixel_transforms.map.insert(new_position, pixel.clone());
            pixel_transforms.map.remove(position);
            pixel_transforms.is_map_dirty = true;
            break;
        }
    }
}