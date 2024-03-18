use std::collections::BTreeMap;
use bevy::prelude::{Fixed, Res, ResMut, Time, Vec3};
use bytemuck::cast_vec;
use crate::{PixelPositions, PixelType};
use rand;
use rand::Rng;
use rand::rngs::ThreadRng;
use crate::engine::check_position::check_pos;

const _DOWN: Vec3 = Vec3::new(0.0, -1.0, 0.0);
const _LEFT: Vec3 = Vec3::new(-1.0, 0.0, 0.0);
const _RIGHT: Vec3 = Vec3::new(1.0, 0.0, 0.0);
const _FORWARD: Vec3 = Vec3::new(0.0, 0.0, 1.0);
const _BACKWARD: Vec3 = Vec3::new(0.0, 0.0, -1.0);

enum Directions {
    D_EMPTY,
    D_DOWN,
    D_UP,
    D_LEFT,
    D_RIGHT,
    D_FORWARD,
    D_BACKWARD,
    D_RAND_LR,
    D_RAND_FB
}

pub fn sand_movement(
    // mut pixels: Query<(&mut Pixel, &mut Transform), With<PixelSand>>,
    mut pixel_transforms: ResMut<PixelPositions>,
    _: Res<Time<Fixed>>
) {
    // let mut transforms: Vec<_> = pixels.iter().map(|(transform)| transform.translation.clone()).collect();
    let mut rng: ThreadRng = rand::thread_rng();
    let mut pixel_transforms_clone = BTreeMap::new();
    let mut is_dirty = pixel_transforms.is_map_dirty;
    // for (position_index, mut pixel) in pixel_transforms.map.iter() {
    //     pixel_transforms_clone.insert(*position_index, pixel.clone());
    // }
    pixel_transforms_clone.clone_from(&pixel_transforms.map);

    for (position_index, pixel) in pixel_transforms.map.iter_mut() {
        if pixel.pixel_type != PixelType::Sand {
            continue;
        }

        if pixel.dont_move {
            pixel.dont_move = false;
            continue;
        }

        let mut position = *position_index; // Vect3::from_index(*position_index);

        let dir_num1 = rng.gen_range(0..=1);
        let dir_num2 = rng.gen_range(0..=1);
        let dir1 = if dir_num1 == 0 { _LEFT } else { _RIGHT };
        let dir2 = if dir_num2 == 0 { _FORWARD } else { _BACKWARD };

        let mut direction = Vec3::new(0.0, 0.0, 0.0);

        let check_directions = vec![
            _DOWN,
            _DOWN + dir1,
            _DOWN + dir2,
        ];

        for dir in check_directions.iter() {
            // Check if it can move - the position is an air spot
            let can_move = check_pos(&pixel_transforms_clone, position, position + *dir);

            // Check swap rules - say, water is in the spot to move to
            // This should probably be moved...
            if let Some(pix_at_dir) = pixel_transforms_clone.get(&(position + *dir)) {
                if pix_at_dir.pixel_type == PixelType::Water {
                    // swap places
                    let temp_pixel = pix_at_dir.clone();
                    pixel_transforms_clone.insert(position + *dir, pixel.clone());
                    pixel_transforms_clone.insert(position, temp_pixel);
                    // direction = Vec3::new(0.0, 0.0, 0.0);
                    break;
                }
            }

            if (can_move) {
                direction = *dir;

                position += direction;
                pixel_transforms_clone.insert(position, pixel.clone());
                pixel_transforms_clone.remove(position_index);
                break;
            }
        }

        // if !check_pos(&pixel_transforms_clone, position, position) {
        //     direction = Vec3::new(0.0, 1.0, 0.0);
        //     pixel.dont_move = true;
        // } else
        // if check_pos(&pixel_transforms_clone, position, position + _DOWN) {
        //     // Already set to down
        //     // direction = _Down;
        // } else if check_pos(&pixel_transforms_clone, position,position + _DOWN + dir1) {
        //     direction = _DOWN + dir1;
        // } else if check_pos(&pixel_transforms_clone, position,position + _DOWN + dir2) {
        //     direction = _DOWN + dir2;
        // // } else if check_pos(position + dir1, &pixel_transforms, None) {
        // //     direction = dir1;
        // // } else if check_pos(position + dir2, &pixel_transforms, None) {
        // //     direction = dir2;
        // } else {
        //     // pixel.dont_move = true;
        //     continue;
        // }

        // pixel.dont_move = true;
        // transform.translation += direction;
        // // pixel_transforms[id] += direction;
        // // pixel_transforms.iter_mut().find(|val| val.entity == pixel.id).unwrap().transform.translation += direction;
        // pixel_transforms.positions.iter_mut().find(|val| val.entity == pixel.id).unwrap_or(&mut PixelTransform {
        //     transform: Transform::from_xyz(0.0, 0.0, 0.0),
        //     entity: pixel.id
        // }).transform.translation += direction;

        // pixel.dont_move = true;
        is_dirty = true;
    }
    pixel_transforms.is_map_dirty = is_dirty;
    pixel_transforms.map = pixel_transforms_clone;
    pixel_transforms = pixel_transforms;
}
