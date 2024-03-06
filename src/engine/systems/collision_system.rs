use bevy::asset::Assets;
use bevy::pbr::StandardMaterial;
use bevy::prelude::{Commands, Mesh, Query, Res, ResMut, Time, Transform, With};
use crate::Pixel;

pub fn collision_system(
    mut pixels: Query<(&mut Pixel, &mut Transform)>,
) {
    // for transform1I in 0..query.iter().len() {
    //     for mut transform2 in &query {
    let mut combinations = pixels.iter_combinations_mut();
    while let Some([(mut pix1, mut transform1), (mut pix2, mut transform2)]) = combinations.fetch_next() {
        // if (transform1.translation.y == transform2.translation.y) {
        //     transform1.translation.y += 1.0;
        // }
        if (transform1.translation.x == transform2.translation.x && transform1.translation.z == transform2.translation.z) {
            if (transform1.translation.y == transform2.translation.y) {
                transform1.translation.y += 1.0;
            }

            if (transform1.translation.y - 0.5 < transform2.translation.y + 0.5 && transform1.translation.y - 0.5 > transform2.translation.y - 0.5) {
                // if (transform1.translation.y - 0.5 < transform2.translation.y + 0.5) {
                transform1.translation.y += (transform2.translation.y + 0.5) - (transform1.translation.y - 0.5);
                // continue;
            }
            if (transform1.translation.y - 0.5 == transform2.translation.y + 0.5) {
                pix1.dont_move = true;
            }
            // if bottom1 is same as top2, set a don't move flag

            // if (transform1.translation.y + 0.5 < transform2.translation.y + 0.5 && transform1.translation.y + 0.5 > transform2.translation.y - 0.5) {
            //     // if (transform1.translation.y - 0.5 < transform2.translation.y + 0.5) {
            //     transform2.translation.y += (transform1.translation.y + 0.5) - (transform2.translation.y - 0.5);
            //     // continue;
            // }

        }
    }
    //     // mutably access components data
    //
    //     // Find out if there is something below
    //     // If so, return
    //     if (transform1.translation.x == transform2.translation.x && transform1.translation.z == transform2.translation.z) {
    //         if (transform1.translation.y < transform2.translation.y) {
    //             transform2.translation.y = transform1.translation.y + 2.0;
    //         }
    //         // if (transform1.translation.y > transform2.translation.y) {
    //         //     transform1.translation.y = transform2.translation.y;
    //         // }
    //     }
    //
    //
    // // }
    // }

    // gravity
    // for mut transform in &mut rigid_body {
    //     if (transform.translation.y > 0.5) {
    //         transform.translation.y -= 9.8 * time.delta_seconds();
    //         if (transform.translation.y < 0.5) {
    //             transform.translation.y = 0.5
    //         }
    //     }
    // }

    // unsafe {
    //     // TIMER += time.delta_seconds();
    //     // if (TIMER > 0.000001) {
    //         // let shape = meshes.get_mut(Cuboid::default()).unwrap();
    //     for i in 0..10 {
    //         spawn_cube(&mut commands, meshes.add(Cuboid {
    //             half_size: Vec3 {
    //                 x: 0.5,
    //                 y: 0.5,
    //                 z: 0.5,
    //             }
    //         }), &mut materials);
    //     }
    //     //     TIMER = 0.0;
    //     // }
    //
    // }
}